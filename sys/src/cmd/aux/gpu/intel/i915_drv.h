/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: what the driver's files share (Linux's i915_drv.h and
 * the headers next to each file): its types, registers, state and
 * functions.  Parts follow Linux's i915 (MIT); notices in NOTICE.
 */

#include "linux.h"
#include "i915_reg_defs.h"
#include "display/intel_display_limits.h"
#include "display/intel_display.h"
#include "display/intel_display_reg_defs.h"
#include "i915_reg.h"
#include "intel_mchbar_regs.h"
#include "gt/intel_engine_regs.h"
#include "gt/intel_gt_regs.h"
#include "gt/intel_lrc_reg.h"
#include "gt/intel_lrc.h"
#include "gt/intel_gpu_commands.h"
#include "gt/intel_gtt.h"
#include "display/intel_dp_aux_regs.h"
#include "display/intel_gmbus_regs.h"
#include "drm/drm_dp.h"
#include "drm/drm_edid.h"


/* a context's address space: 48 bits (four levels), with -3 32 bits (three) */
#define Pplimit48	(1ULL<<48)	/* all of the 48 bits (hasvk's heaps go to the top) */
#define Pplimit32	0xFFF00000ULL
#define Ptaddr		0xFFFFFFFFF000ULL	/* the address in an entry */

enum {
	Pcigmch		= 0x50,		/* GMCH graphics control (16 bits) */

	Fwrange		= 0x40000,	/* gen 8: the GT's registers below need forcewake (intel_uncore.c) */
	Fwidle		= 5,		/* ms without an access: forcewake off, the GT may sleep (RC6) */

	/* the RPS events we take (i915's gen6_pm_rps_work) */
	Pmrpsevents	= GEN6_PM_RP_UP_THRESHOLD | GEN6_PM_RP_DOWN_THRESHOLD | GEN6_PM_RP_DOWN_TIMEOUT,

	Scanalign	= 256*1024,	/* a scanout surface's alignment (gen 9: 256 KB, gen 8: 4 KB) */

	/*
	 * PDEs: bit 7 is the 2 MB page bit there, not PAT (the blitter
	 * honours it): index 2, write-through in the LLC, as the GPU only
	 * reads them (i915's gen8_pde_encode uses no PAT bit either)
	 */
	Pdecached	= PPAT_DISPLAY_ELLC,

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
	Ppbase		= 0x100000,	/* a context's objects from 1 MB (the driver's choice) */
	Rsva		= 0x1000,	/* the null render state's page in every context */

	Nctx		= 64,
	Nbo		= 1024,
	Bomax		= 1<<30,	/* an object's size; their addresses in the clients 1 GB apart */
	Fbva		= 0x3000000000ULL,	/* the driver's frame buffers (display.c), 1 GB apart */
	Anyaddr		= ~0,		/* bomap: the driver chooses the GPU address */
	Fwin		= 256,		/* fences in flight per context */
	Timeout		= 5000,		/* ms before a fence is given up */
};

/* display (display/) */
enum {
	Hdmimaxclock	= 300000,	/* kHz: HDMI 1.4 on DDI (hdmi_port_clock_limit) */
	Ourpipe		= 1,		/* B: A drives the panel */
};

typedef struct Engine Engine;
typedef struct Ectx Ectx;
typedef struct Bo Bo;
typedef struct Ctx Ctx;
typedef struct Fidaux Fidaux;
typedef struct Hole Hole;
typedef struct Rps Rps;
typedef struct Displayinfo Displayinfo;
typedef struct Scan Scan;
typedef struct Wa Wa;


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
	uvlong	gtt;		/* the GPU address: in the context's PPGTT, or the GTT (-G); 0: not mapped */
	int	plane;		/* the visible plane mapped (map plane), no segment */
	ulong	scanbase, scangtt;	/* in the GTT for the display (scanout), aligned */
	int	scanwt;		/* mapped write-through in the PPGTT: the display reads memory */
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

	/*
	 * the address space: a tree of pool pages from the PML4 down, the
	 * scratch tables where nothing is (with -3 the four page
	 * directories under its first entry are the PDP registers)
	 */
	ulong	serial;
	int	pml4;
	Hole	*holes;
};

struct Fidaux {
	Ctx	*ctx;
	uvlong	fence;
	int	query;
};

struct Hole {
	Hole	*next;
	uvlong	start, len;	/* GPU addresses, bytes */
};

/* RPS: frequencies in 50 MHz units (i915's rps) */
enum { Lowpower, Between, Highpower };
struct Rps {
	int	on;
	int	rp0, rp1, min, max, efficient, idle;
	int	minsoft, maxsoft;
	int	cur, power, lastadj;
	u32int	pmimr;		/* GEN8_GT_IMR(2) */
	vlong	busy;		/* last seen busy */
};

/*
 * the scanout: a client's object on the display plane instead of the
 * draw screen (aux/vga's), the plane's registers as they were
 */
struct Scan {
	Bo	*bo;
	int	p;
	u32int	cntr, stride, offset, surf;
	ulong	flip;		/* a new surface written: the request waits until it is live */
};

/* a workaround: a masked register, the bits to set in the upper half, their values below */
struct Wa {
	ulong	reg;
	u32int	val;
};


/* what Linux's DISPLAY_INFO() gives the register macros (display/intel_display.c) */
struct Displayinfo {
	u32int	mmio_offset;
	u32int	pipe_offsets[4];
	u32int	trans_offsets[4];
};

/* the outputs (display/) */
typedef struct Output Output;
struct Output {
	char	name[16];	/* eDP-1, DP-1, HDMI-A-1: Linux's names */
	int	port;		/* DDI A-D: 0-3 */
	int	type;		/* Oedp, Odp, Ohdmi */
	int	connected;
	uchar	edid[256];
	int	nedid;
	int	trans;		/* the transcoder driving it (Tedp, 0-2), -1 none */
	int	pipe;		/* the pipe feeding that transcoder, -1 none */
	int	w, h;		/* its active size; refresh in mHz */
	long	refresh;
	/* set by this driver (outsetmode): the frame buffer on its plane */
	int	ours;
	ulong	fbgtt, fbsize;
	u32int	*fb;
	char	fbseg[32];
	int	wrpll;		/* WRPLL_CTL1 or 2 (0, 1), -1 none */
};
enum { Oedp, Odp, Ohdmi };
enum { Tedp = 3 };
enum { Maxout = 8 };

extern Output	outputs[Maxout];
extern int	noutputs;

/*
 * the EDID's detailed timings, to set (the standard timings carry no
 * timing); the first one is the preferred
 */
typedef struct Timing Timing;
struct Timing {
	int	clock;		/* kHz */
	int	ha, hss, hse, ht;
	int	va, vss, vse, vt;
	int	phsync, pvsync;
	int	hz;
};

/* the file server's files (plan9/fs.c; contexts' made by newctx) */
enum {
	Xctl,
	Xstatus,
	Xclone,
	Xctxctl,
	Xctxsubmit,
	Xctxfence,

	Xoutedid = 0,
	Xoutmodes,
	Xoutmode,
	Xout = 0x100,		/* outputs/<n>/: Xout | n<<4 | Xoutedid... */
	Xctxbase = 0x1000,
};

#define DBG	if(debug) fprint

/* intelgpu.c */
extern char	*gdir;
extern int	debug, poll, uncached, global;
extern int	noclockgating, norcsinit, nopm, legacy32;
extern QLock	lk;		/* engines, fences, the GTT */
extern Rendez	done;		/* fences completed */
extern QLock	ctxlock;	/* contexts, objects */
/* plan9/fs.c */
extern File	*ctxdir;
extern Srv	fs;
/* intel_uncore.c */
extern Lock	fwlk;
extern int	fwheld;
extern vlong	fwlast;
/* i915_driver.c */
extern int	gctlfd, irqfd;
extern u32int	*mmio;
extern uvlong	mmiosize;
extern uvlong	drvpa;
extern u32int	*drv;
extern ulong	drvgtt;
/* i915_irq.c */
extern ulong	nirq;
/* gt/gen8_ppgtt.c */
extern u32int	*ptpool;
extern uvlong	ptpa;
extern uchar	ptused[Ptpages];
extern ulong	serial;
/* gt/intel_ggtt.c */
extern uvlong	*gtt;
extern uvlong	nent;
extern Hole	*holes;
/* gt/intel_gtt.c */
extern uvlong	scratchpte, scratchgtt;
extern uvlong	scratchtab[3], scratchfill[4];
/* gt/intel_renderstate.c */
extern ulong	rsgtt;
extern uvlong	rspa;
/* gt/intel_sseu.c */
extern int	sslices, ssubslices, seus;
/* gt/intel_rps.c */
extern Rps	rps;
/* gt/intel_workarounds.c */
extern Wa	rcswa[11];
/* gt/intel_engine_cs.c */
extern Engine	eng[2];
/* gem/i915_gem_context.c */
extern u32int	*ctxpool;
extern uvlong	ctxpoolpa;
extern ulong	ctxpoolgtt;
extern Ctx	*ctxs[Nctx];
/* gem/i915_gem_object.c */
extern Bo	*bos[Nbo];
/* display/i9xx_plane.c */
extern Scan	scan;
/* display/intel_display.c */
extern Output	outputs[Maxout];
extern int	noutputs;
extern Displayinfo	bdw_display_info;
extern char	*transname[4];
/* display/intel_cdclk.c */
extern int	cdclk;

/* intelgpu.c */
void	usage(void);

/* plan9/fs.c */
int	ftype(File *f);
Ctx*fctx(File *f);
char*ctxreply(Ctx *c, char *fmt, ...);
char*planestr(void);
char*statusstr(void);
void	fsopen(Req *r);
void	fsdestroyfid(Fid *f);
void	fsread(Req *r);
void	fsend(Srv*);
char*ctxctl(Ctx *c, char **f, int n);
char*devctl(char **f, int n);
void	fswrite(Req *r);
void	mktree(void);

/* plan9/outputs.c */
int	addmode(char *b, int n, int len, int w, int h, int hz);
int	dtd(char *b, int n, int len, uchar *d);
char*outmodes(Output *o);
char*outmode(Output *o);
char*dispstatus(void);
int	parsewxh(char *s, int *w, int *h);
void	testpicture(Output *o, int w, int h);
char*outsetmode(Output *o, char *req);

/* plan9/sys.c */
char*readfile(char *path, char *buf, int n);
char*ctlline(char *key, char *buf, int n);
uvlong	fixedseg(char *name, uvlong va, ulong size, int perm);
uvlong*stickyseg(char *name, uvlong va, ulong size, int perm);
void	rmseg(char *name);
void	flushcache(void *p, ulong n);
void	clflushrange(void *p, ulong n);
ulong	fbnew(ulong size, u32int **p, char *seg, int nseg);
void	microwait(int us);

/* i915_driver.c */
ulong	cfg16(int off);
void	devinit(void);

/* intel_uncore.c */
void	fwon(void);
u32int	rd(ulong r);
void	wr(ulong r, u32int v);
u32int	masked(u32int bits, int on);
int	forcewake(void);
void	fwproc(void);
int	waitbits(ulong r, u32int mask, u32int want, int ms);

/* i915_irq.c */
void	irqinit(void);
void	irqproc(void);

/* i915_request.c */
int	fencedone(Ctx *c, uvlong k);
void	reap(void);
void	tickproc(void);
int	fencewait(Ctx *c, uvlong k);

/* intel_clock_gating.c */
void	gtinit(void);

/* gt/intel_gtt.c */
void	ppatinit(void);
void	scratchinit(void);

/* gt/intel_ggtt.c */
uvlong	holealloc(Hole **list, uvlong len);
int	holetake(Hole **list, uvlong a, uvlong len);
void	holefree(Hole **list, uvlong a, uvlong len);
ulong	gttalloc(ulong len);
void	gttfree(ulong a, ulong len);
void	gttmap(ulong a, uvlong pa, uvlong *pas, ulong len);
void	gttunmap(ulong a, ulong len);

/* gt/gen8_ppgtt.c */
int	ptalloc(uvlong fill);
void	ptfree(int i);
uvlong	ptphys(int i);
uvlong*ptmem(int i);
int	ptindex(uvlong pa);
uvlong*ppwalk(Ctx *c, uvlong a, int level, int alloc);
void	pptfree(int n, int level);
uvlong	pdpval(Ctx *c, int n);
int	ppinit(Ctx *c);
void	ppfree(Ctx *c);
int	ppmap(Ctx *c, uvlong a, uvlong pa, uvlong *pas, uvlong len, uvlong bits);
void	ppunmap(Ctx *c, uvlong a, uvlong len);

/* gt/intel_engine_cs.c */
void	enginestart(Engine *e);
void	engineinit(Engine *e);

/* gt/intel_lrc.c */
void	ecimage(Ectx *ec);
void	ecinit(Ctx *c, Ectx *ec, Engine *e, ulong off);
void	ecupdate(Ectx *ec);
uvlong	descriptor(Ectx *ec);

/* gt/intel_execlists_submission.c */
void	kick(Engine *e);
void	enqueue(Ectx *ec);
void	csb(Engine *e);
void	ecemit(Ectx *ec, u32int v);
u32int	ecdone(Ectx *ec);
int	ecspace(Ectx *ec, int n);
char*ecsubmit(Ectx *ec, uvlong a, u32int *seq);

/* gt/intel_reset.c */
void	engreset(Engine *e);

/* gt/intel_renderstate.c */
void	rsinit(void);

/* gt/intel_sseu.c */
void	sseuinit(void);

/* gt/intel_rps.c */
int	gtinterval(int us);
u32int	rpslimits(int val);
u32int	rpspmmask(int val);
void	rpsthresholds(int val);
void	rpsset(int val);
void	rpswork(u32int pm);
void	rpsidle(void);
void	rpsinit(void);

/* gem/i915_gem_object.c */
Bo*findbo(Ctx *c, int id);
char*bomap(Bo *b, uvlong at);
void	bounmap(Bo *b);
char*bonew(Ctx *c, uvlong size, uvlong at);
char*bobind(Bo *b, uvlong at);
void	bofree(Bo *b);

/* gem/i915_gem_context.c */
ulong	ctxslot(void);
int	ctxmem(Ctx *c, int i);
void	ctxmemfree(Ctx *c);
Ctx*newctx(void);
void	freectx(Ctx *c);

/* gem/i915_gem_execbuffer.c */
char*submit(Ctx *c, int boid, ulong off, char *engname);

/* display/i9xx_plane.c */
int	plane(ulong *surf, ulong *stride, int *bpp, int *w, int *h, int *tiled);
char*mapplane(Ctx *c);
void	scanrestore(void);
char*scanout(Ctx *c, int id, int w, int h, int stride);
void	flipwait(Req *r, ulong surf, int p);

/* display/intel_display.c */
void	readmode(Output *o);
Output*newout(char *name, int port, int type);
void	dispinit(void);
void	modeoff(Output *o);
char*modeset(Output *o, Timing *t);

/* display/intel_cdclk.c */
int	getcdclk(void);

/* display/intel_dp_aux.c */
ulong	auxctl(int port);
int	auxch(int port, uchar *send, int n, uchar *recv, int nr);
int	auxmsg(int port, int req, int addr, uchar *buf, int size);
int	auxedid(int port, int off, uchar *buf, int n);

/* display/intel_dp.c */
int	dpsink(int port);

/* display/drm_dp_helper.c */
int	auxi2c(int port, int req, uchar *buf, int size);

/* display/intel_gmbus.c */
int	gmwait(u32int bits);
void	gmreset(void);
int	gmedid(int pin, int off, uchar *buf, int n);

/* display/intel_dpll_mgr.c */
uint	wrpllbudget(int clock);
uvlong	absdiff(uvlong a, uvlong b);
void	wrpllupdate(uvlong freq2k, uint budget, uint r2, uint n2, uint p, uint *bp, uint *bn2, uint *br2);
u32int	wrpllval(int clock);

/* drm_edid.c */
int	edidok(uchar *e);
void	readedid(Output *o);
int	dtdtiming(uchar *d, Timing *t);
int	findtiming(Output *o, int w, int h, int hz, Timing *t);

