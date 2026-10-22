/*
 * SPDX-License-Identifier: MIT
 *
 * Parts follow Linux's i915 driver (MIT); their copyright and
 * permission notices are in NOTICE.
 *
 * intelgpu: user-space driver for Intel GPUs of generation 8
 * (Broadwell) over #G (gpu(3)), serving gpu(4)'s files: contexts
 * with an address space of their own (PPGTT) and buffer objects in
 * it, batch buffers on the render (RCS) and blitter (BCS) engines
 * through execlists (a logical ring context per context and engine,
 * as Linux's i915 has it on gen 8), fences from the contexts'
 * sequence numbers and the user and context switch interrupts (MSI).
 * The display stays as the firmware or aux/vga set it up.
 *
 *	/dev/gpu/0/ctl			the visible plane: plane p gpuaddr stride bpp w h
 *	/dev/gpu/0/status		device, engines, memory, contexts
 *	/dev/gpu/0/clone		a new context, ctx/n
 *	/dev/gpu/0/ctx/n/ctl		bo size, free id, map plane
 *	/dev/gpu/0/ctx/n/submit		bo offset [rcs|bcs]
 *	/dev/gpu/0/ctx/n/fence		as gpu(4)
 *
 * A buffer object is a #g segment of pinned pages (sticky; their
 * addresses from #g/seg/pa), mapped into the context's address space; the client
 * attaches it and writes GPU addresses into its batches itself (no
 * relocations).  A context's address space is 4 GB in three levels
 * (gen 8's 32-bit PPGTT: four page directories, which the context
 * image hands the engine at every switch); with -G all contexts share
 * the global GTT instead, without isolation.
 *
 * An engine runs one context at a time: a context with work waits
 * in the engine's queue, goes into the execlist submit port (ELSP)
 * when the engine is idle, and leaves it when the context status
 * buffer says it is done (no lite restore).
 *
 * usage: aux/intelgpu [-dpGU] [-g #G/n] [-s srvname]
 *
 * Register offsets and command formats as in Intel's PRMs (Broadwell,
 * vol. 2 and 5) and Linux's i915 for gen 8.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "renderstate.h"

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
	Rpdp		= 0x270,	/* PDP n: lower dword at +n*8, upper at +n*8+4 */
	Relsp		= 0x230,	/* RING_ELSP */
	Rexecstatus	= 0x234,
	Rctxctl		= 0x244,	/* RING_CONTEXT_CONTROL */
	Rcsb		= 0x370,	/* RING_CONTEXT_STATUS_BUF: 6 entries of 2 dwords */
	Rcsbptr		= 0x3A0,	/* RING_CONTEXT_STATUS_PTR: write 2:0, read 10:8 */
	Rhwstam		= 0x98,
	Rbbheadu	= 0x168,
	Rbbheadl	= 0x140,
	Rbbstate	= 0x110,
	Rsbbheadu	= 0x11C,
	Rsbbheadl	= 0x114,
	Rsbbstate	= 0x118,
	Rbbperctx	= 0x1C0,
	Rindctx		= 0x1C4,
	Rindctxoff	= 0x1C8,
	Rctxtimestamp	= 0x3A8,
	Rresetctl	= 0xD0,		/* RING_RESET_CTL */
	Resetrequest	= 1<<0,
	Resetready	= 1<<1,
	Gdrst		= 0x941C,	/* GEN6_GDRST: engine resets */

	Stopring	= 1<<8,
	Ringvalid	= 1<<0,
	Runlist		= 1<<15,
	Replaymode	= 1<<11,	/* RING_MODE: GFX_REPLAY_MODE */
	Rpwrclkstate	= 0x20C8,	/* GEN8_R_PWR_CLK_STATE */

	/* context control in the image */
	Ctxinhibitsyn	= 1<<3,
	Ctxrestoreinhibit = 1<<0,
	Ctxrsenable	= 1<<1,
	Lriposted	= 1<<12,	/* MI_LRI_FORCE_POSTED */

	/* the context descriptor */
	Descvalid	= 1<<0,
	Desclegacy32	= 1<<3,		/* LEGACY_32B_CONTEXT << 3 */
	Descl3llc	= 1<<5,
	Descpriv	= 1<<8,

	/* context status buffer */
	Csbentries	= 6,
	Csbptrmask	= 7,
	Csbpreempted	= 1<<1,
	Csbelemswitch	= 1<<2,
	Csbactiveidle	= 1<<3,
	Csbcomplete	= 1<<4,

	Ctxswitchintr	= 1<<8,		/* GT_CONTEXT_SWITCH_INTERRUPT, in an engine's field */
	Headmask	= 0x1FFFFC,

	/* interrupts */
	Masterirq	= 0x44200,
	Mastercontrol	= 1<<31,
	Gtisr0		= 0x44300,	/* bank 0: RCS at 0, BCS at 16; IMR +4, IIR +8, IER +0xC */
	Userintr	= 1<<0,

	/* RCS registers (engine base 0x2000), MI_MODE bit */
	Asyncflipperfdisable = 1<<14,
	Rinstpm		= 0xC0,		/* INSTPM */
	Instpmforceordering = 1<<7,

	/* GT clock gating, Broadwell (i915's broadwell_init_clock_gating, GT part) */
	Gamecochk	= 0x4090,
	Ecochkarbpriosol = 1<<6,
	Ffthreadmode	= 0x20A0,
	Ffdsrefcntffme	= 1<<19,
	Ffvsrefcntffme	= 1<<15,
	Rcsleeppsmictl	= 0x2050,
	Rcsemaidlemsgdisable = 1<<12,
	Ucgctl6		= 0x9430,
	Sdeunitclockgatedisable = 1<<14,
	Misccpctl	= 0x9424,
	Dopclockgateenable = 1<<0,
	L3sqcreg1	= 0xB100,
	L3sqcreg1default = 0x784000,
	Gttcacheen	= 0x4024,
	Gttcacheenall	= 0xF0007FFF,

	/* display */
	Pipesrc		= 0x6001C,
	Dspcntr		= 0x70180,
	Dspstride	= 0x70188,
	Dspsurf		= 0x7019C,

	/* commands */
	Minoop		= 0,
	Miuserintr	= 0x02<<23,
	Mibbstart	= 0x31<<23 | 1,	/* gen 8: 3 dwords, GGTT */
	Mistoredwimm	= 0x20<<23 | 2,	/* gen 8: 4 dwords */
	Storeggtt	= 1<<22,
	Bbppgtt		= 1<<8,		/* the batch's address is in the PPGTT */
	Milri		= 0x22<<23,	/* MI_LOAD_REGISTER_IMM: | 2*n-1 */
	Miflushdw	= 0x26<<23 | 1,
	Flushstoredw	= 1<<14,
	Flushstoreidx	= 1<<21,
	Flushusegtt	= 1<<2,
	Flushtlbinval	= 1<<18,
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
	Pterw		= 1<<1,		/* PPGTT entries: writable */
	Ptecached	= 1<<7,		/* index 4: write-back, LLC (ppatinit) */
	Pteuncached	= 1<<3 | 1<<4,	/* index 3: uncached */
	/*
	 * PDEs: bit 7 is the 2 MB page bit there, not PAT (the blitter
	 * honours it): index 2, write-through in the LLC, as the GPU only
	 * reads them (i915's gen8_pde_encode uses no PAT bit either)
	 */
	Pdecached	= 1<<4,

	Hwsscratch	= 0x40,		/* dword of an engine's status page the flushes may write */

	Ringpages	= 4,		/* a context's ring on an engine */
	Rcsstatepages	= 20,		/* register state of a context image: GEN8_LR_CONTEXT_RENDER_SIZE */
	Xcsstatepages	= 2,		/* GEN8_LR_CONTEXT_OTHER_SIZE */
	Gttlow		= 0x40000000,	/* our part of the GTT: from 1 GB to its end */
	Bova		= 0x2000000000ULL,	/* the objects' addresses in the clients */
	Drvva		= 0x50000000,	/* the driver's pages: the engines' status pages */
	Ctxva		= 0x58000000,	/* the contexts' pages: status, images, rings */
	Ptva		= 0x60000000,	/* the page table pool */
	Ptpages		= 4096,		/* 16 MB: page directories and tables */
	Ppbase		= 0x100000,	/* a context's objects from 1 MB */
	Pplimit		= 0xFFF00000UL,	/* to just below 4 GB */

	Nctx		= 64,
	Nbo		= 1024,
	Fwin		= 256,		/* fences in flight per context */
	Timeout		= 5000,		/* ms before a fence is given up */
};

typedef struct Engine Engine;
typedef struct Ectx Ectx;
typedef struct Bo Bo;
typedef struct Ctx Ctx;
typedef struct Fidaux Fidaux;
typedef struct Hole Hole;

struct Engine {
	char	*name;
	ulong	base;
	int	irqshift;
	u32int	resetbit;	/* in GEN6_GDRST */
	int	statepages;	/* of a context image */
	int	idx;
	u32int	*hws;		/* the engine's status page */
	ulong	hwsgtt;
	int	ok;
	Ectx	*running;	/* in the ELSP */
	Ectx	*queue;		/* with work, waiting for the engine */
	int	csbread;	/* the last status buffer entry read */
	int	nreset;
	u32int	nswitch;	/* contexts completed */
	vlong	since;		/* running since: hang watch */
};

/* a context on an engine: its logical ring context and ring */
struct Ectx {
	Ctx	*ctx;
	Engine	*e;
	u32int	*image;		/* per-process status page, then the register state */
	ulong	lrca;		/* its GTT address */
	u32int	*ring;
	ulong	ringgtt;
	ulong	tail;		/* bytes */
	ulong	subtail;	/* given to the engine */
	u32int	*seqno;		/* done: in the context's status page */
	ulong	seqgtt;
	u32int	nseq;		/* emitted */
	ulong	endpos[Fwin];	/* the ring's tail after sequence number k%Fwin */
	ulong	serial;		/* of the mapping its TLB has */
	int	fresh;		/* RCS: image new, workarounds and render state before its first batch */
	int	queued;
	Ectx	*next;
};

struct Bo {
	int	id;
	Ctx	*ctx;
	char	seg[32];
	uvlong	va, size;
	uvlong	*pa;		/* its pages' physical addresses */
	ulong	gtt;		/* the GPU address: in the context's PPGTT, or the GTT (-G) */
	int	plane;		/* the visible plane mapped (map plane), no segment */
};

struct Ctx {
	int	id;
	int	ref;
	char	*reply;
	uvlong	nfence;		/* submitted */
	uvlong	donefence;	/* complete, with all before it */
	Ectx	*fe[Fwin];	/* fence k: on context engine fe[k%Fwin] */
	u32int	fseq[Fwin];	/* with sequence number fseq[k%Fwin] */
	vlong	ftime[Fwin];	/* submitted then */
	int	failed;

	/* its pages: a status page, per engine an image and a ring */
	u32int	*mem;
	uvlong	pa;
	ulong	gtt, size;
	Ectx	ec[2];
	File	*dir, *ctl, *submit, *fence;

	/* the address space: page directories, tables (pool pages, -1: none) */
	ulong	serial;
	int	pd[4];
	short	pt[4][512];
	Hole	*holes;
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
static int debug, poll, uncached, global;
static u32int *ptpool;		/* Ptpages pages */
static uvlong ptpa;
static uchar ptused[Ptpages];
static ulong serial;
static int gctlfd = -1, irqfd = -1;
static u32int *mmio;
static uvlong mmiosize;
static uvlong *gtt;
static uvlong nent;
static uvlong drvpa;
static u32int *drv;
static ulong drvgtt;
static ulong rsgtt;		/* the null render state (renderstate.h) */

/*
 * the render engine's workarounds, written into each new context
 * (i915's gen8_init_workarounds and bdw_init_workarounds, GT2): masked
 * registers, the bits to set in the upper half, their values below
 */
static struct {
	ulong	reg;
	u32int	val;
} rcswa[] = {
	0x20C0, 0x00800080,	/* INSTPM: force ordering */
	0x209C, 0x40004000,	/* MI_MODE: async flip perf disable */
	0xE4F0, 0x01000100,	/* GEN8_ROW_CHICKEN: partial instruction shootdown disable */
	0x7300, 0x08100810,	/* HDC_CHICKEN0: don't fetch when masked, force non-coherent */
	0x7000, 0x00040000,	/* CACHE_MODE_0: HiZ RAW stall optimisation on */
	0x7004, 0x00400040,	/* CACHE_MODE_1: 4x4 STC optimisation disable */
	0x7008, 0x02800200,	/* GT_MODE: WIZ hashing 16x4 (field bits 9, 7) */
	0xE4F0, 0x00200020,	/* GEN8_ROW_CHICKEN: stall DOP gating disable */
	0xE4F4, 0x00010001,	/* GEN7_ROW_CHICKEN2: DOP clock gating disable */
	0xE184, 0x00020002,	/* HALF_SLICE_CHICKEN3: sampler power bypass disable */
	0x7300, 0x00200020,	/* HDC_CHICKEN0: context save/restore non-coherent */
};
static u32int *ctxpool;		/* Nctx slots of ctxslot() bytes */
static uvlong ctxpoolpa;
static ulong ctxpoolgtt;
static Hole *holes;
static Engine eng[] = {
	{ "rcs", 0x2000, 0, 1<<1, Rcsstatepages, 0 },	/* GEN6_GRDOM_RENDER */
	{ "bcs", 0x22000, 16, 1<<3, Xcsstatepages, 1 },	/* GEN6_GRDOM_BLT */
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

/*
 * a segment of pinned pages, not contiguous: their physical
 * addresses from #g/name/pa, one 64-bit word per page
 */
static uvlong*
stickyseg(char *name, uvlong va, ulong size, int perm)
{
	char path[64];
	uchar *b;
	uvlong *pa;
	ulong i, n;
	int fd, j;

	snprint(path, sizeof path, "#g/%s", name);
	if((fd = create(path, OREAD, DMDIR|perm)) >= 0)
		close(fd);
	snprint(path, sizeof path, "#g/%s/ctl", name);
	if((fd = open(path, OWRITE)) < 0)
		return nil;
	if(fprint(fd, "va %#llux %#lux sticky", va, size) < 0){
		close(fd);
		return nil;
	}
	close(fd);
	n = size/4096;
	b = emalloc9p(n*8);
	pa = emalloc9p(n*sizeof pa[0]);
	snprint(path, sizeof path, "#g/%s/pa", name);
	if((fd = open(path, OREAD)) < 0 || readn(fd, b, n*8) != n*8){
		if(fd >= 0)
			close(fd);
		free(b);
		free(pa);
		return nil;
	}
	close(fd);
	for(i = 0; i < n; i++){
		pa[i] = 0;
		for(j = 7; j >= 0; j--)
			pa[i] = pa[i]<<8 | b[i*8+j];
	}
	free(b);
	return pa;
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
holealloc(Hole **list, ulong len)
{
	Hole *h, **l;
	ulong a;

	len = (len + 4095) & ~4095;
	for(l = list; (h = *l) != nil; l = &h->next)
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
holefree(Hole **list, ulong a, ulong len)
{
	Hole *h, *p, **l;

	len = (len + 4095) & ~4095;
	for(l = list; (p = *l) != nil && p->start < a; l = &p->next)
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
	for(p = *list; p != nil && p->next != h; p = p->next)
		;
	if(p != nil && p->start + p->len == h->start){
		p->len += h->len;
		p->next = h->next;
		free(h);
	}
}

static ulong
gttalloc(ulong len)
{
	return holealloc(&holes, len);
}

static void
gttfree(ulong a, ulong len)
{
	holefree(&holes, a, len);
}

/*
 * map len bytes to gtt address a: the pages pas[], or
 * contiguous memory at pa if pas is nil
 */
static void
gttmap(ulong a, uvlong pa, uvlong *pas, ulong len)
{
	uvlong i, n, bits;

	bits = Ptepresent | (uncached ? Pteuncached : Ptecached);
	n = (len + 4095) / 4096;
	for(i = 0; i < n; i++)
		gtt[a/4096 + i] = (pas != nil ? pas[i] : pa + i*4096) | bits;
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
 * contexts' address spaces: page directories and tables are pages of
 * the pool, written cached (the GPU walks them through the LLC, as
 * i915 has it where there is an LLC), flushed with -U
 */
static int
ptalloc(void)
{
	int i;

	for(i = 0; i < Ptpages; i++)
		if(!ptused[i]){
			ptused[i] = 1;
			memset(ptpool + i*1024, 0, 4096);
			flushcache(ptpool + i*1024, 4096);
			return i;
		}
	return -1;
}

static void
ptfree(int i)
{
	if(i >= 0)
		ptused[i] = 0;
}

static uvlong
ptphys(int i)
{
	return ptpa + (uvlong)i*4096;
}

static uvlong*
ptmem(int i)
{
	return (uvlong*)(ptpool + i*1024);
}

static int
ppinit(Ctx *c)
{
	int i, j;

	for(i = 0; i < 4; i++){
		for(j = 0; j < 512; j++)
			c->pt[i][j] = -1;
		if((c->pd[i] = ptalloc()) < 0){
			while(--i >= 0)
				ptfree(c->pd[i]);
			return -1;
		}
	}
	c->holes = emalloc9p(sizeof(Hole));
	c->holes->next = nil;
	c->holes->start = Ppbase;
	c->holes->len = Pplimit - Ppbase;
	return 0;
}

static void
ppfree(Ctx *c)
{
	Hole *h;
	int i, j;

	for(i = 0; i < 4; i++){
		for(j = 0; j < 512; j++)
			ptfree(c->pt[i][j]);
		ptfree(c->pd[i]);
	}
	while((h = c->holes) != nil){
		c->holes = h->next;
		free(h);
	}
}

/*
 * map len bytes to the context's address a, with the PPAT bits:
 * the pages pas[], or contiguous memory at pa if pas is nil
 */
static int
ppmap(Ctx *c, ulong a, uvlong pa, uvlong *pas, ulong len, uvlong bits)
{
	uvlong *pd, *pt;
	ulong v, end;
	int i, j, k;

	end = a + ((len + 4095) & ~4095);
	for(v = a; v < end; v += 4096, pa += 4096){
		if(pas != nil)
			pa = pas[(v - a)/4096];
		i = v>>30;
		j = v>>21 & 511;
		k = v>>12 & 511;
		if(c->pt[i][j] < 0){
			if((c->pt[i][j] = ptalloc()) < 0)
				return -1;
			pd = ptmem(c->pd[i]);
			pd[j] = ptphys(c->pt[i][j]) | Ptepresent | Pterw | (uncached ? Pteuncached : Pdecached);
			flushcache(&pd[j], 8);
		}
		pt = ptmem(c->pt[i][j]);
		pt[k] = pa | Ptepresent | Pterw | bits;
		flushcache(&pt[k], 8);
	}
	c->serial = ++serial;	/* the engines invalidate their TLBs before its next batch */
	return 0;
}

static void
ppunmap(Ctx *c, ulong a, ulong len)
{
	uvlong *pt;
	ulong v, end;
	int i, j;

	end = a + ((len + 4095) & ~4095);
	for(v = a; v < end; v += 4096){
		i = v>>30;
		j = v>>21 & 511;
		if(c->pt[i][j] < 0)
			continue;
		pt = ptmem(c->pt[i][j]);
		pt[v>>12 & 511] = 0;
		flushcache(&pt[v>>12 & 511], 8);
	}
	c->serial = ++serial;
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
static void irqinit(void);
static void kick(Engine*);

/*
 * engines in execlist mode (i915's gen8_init_common_ring): the
 * engine's status page, interrupts, the run list on, the context
 * status buffer's read pointer where the hardware has it
 */
static void
enginestart(Engine *e)
{
	u32int p;

	e->ok = 0;
	e->running = nil;
	wr(e->base+Rhwstam, ~0);
	wr(e->base+Rimr, ~((Userintr|Ctxswitchintr)<<e->irqshift));
	wr(e->base+Rhws, e->hwsgtt);
	rd(e->base+Rhws);
	wr(e->base+Rmode, masked(Replaymode, 0) | masked(Runlist, 1));
	rd(e->base+Rmode);
	if(e->idx == 0){
		/* i915's gen8_init_render_ring */
		wr(e->base+Rmimode, masked(Asyncflipperfdisable, 1));
		wr(e->base+Rinstpm, masked(Instpmforceordering, 1));
	}
	/* after power-up or a reset the write pointer is all ones: read from entry 0 */
	p = rd(e->base+Rcsbptr) & Csbptrmask;
	if(p == Csbptrmask)
		p = Csbentries-1;
	e->csbread = p;
	e->since = nsec();
	if((rd(e->base+Rmode) & Runlist) == 0){
		fprint(2, "%s: %s: no execlists: mode %#ux\n", argv0, e->name, rd(e->base+Rmode));
		return;
	}
	e->ok = 1;
}

static void
engineinit(Engine *e)
{
	e->hws = drv + e->idx*1024;
	e->hwsgtt = drvgtt + e->idx*4096;
	memset(e->hws, 0, 4096);
	flushcache(e->hws, 4096);
	enginestart(e);
}

/*
 * a context's image for an engine, as i915's populate_lr_context: the
 * per-process status page, then the register state the engine loads
 * (MI_LOAD_REGISTER_IMM lists); the first load restores nothing else
 * (restore inhibit), the engine writes the rest at the switches
 */
static void
ecimage(Ectx *ec)
{
	Engine *e;
	u32int *r;
	ulong b;
	int n;

	e = ec->e;
	memset(ec->image, 0, (1+e->statepages)*4096);
	r = ec->image + 1024;
	b = e->base;
	r[0x01] = Milri | (2*(e->idx == 0 ? 14 : 11) - 1) | Lriposted;
	r[0x02] = b + Rctxctl;
	r[0x03] = masked(Ctxinhibitsyn | Ctxrestoreinhibit | Ctxrsenable, 1);
	r[0x04] = b + Rhead;	r[0x05] = 0;
	r[0x06] = b + Rtail;	r[0x07] = 0;
	r[0x08] = b + Rstart;	r[0x09] = ec->ringgtt;
	r[0x0a] = b + Rctl;	r[0x0b] = (Ringpages-1)<<12 | Ringvalid;
	r[0x0c] = b + Rbbheadu;	r[0x0d] = 0;
	r[0x0e] = b + Rbbheadl;	r[0x0f] = 0;
	r[0x10] = b + Rbbstate;	r[0x11] = 1<<5;
	r[0x12] = b + Rsbbheadu;	r[0x13] = 0;
	r[0x14] = b + Rsbbheadl;	r[0x15] = 0;
	r[0x16] = b + Rsbbstate;	r[0x17] = 0;
	if(e->idx == 0){
		r[0x18] = b + Rbbperctx;	r[0x19] = 0;
		r[0x1a] = b + Rindctx;	r[0x1b] = 0;
		r[0x1c] = b + Rindctxoff;	r[0x1d] = 0;
	}
	r[0x21] = Milri | (2*9 - 1) | Lriposted;
	r[0x22] = b + Rctxtimestamp;	r[0x23] = 0;
	for(n = 3; n >= 0; n--){
		r[0x24 + (3-n)*4] = b + Rpdp + n*8 + 4;
		r[0x26 + (3-n)*4] = b + Rpdp + n*8;
	}
	if(e->idx == 0){
		r[0x41] = Milri | (2*1 - 1);
		r[0x42] = Rpwrclkstate;	r[0x43] = 0;	/* gen 8: no explicit RPCS request (make_rpcs) */
	}
	memset(ec->ring, 0, Ringpages*4096);
	flushcache(ec->image, (1+e->statepages)*4096);
	flushcache(ec->ring, Ringpages*4096);
	ec->tail = ec->subtail = 0;
	memset(ec->endpos, 0, sizeof ec->endpos);
}

static void
ecinit(Ctx *c, Ectx *ec, Engine *e, ulong off)
{
	ec->ctx = c;
	ec->e = e;
	ec->image = c->mem + off/4;
	ec->lrca = c->gtt + off;
	ec->ring = c->mem + (off + (1+e->statepages)*4096)/4;
	ec->ringgtt = c->gtt + off + (1+e->statepages)*4096;
	ec->seqno = c->mem + e->idx*16;
	ec->seqgtt = c->gtt + e->idx*64;
	*ec->seqno = 0;
	ec->nseq = 0;
	ec->serial = 0;
	ec->fresh = 1;
	ec->queued = 0;
	ec->next = nil;
	ecimage(ec);
}

/* the ring's tail and the address space into the image, before the engine loads it */
static void
ecupdate(Ectx *ec)
{
	u32int *r;
	uvlong pa;
	int n;

	r = ec->image + 1024;
	r[0x07] = ec->tail;
	r[0x09] = ec->ringgtt;
	for(n = 3; n >= 0; n--){
		pa = global ? 0 : ptphys(ec->ctx->pd[n]);
		r[0x25 + (3-n)*4] = pa>>32;
		r[0x27 + (3-n)*4] = pa;
	}
	flushcache(r, 0x50*4);
}

static uvlong
descriptor(Ectx *ec)
{
	return (uvlong)(ec->lrca>>12)<<32 | ec->lrca | Descvalid | Desclegacy32 | Descl3llc | Descpriv;
}

/* the next context with work into the idle engine's ELSP; with lk */
static void
kick(Engine *e)
{
	Ectx *ec;
	uvlong d;

	if(!e->ok || e->running != nil || (ec = e->queue) == nil)
		return;
	e->queue = ec->next;
	ec->next = nil;
	ec->queued = 0;
	ecupdate(ec);
	d = descriptor(ec);
	/* both elements, the second (none) first; the first's lower dword starts it */
	wr(e->base+Relsp, 0);
	wr(e->base+Relsp, 0);
	wr(e->base+Relsp, d>>32);
	wr(e->base+Relsp, d);
	rd(e->base+Rexecstatus);
	e->running = ec;
	ec->subtail = ec->tail;
	e->since = nsec();
}

static void
enqueue(Ectx *ec)
{
	Ectx **l;

	if(ec->queued || ec->e->running == ec)
		return;
	for(l = &ec->e->queue; *l != nil; l = &(*l)->next)
		;
	*l = ec;
	ec->next = nil;
	ec->queued = 1;
}

/*
 * the context status buffer (i915's intel_lrc_irq_handler): a context
 * that went idle or was switched out leaves the engine, which takes
 * the next; with work added meanwhile it queues again.  With lk.
 */
static void
csb(Engine *e)
{
	u32int st, id, w;
	Ectx *ec;
	int r;

	w = rd(e->base+Rcsbptr) & Csbptrmask;
	if(w == Csbptrmask || w >= Csbentries)
		return;
	r = e->csbread;
	while(r != w){
		r = (r + 1) % Csbentries;
		st = rd(e->base + Rcsb + r*8);
		id = rd(e->base + Rcsb + r*8 + 4);
		DBG(2, "%s csb %d: %#ux %#ux\n", e->name, r, st, id);
		if(st & Csbpreempted)
			fprint(2, "%s: %s: context %#ux preempted\n", argv0, e->name, id);
		if((st & (Csbcomplete|Csbactiveidle|Csbelemswitch)) == 0)
			continue;
		ec = e->running;
		if(ec == nil || (ec->lrca>>12) != id)
			continue;
		e->running = nil;
		e->nswitch++;
		if(ec->tail != ec->subtail)
			enqueue(ec);
	}
	e->csbread = r;
	wr(e->base+Rcsbptr, (Csbptrmask<<8)<<16 | (r & Csbptrmask)<<8);
	kick(e);
}

static void
ecemit(Ectx *ec, u32int v)
{
	ec->ring[ec->tail/4] = v;
	ec->tail = (ec->tail + 4) % (Ringpages*4096);
}

/* the context's sequence number done on the engine */
static u32int
ecdone(Ectx *ec)
{
	flushcache(ec->seqno, 4);
	return *ec->seqno;
}

/*
 * room for n dwords in the ring: what is done is free, from where the
 * oldest unfinished submission starts; waits (lk released) for the
 * engine to get on
 */
static int
ecspace(Ectx *ec, int n)
{
	ulong start, size;
	u32int d;
	vlong t0;

	size = Ringpages*4096;
	t0 = nsec();
	for(;;){
		d = ecdone(ec);
		if(d == ec->nseq)
			return 0;
		start = ec->endpos[d % Fwin];
		if((start + size - ec->tail) % size > (ulong)n*4 + 8)
			return 0;
		if(nsec() - t0 > (vlong)Timeout*1000000)
			return -1;
		rsleep(&done);
	}
}

/*
 * the batch at gpu address a into the context's ring on its engine:
 * the TLBs invalidated if its mapping changed, the batch, the caches
 * flushed, the sequence number into the context's status page, the
 * interrupt; then queued for the engine.  With lk.
 */
static char*
ecsubmit(Ectx *ec, ulong a, u32int *seq)
{
	Engine *e;
	ulong t0;
	uvlong pa;
	int n;

	e = ec->e;
	if(!e->ok)
		return "engine not running";
	if(ecspace(ec, 72 + 2*nelem(rcswa) + 2 + 2*6 + 3) < 0)
		return "ring full (engine hung?)";
	t0 = ec->tail;
	if(ec->fresh && e->idx == 0){
		/*
		 * a new render context: the workarounds, flushed before and
		 * after, then the null render state, a privileged batch in
		 * the GTT (i915's gen8_init_rcs_context)
		 */
		ecemit(ec, Pipecontrol);
		ecemit(ec, Pccsstall | Pcrtflush | Pcdcflush | Pcdepthflush | Pcqwwrite | Pcglobalgtt);
		ecemit(ec, e->hwsgtt + Hwsscratch*4);
		ecemit(ec, 0);
		ecemit(ec, 0);
		ecemit(ec, 0);
		ecemit(ec, Milri | (2*nelem(rcswa) - 1));
		for(n = 0; n < nelem(rcswa); n++){
			ecemit(ec, rcswa[n].reg);
			ecemit(ec, rcswa[n].val);
		}
		ecemit(ec, Minoop);
		ecemit(ec, Pipecontrol);
		ecemit(ec, Pccsstall | Pcrtflush | Pcdcflush | Pcdepthflush | Pcqwwrite | Pcglobalgtt);
		ecemit(ec, e->hwsgtt + Hwsscratch*4);
		ecemit(ec, 0);
		ecemit(ec, 0);
		ecemit(ec, 0);
		ecemit(ec, Mibbstart);
		ecemit(ec, rsgtt);
		ecemit(ec, 0);
	}
	ec->fresh = 0;
	if(!global && ec->serial != ec->ctx->serial){
		if(e->idx == 0){
			ecemit(ec, Pipecontrol);
			ecemit(ec, Pccsstall | Pctlbinval | Pcqwwrite | Pcglobalgtt);
			ecemit(ec, e->hwsgtt + Hwsscratch*4);
			ecemit(ec, 0);
			ecemit(ec, 0);
			ecemit(ec, 0);
		}else{
			ecemit(ec, (Miflushdw+1) | Flushtlbinval | Flushstoredw | Flushstoreidx);
			ecemit(ec, Hwsscratch*4 | Flushusegtt);
			ecemit(ec, 0);
			ecemit(ec, 0);
		}
		ec->serial = ec->ctx->serial;
	}
	/*
	 * the PDPs from the ring before every batch, as i915's
	 * gen8_emit_bb_start does before the first (pd_dirty_rings): the
	 * blitter did not take them from the image (on the laptop it
	 * walked wrong page tables), and another context may have run on
	 * the engine between two batches of this one
	 */
	if(!global){
		ecemit(ec, Milri | (2*8 - 1));
		for(n = 3; n >= 0; n--){
			pa = ptphys(ec->ctx->pd[n]);
			ecemit(ec, e->base + Rpdp + n*8 + 4);
			ecemit(ec, pa>>32);
			ecemit(ec, e->base + Rpdp + n*8);
			ecemit(ec, pa);
		}
		ecemit(ec, Minoop);
	}
	ecemit(ec, Mibbstart | (global ? 0 : Bbppgtt));
	ecemit(ec, a);
	ecemit(ec, 0);
	if(e->idx == 0){
		ecemit(ec, Pipecontrol);
		ecemit(ec, Pccsstall | Pcrtflush | Pcdcflush | Pcdepthflush | Pcqwwrite | Pcglobalgtt);
		ecemit(ec, e->hwsgtt + Hwsscratch*4);
		ecemit(ec, 0);
		ecemit(ec, 0);
		ecemit(ec, 0);
	}else{
		ecemit(ec, (Miflushdw+1) | Flushstoredw | Flushstoreidx);
		ecemit(ec, Hwsscratch*4 | Flushusegtt);
		ecemit(ec, 0);
		ecemit(ec, 0);
	}
	*seq = ++ec->nseq;
	ecemit(ec, Mistoredwimm | Storeggtt);
	ecemit(ec, ec->seqgtt);
	ecemit(ec, 0);
	ecemit(ec, *seq);
	ecemit(ec, Miuserintr);
	while(ec->tail & 7)
		ecemit(ec, Minoop);
	ec->endpos[*seq % Fwin] = ec->tail;
	if(ec->tail > t0)
		flushcache(ec->ring + t0/4, ec->tail - t0);
	else{
		flushcache(ec->ring + t0/4, Ringpages*4096 - t0);
		flushcache(ec->ring, ec->tail);
	}
	enqueue(ec);
	kick(e);
	return nil;
}

/*
 * an engine reset (i915's gen8 reset_prepare and gen6_reset_engines):
 * the engine asked to stop, reset through GDRST, started again; the
 * contexts it ran or had queued fail, their work counts as done and
 * their images start afresh.  With lk.
 */
static void
engreset(Engine *e)
{
	Ectx *ec, *bad[Nctx+1];
	Ctx *c;
	int i, n;

	wr(e->base+Rresetctl, masked(Resetrequest, 1));
	for(i = 0; i < 20 && (rd(e->base+Rresetctl) & Resetready) == 0; i++)
		sleep(1);
	wr(Gdrst, e->resetbit);
	for(i = 0; i < 500 && (rd(Gdrst) & e->resetbit) != 0; i++)
		sleep(1);
	wr(e->base+Rresetctl, masked(Resetrequest, 0));

	n = 0;
	if(e->running != nil)
		bad[n++] = e->running;
	for(ec = e->queue; ec != nil && n < nelem(bad); ec = ec->next)
		bad[n++] = ec;
	e->queue = nil;
	for(i = 0; i < n; i++){
		ec = bad[i];
		c = ec->ctx;
		c->failed = 1;
		*ec->seqno = ec->nseq;
		flushcache(ec->seqno, 4);
		ec->queued = 0;
		ec->next = nil;
		ecimage(ec);
		ec->serial = 0;
		ec->fresh = 1;
	}
	enginestart(e);
	e->nreset++;
	fprint(2, "%s: %s reset, %d contexts failed, %s\n", argv0, e->name, n,
		e->ok ? "running again" : "not running");
}

static void
irqinit(void)
{
	u32int mask;
	int i;

	mask = 0;
	for(i = 0; i < nelem(eng); i++)
		if(eng[i].ok)
			mask |= (Userintr|Ctxswitchintr) << eng[i].irqshift;
	wr(Gtisr0+8, ~0);		/* IIR */
	wr(Gtisr0+0xC, mask);		/* IER */
	wr(Gtisr0+4, ~mask);		/* IMR */
	wr(Masterirq, Mastercontrol);
}

/*
 * fences
 */
static int
fencedone(Ctx *c, uvlong k)
{
	Ectx *ec;

	if(k <= c->donefence)
		return 1;
	if(k > c->nfence)
		return 0;
	ec = c->fe[k % Fwin];
	return ec == nil || (int)(ecdone(ec) - c->fseq[k % Fwin]) >= 0;
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
	int i;

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
		for(i = 0; i < nelem(eng); i++)
			csb(&eng[i]);
		reap();
		rwakeupall(&done);
		qunlock(&lk);
	}
}

/*
 * now and then: the status buffers (an interrupt may be lost), the
 * waiters' timeouts, an engine that runs one context too long hung
 */
static void
tickproc(void)
{
	Engine *e;
	int i;

	for(;;){
		sleep(100);
		qlock(&lk);
		for(i = 0; i < nelem(eng); i++){
			e = &eng[i];
			csb(e);
			if(e->running != nil && nsec() - e->since > (vlong)Timeout*1000000){
				fprint(2, "%s: %s hung in context %d: head %#ux acthd %#ux ipehr %#ux eir %#ux execlist %#ux"
					" pdp0 %#.8ux%.8ux (pd0 %#llux)\n",
					argv0, e->name, e->running->ctx->id, rd(e->base+Rhead) & Headmask,
					rd(e->base+Racthd), rd(e->base+Ripehr), rd(e->base+Reir), rd(e->base+Rexecstatus),
					rd(e->base+Rpdp+4), rd(e->base+Rpdp), global ? 0 : ptphys(e->running->ctx->pd[0]));
				engreset(e);
				irqinit();
				kick(e);
			}
		}
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
	if((b->pa = stickyseg(b->seg, b->va, size, 0770)) == nil){
		rmseg(b->seg);
		free(b);
		return "no memory";
	}
	qlock(&lk);
	if(global){
		b->gtt = gttalloc(size);
		if(b->gtt != 0)
			gttmap(b->gtt, 0, b->pa, size);
	}else{
		b->gtt = holealloc(&c->holes, size);
		if(b->gtt != 0 && ppmap(c, b->gtt, 0, b->pa, size, uncached ? Pteuncached : Ptecached) < 0){
			ppunmap(c, b->gtt, size);
			holefree(&c->holes, b->gtt, size);
			b->gtt = 0;
		}
	}
	qunlock(&lk);
	if(b->gtt == 0){
		rmseg(b->seg);
		free(b->pa);
		free(b);
		return "no GPU address space (or page tables) left";
	}
	bos[i] = b;
	return ctxreply(c, "bo %d %s %#llux %#llux %#lux\n", b->id, b->seg, b->va, b->size, b->gtt);
}

static void
bofree(Bo *b)
{
	qlock(&lk);
	if(global){
		if(!b->plane){
			gttunmap(b->gtt, b->size);
			gttfree(b->gtt, b->size);
		}
	}else{
		ppunmap(b->ctx, b->gtt, b->size);
		holefree(&b->ctx->holes, b->gtt, b->size);
	}
	qunlock(&lk);
	if(!b->plane)
		rmseg(b->seg);
	bos[b->id-1] = nil;
	free(b->pa);
	free(b);
}

/* the visible plane: GTT address, stride, bits a pixel, size; -1 if none */
static int
plane(ulong *surf, ulong *stride, int *bpp, int *w, int *h, int *tiled)
{
	u32int cntr, src;
	int p;

	for(p = 0; p < 3; p++){
		cntr = rd(Dspcntr + p*0x1000);
		if((cntr & 1<<31) == 0)
			continue;
		switch(cntr>>26 & 0xF){
		case 5:		*bpp = 16; break;
		case 6: case 7:	*bpp = 32; break;
		default:	*bpp = 0; break;
		}
		src = rd(Pipesrc + p*0x1000);
		*surf = rd(Dspsurf + p*0x1000) & ~0xFFF;
		*stride = rd(Dspstride + p*0x1000);
		*w = (src>>16 & 0xFFF)+1;
		*h = (src & 0xFFF)+1;
		*tiled = (cntr & 1<<10) != 0;
		return p;
	}
	return -1;
}

/*
 * map plane: the visible plane's memory into the context (uncached,
 * the display engine reads it from memory), page by page from its GTT
 * entries; with -G it is in the GTT already
 */
static char*
mapplane(Ctx *c)
{
	ulong surf, stride, size, off;
	int p, bpp, w, h, tiled, i;
	uvlong pa;
	Bo *b;

	if((p = plane(&surf, &stride, &bpp, &w, &h, &tiled)) < 0)
		return "no plane";
	size = (stride*h + 4095) & ~4095;
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
	b->plane = 1;
	qlock(&lk);
	if(global)
		b->gtt = surf;
	else if((b->gtt = holealloc(&c->holes, size)) != 0)
		for(off = 0; off < size; off += 4096){
			pa = gtt[(surf + off)/4096] & 0x7FFFFFF000ULL;
			if(ppmap(c, b->gtt + off, pa, nil, 4096, Pteuncached) < 0){
				ppunmap(c, b->gtt, off);
				holefree(&c->holes, b->gtt, size);
				b->gtt = 0;
				break;
			}
		}
	qunlock(&lk);
	if(b->gtt == 0){
		free(b);
		return "no GPU address space (or page tables) left";
	}
	bos[i] = b;
	return ctxreply(c, "plane %d %#lux %lud %d %d %d%s\n", b->id, b->gtt, stride, bpp, w, h,
		tiled ? " tiled" : "");
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
	if((err = ecsubmit(&c->ec[e->idx], b->gtt + off, &seq)) != nil){
		qunlock(&lk);
		return err;
	}
	c->nfence++;
	c->fe[c->nfence % Fwin] = &c->ec[e->idx];
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

/*
 * a context's pages: its slot of the pool (attached before the driver's
 * procs are forked: a segment attached later is not theirs), mapped
 * into the GTT with it: the status page (the sequence numbers), then
 * per engine a context image and a ring
 */
static ulong
ctxslot(void)
{
	ulong n;
	int k;

	n = 4096;
	for(k = 0; k < nelem(eng); k++)
		n += (1 + eng[k].statepages + Ringpages)*4096;
	return n;
}

static int
ctxmem(Ctx *c, int i)
{
	ulong off;
	int k;

	c->size = ctxslot();
	c->mem = ctxpool + (uvlong)i*c->size/4;
	c->pa = ctxpoolpa + (uvlong)i*c->size;
	c->gtt = ctxpoolgtt + i*c->size;
	memset(c->mem, 0, c->size);
	qlock(&lk);
	off = 4096;
	for(k = 0; k < nelem(eng); k++){
		ecinit(c, &c->ec[k], &eng[k], off);
		off += (1 + eng[k].statepages + Ringpages)*4096;
	}
	qunlock(&lk);
	return 0;
}

static void
ctxmemfree(Ctx *c)
{
	memset(c->mem, 0, c->size);
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
	if(ctxmem(c, i) < 0){
		free(c);
		return nil;
	}
	qlock(&lk);
	c->serial = ++serial;
	if(!global && ppinit(c) < 0){
		qunlock(&lk);
		ctxmemfree(c);
		free(c);
		werrstr("no page tables left");
		return nil;
	}
	qunlock(&lk);
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
	/* off the engines: not running, not queued */
	qlock(&lk);
	for(i = 0; i < nelem(eng); i++){
		while(eng[i].running == &c->ec[i] && eng[i].ok && nsec() - eng[i].since < (vlong)Timeout*1000000)
			rsleep(&done);
		if(eng[i].running == &c->ec[i]){
			engreset(&eng[i]);
			irqinit();
		}
		if(c->ec[i].queued){
			Ectx **l;

			for(l = &eng[i].queue; *l != nil; l = &(*l)->next)
				if(*l == &c->ec[i]){
					*l = c->ec[i].next;
					break;
				}
			c->ec[i].queued = 0;
		}
	}
	ctxs[c->id-1] = nil;
	if(!global)
		ppfree(c);
	qunlock(&lk);
	ctxmemfree(c);
	for(i = 0; i < nelem(eng); i++){
		qlock(&lk);
		kick(&eng[i]);
		qunlock(&lk);
	}
	free(c->reply);
	free(c);
}

static char*
planestr(void)
{
	ulong surf, stride;
	int p, bpp, w, h, tiled;

	if((p = plane(&surf, &stride, &bpp, &w, &h, &tiled)) < 0)
		return estrdup9p("plane none\n");
	return smprint("plane %c %#lux %lud %d %d %d%s\n", 'A'+p, surf, stride, bpp, w, h,
		tiled ? " tiled" : "");
}

static char*
statusstr(void)
{
	char buf[1024], *s, *t, *id;
	Engine *e;
	Ectx *q;
	Hole *h;
	ulong nfree;
	int i, n;

	id = ctlline("id", buf, sizeof buf);
	s = smprint("device intel %s, gen 8\nirq %s %lud\nobjects %s, %s\n", id ? id : "?",
		irqfd >= 0 ? "on" : "poll", nirq, uncached ? "uncached" : "cached (llc)",
		global ? "global gtt" : "ppgtt per context");
	qlock(&lk);
	for(i = 0; i < nelem(eng); i++){
		e = &eng[i];
		n = 0;
		for(q = e->queue; q != nil; q = q->next)
			n++;
		t = smprint("%sengine %s %s execlists: running ctx %d, %d queued, %ud switches, csb %d/%d,"
			" status %#ux mode %#ux acthd %#ux eir %#ux ipehr %#ux pdp0 %#.8ux%.8ux resets %d\n",
			s, e->name, e->ok ? "ok" : "off", e->running ? e->running->ctx->id : 0, n, e->nswitch,
			e->csbread, rd(e->base+Rcsbptr) & Csbptrmask, rd(e->base+Rexecstatus), rd(e->base+Rmode),
			rd(e->base+Racthd), rd(e->base+Reir), rd(e->base+Ripehr), rd(e->base+Rpdp+4),
			rd(e->base+Rpdp), e->nreset);
		free(s);
		s = t;
	}
	/* the render engine's workaround registers as they read now (the last context's) */
	n = 0;
	for(i = 0; i < nelem(rcswa); i++)
		if((rd(rcswa[i].reg) & rcswa[i].val>>16) == (rcswa[i].val & 0xFFFF))
			n++;
	t = smprint("%srcs workarounds %d/%d\n", s, n, nelem(rcswa));
	free(s);
	s = t;
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
			t = smprint("%sctx %d fence %llud/%llud seqno rcs %ud/%ud bcs %ud/%ud lrca %#lux pd0 %#llux%s\n",
				s, ctxs[i]->id, ctxs[i]->donefence, ctxs[i]->nfence,
				ecdone(&ctxs[i]->ec[0]), ctxs[i]->ec[0].nseq, ecdone(&ctxs[i]->ec[1]), ctxs[i]->ec[1].nseq,
				ctxs[i]->ec[0].lrca, global ? 0 : ptphys(ctxs[i]->pd[0]), ctxs[i]->failed ? " failed" : "");
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
	if(n == 2 && strcmp(f[0], "map") == 0 && strcmp(f[1], "plane") == 0)
		return mapplane(c);
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

/* the device's ctl: how the PDPs are loaded, engine resets */
static char*
devctl(char **f, int n)
{
	int i;

	if(n == 1 && strcmp(f[0], "reset") == 0){
		qlock(&lk);
		for(i = 0; i < nelem(eng); i++)
			engreset(&eng[i]);
		irqinit();
		for(i = 0; i < nelem(eng); i++)
			kick(&eng[i]);
		reap();
		rwakeupall(&done);
		qunlock(&lk);
		return nil;
	}
	return "usage: reset";
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
	case Xctl:
		e = devctl(f, n);
		break;
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
	createfile(n, "ctl", user, 0664, (void*)Xctl);
	createfile(n, "status", user, 0444, (void*)Xstatus);
	createfile(n, "clone", user, 0666, (void*)Xclone);
	ctxdir = createfile(n, "ctx", user, DMDIR|0555, nil);
}

/*
 * the null render state (renderstate.h) in the driver's last page,
 * relocated to its GTT address (i915's render_state_setup)
 */
static void
rsinit(void)
{
	u32int *d, v;
	int i, r;

	if(sizeof gen8_null_state_batch > 4096)
		sysfatal("render state larger than a page");
	d = drv + nelem(eng)*1024;
	rsgtt = drvgtt + nelem(eng)*4096;
	memset(d, 0, 4096);
	r = 0;
	for(i = 0; i < nelem(gen8_null_state_batch); i++){
		v = gen8_null_state_batch[i];
		if(i*4 == gen8_null_state_relocs[r]){
			if(i+1 >= nelem(gen8_null_state_batch) || gen8_null_state_batch[i+1] != 0)
				sysfatal("render state: bad relocation at %#x", i*4);
			v += rsgtt;
			r++;
		}
		d[i] = v;
	}
	if(gen8_null_state_relocs[r] != (u32int)-1)
		sysfatal("render state: only %d relocations done", r);
	flushcache(d, 4096);
}

/* the GT part of i915's broadwell_init_clock_gating */
static void
gtinit(void)
{
	u32int m;

	wr(Gamecochk, rd(Gamecochk) | Ecochkarbpriosol);
	wr(Ffthreadmode, rd(Ffthreadmode) & ~(Ffdsrefcntffme | Ffvsrefcntffme));
	wr(Rcsleeppsmictl, masked(Rcsemaidlemsgdisable, 1));
	wr(Ucgctl6, rd(Ucgctl6) | Sdeunitclockgatedisable);
	m = rd(Misccpctl);
	wr(Misccpctl, m & ~Dopclockgateenable);
	wr(L3sqcreg1, L3sqcreg1default);
	wr(Misccpctl, m);
	wr(Gttcacheen, Gttcacheenall);
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

	/* the driver's own pages: the engines' status pages, the null render state */
	if((drvpa = fixedseg("gpu0drv", Drvva, (nelem(eng)+1)*4096, 0700)) == 0)
		sysfatal("#g/gpu0drv: %r");
	if((drv = segattach(0, "gpu0drv", nil, (nelem(eng)+1)*4096)) == (void*)-1)
		sysfatal("segattach gpu0drv: %r");
	if((drvgtt = gttalloc((nelem(eng)+1)*4096)) == 0)
		sysfatal("no GTT for the status pages");
	gttmap(drvgtt, drvpa, nil, (nelem(eng)+1)*4096);
	rsinit();
	/* the contexts' pages, here: the procs forked later see only what is attached now */
	if((ctxpoolpa = fixedseg("gpu0ctx", Ctxva, Nctx*ctxslot(), 0700)) == 0)
		sysfatal("#g/gpu0ctx: %r");
	if((ctxpool = segattach(0, "gpu0ctx", nil, Nctx*ctxslot())) == (void*)-1)
		sysfatal("segattach gpu0ctx: %r");
	if((ctxpoolgtt = gttalloc(Nctx*ctxslot())) == 0)
		sysfatal("no GTT for the contexts");
	gttmap(ctxpoolgtt, ctxpoolpa, nil, Nctx*ctxslot());
	if(!global){
		if((ptpa = fixedseg("gpu0pt", Ptva, Ptpages*4096, 0700)) == 0)
			sysfatal("#g/gpu0pt: %r");
		if((ptpool = segattach(0, "gpu0pt", nil, Ptpages*4096)) == (void*)-1)
			sysfatal("segattach gpu0pt: %r");
	}
	/* a previous driver may have left an engine hung: reset both first */
	for(i = 0; i < nelem(eng); i++){
		wr(eng[i].base+Rresetctl, masked(Resetrequest, 1));
		wr(Gdrst, eng[i].resetbit);
		sleep(10);
		wr(eng[i].base+Rresetctl, masked(Resetrequest, 0));
	}
	gtinit();
	for(i = 0; i < nelem(eng); i++)
		engineinit(&eng[i]);

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
	fprint(2, "usage: %s [-dpGU] [-g #G/n] [-s srvname]\n", argv0);
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
	case 'G':
		global = 1;
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
