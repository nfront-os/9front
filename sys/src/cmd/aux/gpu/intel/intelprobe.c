/*
 * SPDX-License-Identifier: MIT
 *
 * intelprobe: first steps with an Intel GPU of generation 8 (Broadwell,
 * HD 5500 8086:1616 and kin) over #G (gpu(3)), before a driver:
 *
 *	intelprobe [-g #G/n] probe	what the device is and in what state:
 *					bars, GTT and stolen memory, forcewake,
 *					the blitter ring, the display planes;
 *					reads only
 *	intelprobe [-g #G/n] blt		the first command: the blitter (BCS) in
 *					ring buffer mode fills a red rectangle
 *					on the visible plane, writes a sequence
 *					number to its status page; checked
 *					through the aperture
 *	intelprobe [-g #G/n] batch	the same from a batch buffer the ring
 *					starts (green), and MI_USER_INTERRUPT
 *					through #G/n/irq
 *	intelprobe [-g #G/n] peek reg...	MMIO registers
 *
 * Register offsets and command formats as in Intel's PRMs (Broadwell,
 * vol. 2 and 5) and Linux's i915 for gen 8.
 */
#include <u.h>
#include <libc.h>

enum {
	/* PCI configuration */
	Pcirev		= 0x08,
	Pcigmch		= 0x50,		/* GMCH graphics control (16 bits) */
	Pcibdsm		= 0x5C,		/* base of stolen memory */

	/* power: forcewake for the render well (gen 8: render and blitter) */
	Forcewakemt	= 0xA188,
	Forcewakeack	= 0x130044,

	/* GTT */
	Gfxflshcntl	= 0x101008,	/* GFX_FLSH_CNTL_GEN6: GTT writes visible */
	Ppat0		= 0x40E0,	/* GEN8_PRIVATE_PAT_LO, _HI */
	Ppat1		= 0x40E4,

	/* the blitter's ring (BCS) */
	Bcs		= 0x22000,
	Rtail		= 0x30,
	Rhead		= 0x34,
	Rstart		= 0x38,
	Rctl		= 0x3C,
	Ripeir		= 0x64,
	Ripehr		= 0x68,
	Rinstdone	= 0x6C,
	Racthd		= 0x74,
	Rhws		= 0x80,		/* RING_HWS_PGA */
	Rmimode		= 0x9C,
	Rimr		= 0xA8,
	Reir		= 0xB0,
	Resr		= 0xB8,
	Rmode		= 0x29C,	/* RING_MODE_GEN7: GFX_RUN_LIST_ENABLE (bit 15) */

	Stopring	= 1<<8,		/* MI_MODE */
	Ringvalid	= 1<<0,		/* RING_CTL */
	Runlist		= 1<<15,	/* execlists */

	/* display, per pipe (A, B, C: + n*0x1000) */
	Pipesrc		= 0x6001C,
	Pipeconf	= 0x70008,
	Dspcntr		= 0x70180,
	Dspstride	= 0x70188,
	Dspsurf		= 0x7019C,

	Ppstatus	= 0xC7200,	/* eDP panel power */

	/* commands */
	Minoop		= 0,
	Miuserintr	= 0x02<<23,
	Miflushdw	= 0x26<<23 | 1,	/* +1: gen 8, 64-bit address */
	Flushstoredw	= 1<<14,
	Flushstoreidx	= 1<<21,
	Flushusegtt	= 1<<2,
	Mibbstart	= 0x31<<23 | 1,	/* MI_BATCH_BUFFER_START, gen 8: 3 dwords, GGTT */
	Mibbend		= 0x0A<<23,
	Xycolorblt	= 2<<29 | 0x50<<22,
	Bltwritergba	= 3<<20,
	Blttiled	= 1<<11,

	Hwsseqno	= 0x30,		/* dword of the status page with the sequence number */
	Seqno		= 0x9f0001,

	/* interrupts (gen 8) */
	Masterirq	= 0x44200,	/* GEN8_MASTER_IRQ: bit 31 enables */
	Gtisr0		= 0x44300,	/* GEN8_GT_ISR(0), IMR +4, IIR +8, IER +0xC */
	Bcsirqshift	= 16,		/* the blitter's bits in GT bank 0 */
	Userintr	= 1<<0,		/* GT_RENDER_USER_INTERRUPT */

	Segva		= 0x50000000,	/* our pages: ring, status page, batch */
	Npage		= 3,
};

static char *gdir = "#G/0";
static int force;
static vlong tstart;

/* how long since the start: where the time goes */
static void
tick(char *what)
{
	print("t+%lldms %s\n", (nsec() - tstart)/1000000, what);
}
static char pcidev[32];
static u32int *mmio;
static uvlong mmiosize;
static uvlong *gtt;
static uchar *aper;
static uvlong apersize;

static char*
readfile(char *path, char *buf, int n)
{
	int fd, m;

	if((fd = open(path, OREAD)) < 0)
		return nil;
	m = readn(fd, buf, n-1);
	close(fd);
	if(m < 0)
		return nil;
	buf[m] = 0;
	return buf;
}

/* the value of the line key in #G/n/ctl */
static char*
ctlline(char *key, char *buf, int n)
{
	char *s, *e, path[64];
	int l;

	snprint(path, sizeof path, "%s/ctl", gdir);
	if(readfile(path, buf, n) == nil)
		sysfatal("read %s: %r", path);
	l = strlen(key);
	for(s = buf; *s; s = e){
		if((e = strchr(s, '\n')) != nil)
			*e++ = 0;
		else
			e = s+strlen(s);
		if(strncmp(s, key, l) == 0 && s[l] == ' ')
			return s+l+1;
	}
	return nil;
}

static void*
bar(int i, uvlong *size)
{
	char buf[1024], key[8], *f[5], *s;
	void *p;

	snprint(key, sizeof key, "bar%d", i);
	if((s = ctlline(key, buf, sizeof buf)) == nil || tokenize(s, f, nelem(f)) < 3 || strcmp(f[2], "-") == 0)
		sysfatal("%s: no memory %s", gdir, key);
	*size = strtoull(f[1], nil, 0);
	p = segattach(0, f[2], nil, *size);
	if(p == (void*)-1)
		sysfatal("segattach %s: %r", f[2]);
	return p;
}

static ulong
cfg(int off, int n)
{
	char path[64];
	uchar b[4];
	int fd;

	snprint(path, sizeof path, "#$/pci/%sraw", pcidev);
	if((fd = open(path, OREAD)) < 0)
		sysfatal("open %s: %r", path);
	if(pread(fd, b, n, off) != n)
		sysfatal("read %s: %r", path);
	close(fd);
	return n == 4 ? b[0] | b[1]<<8 | b[2]<<16 | (ulong)b[3]<<24 : n == 2 ? b[0] | b[1]<<8 : b[0];
}

static u32int
rd(ulong r)
{
	return ((volatile u32int*)mmio)[r/4];
}

static void
wr(ulong r, u32int v)
{
	((volatile u32int*)mmio)[r/4] = v;
}

/* masked registers: the upper half says which bits of the lower to write */
static u32int
masked(u32int bits, int on)
{
	return bits<<16 | (on ? bits : 0);
}

static void
flushcache(void *p, ulong n)
{
	uchar *a, *e;

	a = (uchar*)((uintptr)p & ~63);
	e = (uchar*)p+n;
#if defined(__x86_64__) || defined(__i386__)
	for(; a < e; a += 64)
		__asm__ volatile("clflush (%0)" :: "r"(a) : "memory");
	__asm__ volatile("mfence" ::: "memory");
#else
	USED(a, e);	/* Intel GPUs are on x86 */
#endif
}

static void
init(void)
{
	char buf[1024], *s, *f[3];

	if((s = ctlline("pci", buf, sizeof buf)) == nil || tokenize(s, f, nelem(f)) < 1)
		sysfatal("%s: no pci line", gdir);
	snprint(pcidev, sizeof pcidev, "%s", f[0]);
	/* registers of another device are not to be written */
	if((s = ctlline("id", buf, sizeof buf)) == nil || strncmp(s, "8086/", 5) != 0)
		if(!force)
			sysfatal("%s is %s, not Intel's (-f)", gdir, s ? s : "?");
	mmio = bar(0, &mmiosize);
	gtt = (uvlong*)((uchar*)mmio + mmiosize/2);	/* gen 8: the GTT is the upper half of GTTMMADR */
	aper = bar(2, &apersize);
}

/* GTT size and stolen memory from GMCH control (gen 8 encoding) */
static void
gmch(uvlong *gttsize, uvlong *stolen)
{
	ulong g;
	int s;

	g = cfg(Pcigmch, 2);
	s = g>>6 & 3;
	*gttsize = s ? (1ULL<<s)<<20 : 0;
	*stolen = (uvlong)(g>>8 & 0xff) << 25;
}

static int
forcewake(int on)
{
	int i;

	wr(Forcewakemt, masked(1, on));
	for(i = 0; i < 500; i++){
		if((rd(Forcewakeack) & 1) == on)
			return 0;
		sleep(1);
	}
	return -1;
}

static void
ringstate(char *what)
{
	print("%s: head %#ux tail %#ux start %#ux ctl %#ux acthd %#ux mode %#ux mimode %#ux\n", what,
		rd(Bcs+Rhead), rd(Bcs+Rtail), rd(Bcs+Rstart), rd(Bcs+Rctl), rd(Bcs+Racthd),
		rd(Bcs+Rmode), rd(Bcs+Rmimode));
	print("%s: ipeir %#ux ipehr %#ux instdone %#ux eir %#ux esr %#ux hws %#ux imr %#ux\n", what,
		rd(Bcs+Ripeir), rd(Bcs+Ripehr), rd(Bcs+Rinstdone), rd(Bcs+Reir), rd(Bcs+Resr),
		rd(Bcs+Rhws), rd(Bcs+Rimr));
}

/* the plane that shows something: -1 if none */
static int
plane(void)
{
	int p;

	for(p = 0; p < 3; p++)
		if(rd(Dspcntr + p*0x1000) & 1<<31)
			return p;
	return -1;
}

static void
probe(void)
{
	char buf[1024], *s;
	uvlong gttsize, stolen;
	int p;

	s = ctlline("id", buf, sizeof buf);
	print("device %s pci %s rev %#lux\n", s ? s : "?", pcidev, cfg(Pcirev, 1));
	print("mmio %#llux bytes, aperture %#llux bytes\n", mmiosize, apersize);
	gmch(&gttsize, &stolen);
	print("gmch %#lux: gtt %lld KB (%lld entries, %lld MB of address space), stolen %lld MB at %#lux\n",
		cfg(Pcigmch, 2), gttsize>>10, gttsize/8, gttsize/8*4096>>20, stolen>>20, cfg(Pcibdsm, 4) & ~0xFFFFFUL);
	print("ppat %#.8ux %#.8ux\n", rd(Ppat1), rd(Ppat0));
	print("forcewake ack %#ux", rd(Forcewakeack));
	if(forcewake(1) < 0)
		print(", no ack: %#ux\n", rd(Forcewakeack));
	else
		print(", ack after request\n");
	ringstate("bcs");
	for(p = 0; p < 3; p++)
		print("pipe %c: conf %#.8ux src %#.8ux plane %#.8ux stride %#ux surf %#.8ux\n", 'A'+p,
			rd(Pipeconf + p*0x1000), rd(Pipesrc + p*0x1000), rd(Dspcntr + p*0x1000),
			rd(Dspstride + p*0x1000), rd(Dspsurf + p*0x1000));
	print("panel power %#ux\n", rd(Ppstatus));
	if((p = plane()) >= 0 && (rd(Dspsurf + p*0x1000)/4096)*8 < gttsize)
		print("gtt[surf] %#.16llux\n", gtt[rd(Dspsurf + p*0x1000)/4096]);
	forcewake(0);
}

static uvlong
fixedseg(char *name, uintptr va, ulong size)
{
	char buf[256], path[64], *f[6];
	int fd, n;

	snprint(path, sizeof path, "#g/%s", name);
	if((fd = create(path, OREAD, DMDIR|0700)) >= 0)
		close(fd);
	snprint(path, sizeof path, "#g/%s/ctl", name);
	if((fd = open(path, ORDWR)) < 0)
		return 0;
	fprint(fd, "va %#p %#lux fixed", va, size);
	n = pread(fd, buf, sizeof buf-1, 0);
	close(fd);
	if(n <= 0)
		return 0;
	buf[n] = 0;
	if(tokenize(buf, f, nelem(f)) < 5 || strcmp(f[3], "fixed") != 0){
		werrstr("%s: not a fixed segment: %s", name, buf);
		return 0;
	}
	return strtoull(f[4], nil, 0);
}

/* what the commands work on: our pages in the GTT, the visible plane */
typedef struct Gpu Gpu;
struct Gpu {
	u32int	*ring;		/* page 0 */
	u32int	*hws;		/* page 1: the status page */
	u32int	*batch;		/* page 2 */
	ulong	ringgtt, hwsgtt, batchgtt;
	uvlong	nent;		/* GTT entries */
	u32int	surf, stride;
	int	tiled, bpp;
};

static void
ctlwrite(char *msg)
{
	char path[64];
	int fd;

	snprint(path, sizeof path, "%s/ctl", gdir);
	if((fd = open(path, OWRITE)) < 0 || fprint(fd, "%s", msg) < 0)
		sysfatal("%s: %s: %r", path, msg);
	close(fd);
}

static void
setup(Gpu *g)
{
	uvlong gttsize, stolen, pa;
	u32int cntr;
	int p, i, fmt;

	if((p = plane()) < 0)
		sysfatal("no plane is on");
	cntr = rd(Dspcntr + p*0x1000);
	g->surf = rd(Dspsurf + p*0x1000) & ~0xFFF;
	g->stride = rd(Dspstride + p*0x1000);
	g->tiled = (cntr & 1<<10) != 0;
	fmt = cntr>>26 & 0xF;
	switch(fmt){
	case 5:		/* DISPPLANE_BGRX565 */
		g->bpp = 16;
		break;
	case 6:		/* DISPPLANE_BGRX888 */
	case 7:		/* DISPPLANE_BGRA888 */
		g->bpp = 32;
		break;
	default:
		sysfatal("plane format %d: not 16 or 32 bits", fmt);
	}
	print("plane %c: cntr %#ux surf %#ux stride %ud, %d bits%s\n", 'A'+p, cntr, g->surf, g->stride,
		g->bpp, g->tiled ? ", tiled" : "");

	/* bus mastering, our pages, the last entries of the GTT */
	tick("bme on");
	ctlwrite("bme on");
	tick("fixed segment");
	if((pa = fixedseg("intelprobe", Segva, Npage*4096)) == 0)
		sysfatal("#g/intelprobe: %r");
	tick("segattach");
	if((g->ring = segattach(0, "intelprobe", nil, Npage*4096)) == (void*)-1)
		sysfatal("segattach intelprobe: %r");
	tick("pages");
	g->hws = g->ring + 1024;
	g->batch = g->ring + 2*1024;
	memset(g->ring, 0, Npage*4096);
	gmch(&gttsize, &stolen);
	g->nent = gttsize/8;
	if(g->nent < 1024)
		sysfatal("gtt of %lld entries", g->nent);
	g->ringgtt = (g->nent-Npage)*4096;
	g->hwsgtt = g->ringgtt + 4096;
	g->batchgtt = g->ringgtt + 2*4096;
	print("pages at %#llux, gtt %#lux (ring) %#lux (status) %#lux (batch)\n", pa,
		g->ringgtt, g->hwsgtt, g->batchgtt);

	tick("forcewake");
	if(forcewake(1) < 0)
		sysfatal("forcewake: no ack (%#ux)", rd(Forcewakeack));
	tick("forcewake acknowledged");
	ringstate("before");

	/* PTEs: present, PPAT 0; then the GTT flushed */
	for(i = 0; i < Npage; i++)
		gtt[g->nent-Npage+i] = (pa + i*4096) | 1;
	wr(Gfxflshcntl, 1);
	rd(Gfxflshcntl);
	tick("gtt");
}

/* the ring in ring buffer mode (no execlists), 1 page */
static void
startring(Gpu *g)
{
	wr(Bcs+Rmode, masked(Runlist, 0));
	wr(Bcs+Rctl, 0);
	wr(Bcs+Rhead, 0);
	wr(Bcs+Rtail, 0);
	wr(Bcs+Rhws, g->hwsgtt);
	wr(Bcs+Rstart, g->ringgtt);
	wr(Bcs+Rctl, (1-1)<<12 | Ringvalid);
	wr(Bcs+Rmimode, masked(Stopring, 0));
	ringstate("ready");
}

static void
teardown(Gpu *g)
{
	int i;

	wr(Bcs+Rmimode, masked(Stopring, 1));
	sleep(10);
	wr(Bcs+Rctl, 0);
	for(i = 0; i < Npage; i++)
		gtt[g->nent-Npage+i] = 0;
	wr(Gfxflshcntl, 1);
	forcewake(0);
}

/* XY_COLOR_BLT of the rectangle into the plane: 7 dwords at c */
static int
fill(Gpu *g, u32int *c, int x0, int y0, int x1, int y1, ulong rgb)
{
	ulong color, depth;
	int n;

	if(g->bpp == 16){
		depth = 1;	/* 565 */
		color = (rgb>>8 & 0xF800) | (rgb>>5 & 0x07E0) | (rgb>>3 & 0x001F);
	}else{
		depth = 3;	/* 8888 */
		color = rgb & 0xFFFFFF;
	}
	n = 0;
	c[n++] = Xycolorblt | (g->bpp == 32 ? Bltwritergba : 0) | (g->tiled ? Blttiled : 0) | (7-2);
	c[n++] = depth<<24 | 0xF0<<16 | (g->tiled ? g->stride/4 : g->stride);	/* PATCOPY */
	c[n++] = y0<<16 | x0;
	c[n++] = y1<<16 | x1;
	c[n++] = g->surf;
	c[n++] = 0;
	c[n++] = color;
	return n;
}

/* the flush with the sequence number to the status page, and an interrupt: 6 dwords */
static int
breadcrumb(u32int *c)
{
	int n;

	n = 0;
	c[n++] = (Miflushdw+1) | Flushstoredw | Flushstoreidx;
	c[n++] = Hwsseqno*4 | Flushusegtt;
	c[n++] = 0;
	c[n++] = Seqno;
	c[n++] = Miuserintr;
	c[n++] = Minoop;
	return n;
}

static int
waitseqno(Gpu *g)
{
	vlong t0;

	t0 = nsec();
	for(;;){
		flushcache(&g->hws[Hwsseqno], 4);
		if(g->hws[Hwsseqno] == Seqno)
			break;
		if(nsec() - t0 > 2000000000LL){
			print("FAIL: no sequence number after 2 s (status page %#ux)\n", g->hws[Hwsseqno]);
			ringstate("after");
			return -1;
		}
		sleep(1);
	}
	print("ok: sequence number after %lld µs\n", (nsec() - t0)/1000);
	ringstate("after");
	return 0;
}

/* the pixel in the middle of the rectangle, through the aperture */
static void
readback(Gpu *g, int x, int y, ulong rgb)
{
	uvlong off;
	ulong want, got;

	off = (uvlong)g->surf + (uvlong)y*g->stride + x*(g->bpp/8);
	if(g->tiled){
		print("tiled plane: look at the screen (the rectangle at %d,%d)\n", x, y);
		return;
	}
	if(off + 4 > apersize){
		print("the plane is beyond the aperture: look at the screen\n");
		return;
	}
	if(g->bpp == 16){
		want = (rgb>>8 & 0xF800) | (rgb>>5 & 0x07E0) | (rgb>>3 & 0x001F);
		got = *(u16int*)(aper + off);
	}else{
		want = rgb & 0xFFFFFF;
		got = *(u32int*)(aper + off) & 0xFFFFFF;
	}
	if(got == want)
		print("ok: the rectangle is in the framebuffer (%#lux)\n", got);
	else
		print("FAIL: framebuffer at %d,%d is %#lux, want %#lux\n", x, y, got, want);
}

static void
blt(void)
{
	Gpu g;
	int n;

	setup(&g);
	n = fill(&g, g.ring, 100, 100, 300, 200, 0xFF0000);
	n += breadcrumb(g.ring+n);
	if(n & 1)
		g.ring[n++] = Minoop;
	flushcache(g.ring, Npage*4096);
	startring(&g);
	wr(Bcs+Rtail, n*4);
	if(waitseqno(&g) == 0)
		readback(&g, 150, 150, 0xFF0000);
	teardown(&g);
}

static void
catch(void*, char *msg)
{
	if(strstr(msg, "alarm") != nil)
		noted(NCONT);
	noted(NDFLT);
}

/*
 * the fill from a batch buffer, which the ring starts as a driver
 * would; the ring's MI_USER_INTERRUPT through #G/n/irq
 */
static void
batch(void)
{
	char path[64], buf[32], buf2[1024], *s;
	Gpu g;
	int n, m, fd;
	u32int master, iir;
	vlong t0;

	setup(&g);
	m = fill(&g, g.batch, 350, 100, 550, 200, 0x00FF00);
	g.batch[m++] = Mibbend;
	if(m & 1)
		g.batch[m++] = Minoop;
	n = 0;
	g.ring[n++] = Mibbstart;
	g.ring[n++] = g.batchgtt;
	g.ring[n++] = 0;
	n += breadcrumb(g.ring+n);
	if(n & 1)
		g.ring[n++] = Minoop;
	flushcache(g.ring, Npage*4096);

	/* the blitter's user interrupt, and only that */
	snprint(path, sizeof path, "%s/irq", gdir);
	if((fd = open(path, OREAD)) < 0)
		sysfatal("open %s: %r", path);
	wr(Gtisr0+8, ~0);			/* IIR: old ones off */
	wr(Gtisr0+0xC, Userintr<<Bcsirqshift);	/* IER */
	wr(Gtisr0+4, ~(Userintr<<Bcsirqshift));	/* IMR */
	wr(Masterirq, 1<<31);
	print("irq: master %#ux gt isr %#ux imr %#ux ier %#ux\n", rd(Masterirq), rd(Gtisr0),
		rd(Gtisr0+4), rd(Gtisr0+0xC));

	startring(&g);
	/*
	 * the ring's own mask after its start, which sets it again; on gen 8
	 * in GT bank 0's layout (i915: GT_RENDER_USER_INTERRUPT <<
	 * GEN8_BCS_IRQ_SHIFT), only its implemented bits read back
	 */
	wr(Bcs+Rimr, ~(Userintr<<Bcsirqshift));
	print("ring imr %#ux\n", rd(Bcs+Rimr));
	t0 = nsec();
	wr(Bcs+Rtail, n*4);
	notify(catch);
	alarm(2000);
	m = read(fd, buf, sizeof buf-1);
	alarm(0);
	if(m <= 0)
		print("FAIL: no interrupt within 2 s (%r)\n");
	else{
		buf[m] = 0;
		master = rd(Masterirq);
		iir = rd(Gtisr0+8);
		print("ok: interrupt %s after %lld µs: master %#ux gt iir %#ux\n", buf,
			(nsec() - t0)/1000, master, iir);
		wr(Gtisr0+8, iir);		/* acknowledged */
		ctlwrite("irq ack");
	}
	if(waitseqno(&g) == 0)
		readback(&g, 450, 150, 0x00FF00);
	/* where the interrupt is: the GPU's registers, the kernel's count */
	print("irq after: master %#ux gt isr %#ux iir %#ux; #G: irq %s\n", rd(Masterirq), rd(Gtisr0),
		rd(Gtisr0+8), (s = ctlline("irq", buf2, sizeof buf2)) ? s : "?");

	/* interrupts off again */
	wr(Masterirq, 0);
	wr(Gtisr0+0xC, 0);
	wr(Gtisr0+4, ~0);
	wr(Bcs+Rimr, ~0);
	close(fd);
	teardown(&g);
}

static void
peek(int argc, char **argv)
{
	ulong r;
	int i;

	forcewake(1);
	for(i = 0; i < argc; i++){
		r = strtoul(argv[i], nil, 0);
		if(r+4 > mmiosize/2){
			print("%#lux: beyond the registers\n", r);
			continue;
		}
		print("%#.6lux %#.8ux\n", r, rd(r));
	}
	forcewake(0);
}

static void
usage(void)
{
	fprint(2, "usage: %s [-f] [-g #G/n] probe | blt | batch | peek reg...\n", argv0);
	exits("usage");
}

void
main(int argc, char **argv)
{
	ARGBEGIN{
	case 'f':
		force = 1;
		break;
	case 'g':
		gdir = EARGF(usage());
		break;
	default:
		usage();
	}ARGEND
	if(argc < 1)
		usage();
	tstart = nsec();
	init();
	tick("init");
	if(strcmp(argv[0], "probe") == 0)
		probe();
	else if(strcmp(argv[0], "blt") == 0)
		blt();
	else if(strcmp(argv[0], "batch") == 0)
		batch();
	else if(strcmp(argv[0], "peek") == 0)
		peek(argc-1, argv+1);
	else
		usage();
	exits(nil);
}
