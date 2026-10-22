/*
 * SPDX-License-Identifier: MIT
 *
 * intelgpu: user-space driver for Intel GPUs of generation 8
 * (Broadwell) over #G (gpu(3)), serving gpu(4)'s files: contexts
 * with buffer objects in the global GTT, batch buffers on the
 * render (RCS) and blitter (BCS) engines in ring buffer mode, fences
 * from the engines' sequence numbers and the user interrupt (MSI).
 * The display stays as the firmware or aux/vga set it up.
 *
 *	/dev/gpu/0/ctl			the visible plane: plane gpuaddr stride bpp w h
 *	/dev/gpu/0/status		device, engines, memory, contexts
 *	/dev/gpu/0/clone		a new context, ctx/n
 *	/dev/gpu/0/ctx/n/ctl		bo size, free id
 *	/dev/gpu/0/ctx/n/submit		bo offset [rcs|bcs]
 *	/dev/gpu/0/ctx/n/fence		as gpu(4)
 *
 * A buffer object is a #g segment of physically contiguous memory
 * (fixed), mapped into the GTT; the client attaches it and writes
 * GPU addresses into its batches itself (no relocations).  Every
 * client sees the whole GTT: there is no isolation (PPGTT) yet.
 *
 * usage: aux/intelgpu [-dpU] [-g #G/n] [-s srvname]
 *
 * Register offsets and command formats as in Intel's PRMs (Broadwell,
 * vol. 2 and 5) and Linux's i915 for gen 8.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>

enum {
	Pcigmch		= 0x50,		/* GMCH graphics control (16 bits) */

	Forcewakemt	= 0xA188,
	Forcewakeack	= 0x130044,
	Gfxflshcntl	= 0x101008,	/* GTT writes visible */
	Ppatlo		= 0x40E0,	/* GEN8_PRIVATE_PAT_LO, _HI */
	Ppathi		= 0x40E4,

	/* per engine, from its base */
	Rtail		= 0x30,
	Rhead		= 0x34,
	Rstart		= 0x38,
	Rctl		= 0x3C,
	Ripehr		= 0x68,
	Racthd		= 0x74,
	Rhws		= 0x80,
	Rmimode		= 0x9C,
	Rimr		= 0xA8,
	Reir		= 0xB0,
	Resr		= 0xB8,
	Rmode		= 0x29C,

	Stopring	= 1<<8,
	Ringvalid	= 1<<0,
	Runlist		= 1<<15,
	Headmask	= 0x1FFFFC,

	/* interrupts */
	Masterirq	= 0x44200,
	Mastercontrol	= 1<<31,
	Gtisr0		= 0x44300,	/* bank 0: RCS at 0, BCS at 16; IMR +4, IIR +8, IER +0xC */
	Userintr	= 1<<0,

	/* display */
	Pipesrc		= 0x6001C,
	Dspcntr		= 0x70180,
	Dspstride	= 0x70188,
	Dspsurf		= 0x7019C,

	/* commands */
	Minoop		= 0,
	Miuserintr	= 0x02<<23,
	Mibbstart	= 0x31<<23 | 1,	/* gen 8: 3 dwords, GGTT */
	Miflushdw	= 0x26<<23 | 1,
	Flushstoredw	= 1<<14,
	Flushstoreidx	= 1<<21,
	Flushusegtt	= 1<<2,
	Pipecontrol	= 3<<29 | 3<<27 | 2<<24 | (6-2),
	Pccsstall	= 1<<20,
	Pcqwwrite	= 1<<14,
	Pcglobalgtt	= 1<<24,
	Pcrtflush	= 1<<12,
	Pcdcflush	= 1<<5,
	Pcdepthflush	= 1<<0,
	Pctlbinval	= 1<<18,

	/* PTEs: present, PPAT index by PWT (3), PCD (4), PAT (7) */
	Ptepresent	= 1,
	Ptecached	= 1<<7,		/* index 4: write-back, LLC (ppatinit) */
	Pteuncached	= 1<<3 | 1<<4,	/* index 3: uncached */

	Hwsseqno	= 0x30,		/* dword of the status page with the sequence number */

	Ringpages	= 4,
	Gttlow		= 0x40000000,	/* our part of the GTT: from 1 GB to its end */
	Bova		= 0x2000000000ULL,	/* the objects' addresses in the clients */
	Drvva		= 0x50000000,	/* the driver's pages: rings, status pages */

	Nctx		= 64,
	Nbo		= 1024,
	Fwin		= 256,		/* fences in flight per context */
	Timeout		= 5000,		/* ms before a fence is given up */
};

typedef struct Engine Engine;
typedef struct Bo Bo;
typedef struct Ctx Ctx;
typedef struct Fidaux Fidaux;
typedef struct Hole Hole;

struct Engine {
	char	*name;
	ulong	base;
	int	irqshift;
	u32int	*ring;		/* Ringpages pages */
	u32int	*hws;		/* the status page */
	ulong	ringgtt, hwsgtt;
	ulong	tail;		/* bytes */
	u32int	seqno;		/* last emitted */
	int	ok;
};

struct Bo {
	int	id;
	Ctx	*ctx;
	char	seg[32];
	uvlong	va, pa, size;
	ulong	gtt;
};

struct Ctx {
	int	id;
	int	ref;
	char	*reply;
	uvlong	nfence;		/* submitted */
	uvlong	donefence;	/* complete, with all before it */
	Engine	*fe[Fwin];	/* fence k: on engine fe[k%Fwin] */
	u32int	fseq[Fwin];	/* with sequence number fseq[k%Fwin] */
	vlong	ftime[Fwin];	/* submitted then */
	int	failed;
	File	*dir, *ctl, *submit, *fence;
};

struct Fidaux {
	Ctx	*ctx;
	uvlong	fence;
	int	query;
};

struct Hole {
	Hole	*next;
	ulong	start, len;	/* GTT, bytes */
};

static char *gdir = "#G/0";
static int debug, poll, uncached;
static int gctlfd = -1, irqfd = -1;
static u32int *mmio;
static uvlong mmiosize;
static uvlong *gtt;
static uvlong nent;
static uvlong drvpa;
static u32int *drv;
static Hole *holes;
static Engine eng[] = {
	{ "rcs", 0x2000, 0 },
	{ "bcs", 0x22000, 16 },
};
static Ctx *ctxs[Nctx];
static Bo *bos[Nbo];
static File *ctxdir;
static Srv fs;
static QLock lk;		/* engines, fences, the GTT */
static Rendez done;		/* fences completed */
static QLock ctxlock;		/* contexts, objects */
static ulong nirq;

#define DBG	if(debug) fprint

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

static u32int
masked(u32int bits, int on)
{
	return bits<<16 | (on ? bits : 0);
}

/* the CPU's caches out of the way of an uncached GPU (-U) */
static void
flushcache(void *p, ulong n)
{
	uchar *a, *e;

	if(!uncached)
		return;
	a = (uchar*)((uintptr)p & ~63);
	e = (uchar*)p+n;
#if defined(__x86_64__) || defined(__i386__)
	for(; a < e; a += 64)
		__asm__ volatile("clflush (%0)" :: "r"(a) : "memory");
	__asm__ volatile("mfence" ::: "memory");
#else
	USED(a, e);
#endif
}

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

static ulong
cfg16(int off)
{
	char buf[1024], path[64], *s, *f[2];
	uchar b[2];
	int fd;

	if((s = ctlline("pci", buf, sizeof buf)) == nil || tokenize(s, f, nelem(f)) < 1)
		sysfatal("%s: no pci line", gdir);
	snprint(path, sizeof path, "#$/pci/%sraw", f[0]);
	if((fd = open(path, OREAD)) < 0 || pread(fd, b, 2, off) != 2)
		sysfatal("%s: %r", path);
	close(fd);
	return b[0] | b[1]<<8;
}

static uvlong
fixedseg(char *name, uvlong va, ulong size, int perm)
{
	char buf[256], path[64], *f[6];
	int fd, n;

	snprint(path, sizeof path, "#g/%s", name);
	if((fd = create(path, OREAD, DMDIR|perm)) >= 0)
		close(fd);
	snprint(path, sizeof path, "#g/%s/ctl", name);
	if((fd = open(path, ORDWR)) < 0)
		return 0;
	fprint(fd, "va %#llux %#lux fixed", va, size);
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

static void
rmseg(char *name)
{
	char path[64];

	snprint(path, sizeof path, "#g/%s", name);
	remove(path);
}

/*
 * the GTT: our part from Gttlow, first fit
 */
static ulong
gttalloc(ulong len)
{
	Hole *h, **l;
	ulong a;

	len = (len + 4095) & ~4095;
	for(l = &holes; (h = *l) != nil; l = &h->next)
		if(h->len >= len){
			a = h->start;
			h->start += len;
			h->len -= len;
			if(h->len == 0){
				*l = h->next;
				free(h);
			}
			return a;
		}
	return 0;
}

static void
gttfree(ulong a, ulong len)
{
	Hole *h, *p, **l;

	len = (len + 4095) & ~4095;
	for(l = &holes; (p = *l) != nil && p->start < a; l = &p->next)
		;
	h = emalloc9p(sizeof *h);
	h->start = a;
	h->len = len;
	h->next = p;
	*l = h;
	/* merge with the next, and the previous with it */
	if(p != nil && a + len == p->start){
		h->len += p->len;
		h->next = p->next;
		free(p);
	}
	for(p = holes; p != nil && p->next != h; p = p->next)
		;
	if(p != nil && p->start + p->len == h->start){
		p->len += h->len;
		p->next = h->next;
		free(h);
	}
}

/* map len bytes of contiguous memory at pa to gtt address a */
static void
gttmap(ulong a, uvlong pa, ulong len)
{
	uvlong i, n, bits;

	bits = Ptepresent | (uncached ? Pteuncached : Ptecached);
	n = (len + 4095) / 4096;
	for(i = 0; i < n; i++)
		gtt[a/4096 + i] = (pa + i*4096) | bits;
	wr(Gfxflshcntl, 1);
	rd(Gfxflshcntl);
}

static void
gttunmap(ulong a, ulong len)
{
	uvlong i, n;

	n = (len + 4095) / 4096;
	for(i = 0; i < n; i++)
		gtt[a/4096 + i] = 0;
	wr(Gfxflshcntl, 1);
	rd(Gfxflshcntl);
}

/*
 * the private PAT: index 0 stays what the firmware left (the
 * display's mappings use it), 3 uncached, 4-7 write-back in the LLC
 * (i915's bdw_setup_private_ppat)
 */
static void
ppatinit(void)
{
	u32int lo, hi;

	lo = rd(Ppatlo);
	lo = (lo & 0xFF) | 0x09<<8 | 0x0A<<16 | 0x00<<24;	/* 1 WC llc+ellc, 2 WT llc+ellc, 3 UC */
	hi = 0x07<<0 | 0x17<<8 | 0x27<<16 | 0x37<<24;		/* 4-7 WB llc, age 0-3 */
	wr(Ppatlo, lo);
	wr(Ppathi, hi);
	DBG(2, "ppat %#.8ux %#.8ux\n", rd(Ppathi), rd(Ppatlo));
}

static int
forcewake(void)
{
	int i;

	wr(Forcewakemt, masked(1, 1));
	for(i = 0; i < 500; i++){
		if(rd(Forcewakeack) & 1)
			return 0;
		sleep(1);
	}
	return -1;
}

/*
 * engines in ring buffer mode (no execlists), each with Ringpages
 * of ring and a status page
 */
static void
engineinit(Engine *e, int i)
{
	ulong r;

	e->ring = drv + i*(Ringpages+1)*1024;
	e->hws = e->ring + Ringpages*1024;
	r = gttalloc((Ringpages+1)*4096);
	if(r == 0)
		sysfatal("no GTT for the %s ring", e->name);
	gttmap(r, drvpa + i*(Ringpages+1)*4096, (Ringpages+1)*4096);
	e->ringgtt = r;
	e->hwsgtt = r + Ringpages*4096;
	memset(e->ring, 0, (Ringpages+1)*4096);
	flushcache(e->ring, (Ringpages+1)*4096);

	wr(e->base+Rmode, masked(Runlist, 0));
	wr(e->base+Rctl, 0);
	wr(e->base+Rhead, 0);
	wr(e->base+Rtail, 0);
	wr(e->base+Rhws, e->hwsgtt);
	wr(e->base+Rstart, e->ringgtt);
	wr(e->base+Rctl, (Ringpages-1)<<12 | Ringvalid);
	wr(e->base+Rmimode, masked(Stopring, 0));
	/* its user interrupt, in GT bank 0's layout, after the start */
	wr(e->base+Rimr, ~(Userintr<<e->irqshift));
	e->tail = 0;
	e->seqno = 0;
	if((rd(e->base+Rctl) & Ringvalid) == 0 || rd(e->base+Rstart) != e->ringgtt){
		fprint(2, "%s: %s ring did not start: ctl %#ux start %#ux\n", argv0, e->name,
			rd(e->base+Rctl), rd(e->base+Rstart));
		return;
	}
	e->ok = 1;
}

static void
irqinit(void)
{
	u32int mask;
	int i;

	mask = 0;
	for(i = 0; i < nelem(eng); i++)
		if(eng[i].ok)
			mask |= Userintr << eng[i].irqshift;
	wr(Gtisr0+8, ~0);		/* IIR */
	wr(Gtisr0+0xC, mask);		/* IER */
	wr(Gtisr0+4, ~mask);		/* IMR */
	wr(Masterirq, Mastercontrol);
}

/* the engine's completed sequence number */
static u32int
engdone(Engine *e)
{
	flushcache(&e->hws[Hwsseqno], 4);
	return e->hws[Hwsseqno];
}

/* room for n dwords in the ring: wait for the engine to read up to there */
static int
ringspace(Engine *e, int n)
{
	ulong head, need, size;
	vlong t0;

	size = Ringpages*4096;
	need = n*4;
	t0 = nsec();
	for(;;){
		/* the tail may not catch up with the head: 8 bytes stay free */
		head = rd(e->base+Rhead) & Headmask;
		if(head == e->tail || (head + size - e->tail) % size - 8 >= need)
			return 0;
		if(nsec() - t0 > (vlong)Timeout*1000000)
			return -1;
		sleep(1);
	}
}

static void
emit(Engine *e, u32int v)
{
	e->ring[e->tail/4] = v;
	e->tail = (e->tail + 4) % (Ringpages*4096);
}

/* the batch at gtt address a on engine e, then its sequence number; with lk */
static char*
dispatch(Engine *e, ulong a, u32int *seqno)
{
	ulong t0;

	if(!e->ok)
		return "engine not running";
	if(ringspace(e, 16) < 0)
		return "ring full (engine hung?)";
	t0 = e->tail;
	*seqno = ++e->seqno;
	emit(e, Mibbstart);
	emit(e, a);
	emit(e, 0);
	if(e->irqshift == 0){
		/* render: caches flushed, then the number; PIPE_CONTROL's qword write */
		emit(e, Pipecontrol);
		emit(e, Pccsstall | Pcrtflush | Pcdcflush | Pcdepthflush | Pcqwwrite | Pcglobalgtt);
		emit(e, e->hwsgtt + Hwsseqno*4);
		emit(e, 0);
		emit(e, *seqno);
		emit(e, 0);
	}else{
		emit(e, (Miflushdw+1) | Flushstoredw | Flushstoreidx);
		emit(e, Hwsseqno*4 | Flushusegtt);
		emit(e, 0);
		emit(e, *seqno);
		emit(e, Minoop);
		emit(e, Minoop);
	}
	emit(e, Miuserintr);
	while(e->tail & 7)	/* the tail on a qword */
		emit(e, Minoop);
	if(e->tail > t0)
		flushcache(e->ring + t0/4, e->tail - t0);
	else{
		flushcache(e->ring + t0/4, Ringpages*4096 - t0);
		flushcache(e->ring, e->tail);
	}
	wr(e->base+Rtail, e->tail);
	return nil;
}

/*
 * fences
 */
static int
fencedone(Ctx *c, uvlong k)
{
	Engine *e;

	if(k <= c->donefence)
		return 1;
	if(k > c->nfence)
		return 0;
	e = c->fe[k % Fwin];
	return e == nil || (int)(engdone(e) - c->fseq[k % Fwin]) >= 0;
}

/* advance every context's donefence; with lk */
static void
reap(void)
{
	Ctx *c;
	int i;

	for(i = 0; i < Nctx; i++){
		if((c = ctxs[i]) == nil)
			continue;
		while(c->donefence < c->nfence && fencedone(c, c->donefence+1))
			c->donefence++;
	}
}

static void
irqproc(void)
{
	char buf[32];
	u32int iir;

	for(;;){
		if(irqfd >= 0){
			if(read(irqfd, buf, sizeof buf) <= 0)
				sysfatal("read irq: %r");
			wr(Masterirq, 0);
			iir = rd(Gtisr0+8);
			wr(Gtisr0+8, iir);
			wr(Masterirq, Mastercontrol);
			fprint(gctlfd, "irq ack");
		}else
			sleep(1);
		qlock(&lk);
		nirq++;
		reap();
		rwakeupall(&done);
		qunlock(&lk);
	}
}

/* the waiters look at the time now and then: a hung engine raises no interrupt */
static void
tickproc(void)
{
	for(;;){
		sleep(100);
		qlock(&lk);
		reap();
		rwakeupall(&done);
		qunlock(&lk);
	}
}

/* waits for fence k; gives up after Timeout: the engine hung */
static int
fencewait(Ctx *c, uvlong k)
{
	qlock(&lk);
	while(!fencedone(c, k)){
		if(nsec() - c->ftime[k % Fwin] > (vlong)Timeout*1000000){
			c->failed = 1;
			qunlock(&lk);
			return -1;
		}
		rsleep(&done);
	}
	qunlock(&lk);
	return 0;
}

/*
 * contexts and their objects
 */
static char*
ctxreply(Ctx *c, char *fmt, ...)
{
	va_list arg;

	free(c->reply);
	va_start(arg, fmt);
	c->reply = vsmprint(fmt, arg);
	va_end(arg);
	return nil;
}

static Bo*
findbo(Ctx *c, int id)
{
	if(id < 1 || id > Nbo || bos[id-1] == nil || bos[id-1]->ctx != c)
		return nil;
	return bos[id-1];
}

static char*
bonew(Ctx *c, uvlong size)
{
	Bo *b;
	int i;

	if(size == 0 || size > 256*1024*1024)
		return "bad size";
	size = (size + 4095) & ~4095ULL;
	for(i = 0; i < Nbo; i++)
		if(bos[i] == nil)
			break;
	if(i == Nbo)
		return "too many objects";
	b = emalloc9p(sizeof *b);
	memset(b, 0, sizeof *b);
	b->id = i+1;
	b->ctx = c;
	b->size = size;
	b->va = Bova + (uvlong)i*256*1024*1024;
	snprint(b->seg, sizeof b->seg, "gpu0bo%d", b->id);
	if((b->pa = fixedseg(b->seg, b->va, size, 0770)) == 0){
		rmseg(b->seg);
		free(b);
		return "no contiguous memory";
	}
	qlock(&lk);
	b->gtt = gttalloc(size);
	if(b->gtt != 0)
		gttmap(b->gtt, b->pa, size);
	qunlock(&lk);
	if(b->gtt == 0){
		rmseg(b->seg);
		free(b);
		return "GTT full";
	}
	bos[i] = b;
	return ctxreply(c, "bo %d %s %#llux %#llux %#lux\n", b->id, b->seg, b->va, b->size, b->gtt);
}

static void
bofree(Bo *b)
{
	qlock(&lk);
	gttunmap(b->gtt, b->size);
	gttfree(b->gtt, b->size);
	qunlock(&lk);
	rmseg(b->seg);
	bos[b->id-1] = nil;
	free(b);
}

static char*
submit(Ctx *c, int boid, ulong off, char *engname)
{
	Engine *e;
	Bo *b;
	u32int seq;
	char *err;
	int i;

	if((b = findbo(c, boid)) == nil)
		return "unknown object";
	if(off >= b->size || (off & 7) != 0)
		return "bad offset";
	e = nil;
	for(i = 0; i < nelem(eng); i++)
		if(strcmp(eng[i].name, engname) == 0)
			e = &eng[i];
	if(e == nil)
		return "unknown engine";
	qlock(&lk);
	if(c->nfence+1 - c->donefence > Fwin){
		qunlock(&lk);
		return "too many fences in flight";
	}
	if((err = dispatch(e, b->gtt + off, &seq)) != nil){
		qunlock(&lk);
		return err;
	}
	c->nfence++;
	c->fe[c->nfence % Fwin] = e;
	c->fseq[c->nfence % Fwin] = seq;
	c->ftime[c->nfence % Fwin] = nsec();
	qunlock(&lk);
	return nil;
}

/*
 * file server
 */
enum {
	Xctl,
	Xstatus,
	Xclone,
	Xctxctl,
	Xctxsubmit,
	Xctxfence,

	Xctxbase = 0x1000,
};

static int
ftype(File *f)
{
	uintptr a;

	a = (uintptr)f->aux;
	return a >= Xctxbase ? a % Xctxbase : a;
}

static Ctx*
fctx(File *f)
{
	uintptr a;

	a = (uintptr)f->aux;
	if(a < Xctxbase)
		return nil;
	return ctxs[a/Xctxbase - 1];
}

static Ctx*
newctx(void)
{
	char name[16], *user;
	Ctx *c;
	int i;

	for(i = 0; i < Nctx; i++)
		if(ctxs[i] == nil)
			break;
	if(i == Nctx){
		werrstr("too many contexts");
		return nil;
	}
	c = emalloc9p(sizeof *c);
	memset(c, 0, sizeof *c);
	c->id = i+1;
	user = getuser();
	snprint(name, sizeof name, "%d", c->id);
	c->dir = createfile(ctxdir, name, user, DMDIR|0555, nil);
	c->ctl = createfile(c->dir, "ctl", user, 0660, (void*)(uintptr)(Xctxbase*(i+1)+Xctxctl));
	c->submit = createfile(c->dir, "submit", user, 0220, (void*)(uintptr)(Xctxbase*(i+1)+Xctxsubmit));
	c->fence = createfile(c->dir, "fence", user, 0660, (void*)(uintptr)(Xctxbase*(i+1)+Xctxfence));
	qlock(&lk);
	ctxs[i] = c;
	qunlock(&lk);
	ctxreply(c, "ctx %d\n", c->id);
	return c;
}

/* last fid gone: its work done (or given up), its objects freed */
static void
freectx(Ctx *c)
{
	int i;

	if(c->nfence > c->donefence)
		fencewait(c, c->nfence);
	for(i = 0; i < Nbo; i++)
		if(bos[i] != nil && bos[i]->ctx == c)
			bofree(bos[i]);
	removefile(c->ctl);
	removefile(c->submit);
	removefile(c->fence);
	removefile(c->dir);
	closefile(c->ctl);
	closefile(c->submit);
	closefile(c->fence);
	closefile(c->dir);
	qlock(&lk);
	ctxs[c->id-1] = nil;
	qunlock(&lk);
	free(c->reply);
	free(c);
}

static char*
planestr(void)
{
	u32int cntr, src;
	int p, bpp;

	for(p = 0; p < 3; p++){
		cntr = rd(Dspcntr + p*0x1000);
		if((cntr & 1<<31) == 0)
			continue;
		switch(cntr>>26 & 0xF){
		case 5:		bpp = 16; break;
		case 6: case 7:	bpp = 32; break;
		default:	bpp = 0; break;
		}
		src = rd(Pipesrc + p*0x1000);
		return smprint("plane %c %#ux %ud %d %d %d%s\n", 'A'+p, rd(Dspsurf + p*0x1000) & ~0xFFF,
			rd(Dspstride + p*0x1000), bpp, (src>>16 & 0xFFF)+1, (src & 0xFFF)+1,
			cntr & 1<<10 ? " tiled" : "");
	}
	return estrdup9p("plane none\n");
}

static char*
statusstr(void)
{
	char buf[1024], *s, *t, *id;
	Engine *e;
	Hole *h;
	ulong nfree;
	int i, n;

	id = ctlline("id", buf, sizeof buf);
	s = smprint("device intel %s, gen 8\nirq %s %lud\nobjects %s\n", id ? id : "?",
		irqfd >= 0 ? "on" : "poll", nirq, uncached ? "uncached" : "cached (llc)");
	qlock(&lk);
	for(i = 0; i < nelem(eng); i++){
		e = &eng[i];
		t = smprint("%sengine %s %s seqno %ud/%ud head %#ux tail %#lux acthd %#ux eir %#ux ipehr %#ux\n",
			s, e->name, e->ok ? "ok" : "off", engdone(e), e->seqno,
			rd(e->base+Rhead) & Headmask, e->tail, rd(e->base+Racthd), rd(e->base+Reir),
			rd(e->base+Ripehr));
		free(s);
		s = t;
	}
	nfree = 0;
	for(h = holes; h != nil; h = h->next)
		nfree += h->len;
	qunlock(&lk);
	n = 0;
	for(i = 0; i < Nbo; i++)
		if(bos[i] != nil)
			n++;
	t = smprint("%sgtt %lld entries, %lud MB free, %d objects\n", s, nent, nfree>>20, n);
	free(s);
	s = t;
	for(i = 0; i < Nctx; i++)
		if(ctxs[i] != nil){
			t = smprint("%sctx %d fence %llud/%llud%s\n", s, ctxs[i]->id, ctxs[i]->donefence,
				ctxs[i]->nfence, ctxs[i]->failed ? " failed" : "");
			free(s);
			s = t;
		}
	return s;
}

static void
fsopen(Req *r)
{
	Fidaux *fa;
	Ctx *c;

	switch(ftype(r->fid->file)){
	case Xclone:
		qlock(&ctxlock);
		if((c = newctx()) == nil){
			qunlock(&ctxlock);
			responderror(r);
			return;
		}
		closefile(r->fid->file);
		r->fid->file = c->ctl;
		incref(c->ctl);
		r->ofcall.qid = c->ctl->qid;
		c->ref++;
		qunlock(&ctxlock);
		break;
	case Xctxctl:
	case Xctxsubmit:
	case Xctxfence:
		qlock(&ctxlock);
		if((c = fctx(r->fid->file)) == nil){
			qunlock(&ctxlock);
			respond(r, "context gone");
			return;
		}
		c->ref++;
		qunlock(&ctxlock);
		break;
	default:
		respond(r, nil);
		return;
	}
	fa = emalloc9p(sizeof *fa);
	memset(fa, 0, sizeof *fa);
	fa->ctx = c;
	r->fid->aux = fa;
	respond(r, nil);
}

static void
fsdestroyfid(Fid *f)
{
	Fidaux *fa;
	Ctx *c;

	if((fa = f->aux) == nil)
		return;
	f->aux = nil;
	c = fa->ctx;
	free(fa);
	qlock(&ctxlock);
	if(--c->ref == 0)
		freectx(c);
	qunlock(&ctxlock);
}

static void
fsread(Req *r)
{
	Fidaux *fa;
	Ctx *c;
	char *s;
	int n;

	fa = r->fid->aux;
	switch(ftype(r->fid->file)){
	case Xctl:
		s = planestr();
		break;
	case Xstatus:
		qlock(&ctxlock);
		s = statusstr();
		qunlock(&ctxlock);
		break;
	case Xctxctl:
		c = fa->ctx;
		qlock(&ctxlock);
		s = estrdup9p(c->reply != nil ? c->reply : "");
		qunlock(&ctxlock);
		break;
	case Xctxfence:
		c = fa->ctx;
		r->ifcall.offset = 0;
		if(fa->fence == 0){
			qlock(&lk);
			reap();
			s = smprint("%llud %llud%s\n", c->donefence, c->nfence, c->failed ? " failed" : "");
			qunlock(&lk);
			break;
		}
		if(fa->fence > c->nfence){
			respond(r, "no such fence");
			return;
		}
		if(!fa->query){
			srvrelease(r->srv);
			fencewait(c, fa->fence);
			srvacquire(r->srv);
		}
		qlock(&lk);
		n = fencedone(c, fa->fence);
		qunlock(&lk);
		s = smprint("%llud %s%s\n", fa->fence, n ? "done" : "busy", c->failed ? " failed" : "");
		break;
	default:
		respond(r, "bug");
		return;
	}
	readstr(r, s);
	free(s);
	respond(r, nil);
}

static char*
ctxctl(Ctx *c, char **f, int n)
{
	Bo *b;

	if(n == 2 && strcmp(f[0], "bo") == 0)
		return bonew(c, strtoull(f[1], nil, 0));
	if(n == 2 && strcmp(f[0], "free") == 0){
		if((b = findbo(c, atoi(f[1]))) == nil)
			return "unknown object";
		if(c->nfence > c->donefence)
			fencewait(c, c->nfence);	/* the GPU may still use it */
		bofree(b);
		return nil;
	}
	return "unknown ctl message";
}

static void
fswrite(Req *r)
{
	char buf[256], *f[8], *e;
	Fidaux *fa;
	int n;

	n = r->ifcall.count;
	if(n >= sizeof buf)
		n = sizeof buf-1;
	memmove(buf, r->ifcall.data, n);
	buf[n] = 0;
	n = tokenize(buf, f, nelem(f));
	fa = r->fid->aux;
	switch(ftype(r->fid->file)){
	case Xctxctl:
		if(n < 1){
			respond(r, "empty ctl message");
			return;
		}
		qlock(&ctxlock);
		e = ctxctl(fa->ctx, f, n);
		qunlock(&ctxlock);
		break;
	case Xctxsubmit:
		if(n != 2 && n != 3){
			respond(r, "usage: bo offset [rcs|bcs]");
			return;
		}
		qlock(&ctxlock);
		e = submit(fa->ctx, atoi(f[0]), strtoul(f[1], nil, 0), n == 3 ? f[2] : "rcs");
		qunlock(&ctxlock);
		break;
	case Xctxfence:
		if(n == 1){
			fa->query = 0;
			fa->fence = strtoull(f[0], nil, 0);
		}else if(n == 2 && (strcmp(f[0], "wait") == 0 || strcmp(f[0], "query") == 0)){
			fa->query = f[0][0] == 'q';
			fa->fence = strtoull(f[1], nil, 0);
		}else{
			respond(r, "usage: [wait|query] fence");
			return;
		}
		e = nil;
		break;
	default:
		e = "permission denied";
	}
	if(e != nil){
		respond(r, e);
		return;
	}
	r->ofcall.count = r->ifcall.count;
	respond(r, nil);
}

static void
mktree(void)
{
	File *gpu, *n;
	char *user;

	user = getuser();
	fs.tree = alloctree(user, user, DMDIR|0555, nil);
	gpu = createfile(fs.tree->root, "gpu", user, DMDIR|0555, nil);
	n = createfile(gpu, "0", user, DMDIR|0555, nil);
	createfile(n, "ctl", user, 0444, (void*)Xctl);
	createfile(n, "status", user, 0444, (void*)Xstatus);
	createfile(n, "clone", user, 0666, (void*)Xclone);
	ctxdir = createfile(n, "ctx", user, DMDIR|0555, nil);
}

static void
devinit(void)
{
	char buf[1024], path[64], *s, *f[5];
	uvlong gttsize;
	ulong g;
	int s0, i;

	if((s = ctlline("id", buf, sizeof buf)) == nil || strncmp(s, "8086/", 5) != 0)
		sysfatal("%s is %s, not Intel's", gdir, s ? s : "?");
	if((s = ctlline("bar0", buf, sizeof buf)) == nil || tokenize(s, f, nelem(f)) < 3)
		sysfatal("%s: no bar0", gdir);
	mmiosize = strtoull(f[1], nil, 0);
	if((mmio = segattach(0, f[2], nil, mmiosize)) == (void*)-1)
		sysfatal("segattach %s: %r", f[2]);
	gtt = (uvlong*)((uchar*)mmio + mmiosize/2);
	g = cfg16(Pcigmch);
	s0 = g>>6 & 3;
	gttsize = s0 ? (1ULL<<s0)<<20 : 0;
	nent = gttsize/8;
	if(nent*4096 <= Gttlow + 64*1024*1024)
		sysfatal("GTT of %lld entries: too small", nent);
	holes = emalloc9p(sizeof *holes);
	holes->next = nil;
	holes->start = Gttlow;
	holes->len = nent*4096 - Gttlow;
	if(nent*4096 > 0xFFFFFFFFULL)
		holes->len = 0xFFFFF000UL - Gttlow;	/* addresses in one dword (GTT up to 4 GB on gen 8) */

	snprint(path, sizeof path, "%s/ctl", gdir);
	if((gctlfd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);
	if(fprint(gctlfd, "bme on") < 0)
		sysfatal("bme on: %r");
	if(forcewake() < 0)
		sysfatal("forcewake: no ack (%#ux)", rd(Forcewakeack));
	if(!uncached)
		ppatinit();

	/* the driver's own pages: rings and status pages of the engines */
	if((drvpa = fixedseg("gpu0drv", Drvva, nelem(eng)*(Ringpages+1)*4096, 0700)) == 0)
		sysfatal("#g/gpu0drv: %r");
	if((drv = segattach(0, "gpu0drv", nil, nelem(eng)*(Ringpages+1)*4096)) == (void*)-1)
		sysfatal("segattach gpu0drv: %r");
	for(i = 0; i < nelem(eng); i++)
		engineinit(&eng[i], i);

	if(!poll){
		snprint(path, sizeof path, "%s/irq", gdir);
		if((irqfd = open(path, OREAD)) < 0)
			fprint(2, "%s: no interrupts, polling: %r\n", argv0);
	}
	irqinit();
}

static void
usage(void)
{
	fprint(2, "usage: %s [-dpU] [-g #G/n] [-s srvname]\n", argv0);
	exits("usage");
}

void
main(int argc, char **argv)
{
	char *srvname;
	int i;

	srvname = "gpu.0";
	ARGBEGIN{
	case 'd':
		debug++;
		break;
	case 'g':
		gdir = EARGF(usage());
		break;
	case 'p':
		poll = 1;
		break;
	case 's':
		srvname = EARGF(usage());
		break;
	case 'U':
		uncached = 1;
		break;
	default:
		usage();
	}ARGEND
	if(argc != 0)
		usage();

	done.l = &lk;
	devinit();
	for(i = 0; i < nelem(eng); i++)
		fprint(2, "%s: %s %s\n", argv0, eng[i].name, eng[i].ok ? "ok" : "off");

	switch(rfork(RFPROC|RFMEM|RFNOWAIT)){
	case -1:
		sysfatal("rfork: %r");
	case 0:
		irqproc();
		exits(nil);
	}
	switch(rfork(RFPROC|RFMEM|RFNOWAIT)){
	case -1:
		sysfatal("rfork: %r");
	case 0:
		tickproc();
		exits(nil);
	}

	fs.open = fsopen;
	fs.read = fsread;
	fs.write = fswrite;
	fs.destroyfid = fsdestroyfid;
	mktree();
	postmountsrv(&fs, srvname, "/dev", MAFTER);
	exits(nil);
}
