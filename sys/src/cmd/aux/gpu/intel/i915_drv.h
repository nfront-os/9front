/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: what the driver's files share (Linux's i915_drv.h and
 * the headers next to each file): its types, registers, state and
 * functions.  Parts follow Linux's i915 (MIT); notices in NOTICE.
 */

#include "linux.h"


/* a context's address space: 48 bits (four levels), with -3 32 bits (three) */
#define Pplimit48	(1ULL<<48)	/* all of the 48 bits (hasvk's heaps go to the top) */
#define Pplimit32	0xFFF00000ULL
#define Ptaddr		0xFFFFFFFFF000ULL	/* the address in an entry */

enum {
	Pcigmch		= 0x50,		/* GMCH graphics control (16 bits) */

	Forcewakemt	= 0xA188,
	Forcewakeack	= 0x130044,
	Fwrange		= 0x40000,	/* gen 8: the GT's registers below need forcewake (intel_uncore.c) */
	Fwidle		= 5,		/* ms without an access: forcewake off, the GT may sleep (RC6) */

	/* RC6 and RPS (i915's gen8_enable_rps, gen6_set_rps; intel_pm.c) */
	Rcstate		= 0xA094,	/* GEN6_RC_STATE */
	Rccontrol	= 0xA090,	/* GEN6_RC_CONTROL */
	Rcctlrc6	= 1<<18,
	Rcctlto		= 1<<28,	/* GEN7_RC_CTL_TO_MODE */
	Rcctlhw		= 1<<31,
	Rc6wakerate	= 0xA09C,
	Rcevalint	= 0xA0A8,
	Rcidlehyst	= 0xA0AC,
	Rcsleep		= 0xA0B0,
	Rc6threshold	= 0xA0B8,
	Rmaxidle	= 0x54,		/* RING_MAX_IDLE, from the engine's base */
	Rpnswreq	= 0xA008,	/* the frequency asked for: HSW_FREQUENCY(x) = x<<24 */
	Rcvideofreq	= 0xA00C,
	Rpdowntimeout	= 0xA010,
	Rpintlimits	= 0xA014,
	Rpstat1		= 0xA01C,	/* actual frequency: bits 13:7 (HSW_CAGF), 50 MHz units */
	Rpcontrol	= 0xA024,
	Rpctlbits	= 1<<11 | 2<<9 | 1<<8 | 1<<7 | 2<<3 | 2<<0,	/* media turbo, hw normal mode, media is gfx, enable, up busy avg, down idle avg */
	Rpupthreshold	= 0xA02C,
	Rpdownthreshold	= 0xA030,
	Rpupei		= 0xA068,
	Rpdownei	= 0xA06C,
	Rpidlehyst	= 0xA070,
	Rpstatecap	= 0x145998,	/* RP0 7:0, RP1 15:8, RPn 23:16 (MCHBAR mirror) */
	Pmintrmsk	= 0xA168,
	Pmredirect	= 1<<31,	/* GEN8_PMINTR_REDIRECT_TO_NON_DISP */
	Pmdowntimeout	= 1<<6,
	Pmupthreshold	= 1<<5,
	Pmdownthreshold	= 1<<4,
	Pmupeiexpired	= 1<<2,
	Pmdowneiexpired	= 1<<1,
	Pmrpsevents	= Pmupthreshold | Pmdownthreshold | Pmdowntimeout,
	Gtisr2		= 0x44320,	/* bank 2: PM; IMR +4, IIR +8, IER +0xC */
	Rc6residency	= 0x138108,	/* GEN6_GT_GFX_RC6, 1.28 µs units */
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
	Desclegacy64	= 3<<3,		/* LEGACY_64B_CONTEXT << 3: 48-bit, four levels */
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

	/* fuses: slices, subslices, EUs (i915's broadwell_sseu_info_init) */
	Fuse2		= 0x9120,
	Eudisable0	= 0x9134,
	Eudisable1	= 0x9138,
	Eudisable2	= 0x913C,

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
	Dspoffset	= 0x701A4,	/* HSW/BDW: DSPOFFSET, y<<16 | x */
	Dspsurflive	= 0x701AC,	/* the surface the plane scans out now */
	Dspfmtmask	= 0xF<<26,
	Dspbgrx		= 6<<26,	/* x8r8g8b8 */
	Dsptiled	= 1<<10,
	Scanalign	= 256*1024,	/* a scanout surface's alignment (gen 9: 256 KB, gen 8: 4 KB) */

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
	Ptewt		= 1<<4,		/* index 2: write-through in the LLC, for what the display reads */

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
	/* DDI ports A-D */
	Ddibufctl	= 0x64000,	/* DDI_BUF_CTL(port): +0x100 a port */
	Ddiinitdetected	= 1<<0,		/* port A: a display (eDP) */
	Sfusestrap	= 0xC2014,	/* ports B, C, D present: bits 2, 1, 0 */
	Sdeisr		= 0xC4000,	/* the PCH's hotplug status: ports B, C, D at 21, 22, 23 */
	Portclksel	= 0x46100,	/* PORT_CLK_SEL(port): +4 a port */
	Lcpllctl	= 0x130040,
	Fusestrap	= 0x42014,
	Rawclkfreq	= 0xC6204,	/* PCH_RAWCLK_FREQ, MHz */

	/* transcoders A, B, C, eDP */
	Transddifuncctl	= 0x400,	/* from the transcoder's base */
	Tddienable	= 1<<31,
	Tddiedpinput	= 7<<12,
	Transhtotal	= 0x00,
	Transvtotal	= 0x0C,
	Translinkm	= 0x40,
	Translinkn	= 0x44,
	Pipeconf	= 0x70008,	/* PIPECONF(cpu_transcoder): A, B, C +0x1000, eDP at 0x7F008 */
	Pipeenable	= 1<<31,

	/* DP AUX */
	Auxsendbusy	= 1<<31,
	Auxdone		= 1<<30,
	Auxtimeouterr	= 1<<28,
	Auxtimeout600	= 1<<26,
	Auxrecverr	= 1<<25,
	Auxsizeshift	= 20,
	Auxprecharge	= 5<<16,

	Auxi2cwrite	= 0x0,
	Auxi2cread	= 0x1,
	Auxi2cmot	= 0x4,
	Auxnativeread	= 0x9,
	Replynative	= 3,		/* ack 0, nack 1, defer 2 */
	Replyi2c	= 3<<2,

	/* GMBUS (the PCH's I2C) */
	Gmbus0		= 0xC5100,	/* pin, rate */
	Gmbus1		= 0xC5104,	/* command */
	Gmbus2		= 0xC5108,	/* status */
	Gmbus3		= 0xC510C,	/* data */
	Gmbus4		= 0xC5110,	/* interrupt mask */
	Gmswclrint	= 1<<31,
	Gmswrdy		= 1<<30,
	Gmcyclewait	= 1<<25,
	Gmcycleindex	= 2<<25,
	Gmcyclestop	= 4<<25,
	Gmactive	= 1<<9,
	Gmsatoer	= 1<<10,
	Gmhwrdy		= 1<<11,

	Ddcaddr		= 0x50,

	/* the modeset: power well, PLLs, buffers, transcoder B, pipe B, plane B */
	Pwrwelldriver	= 0x45404,	/* HSW_PWR_WELL_DRIVER */
	Pwrwellreq	= 1<<31,
	Pwrwellon	= 1<<30,
	Wrpllctl	= 0x46040,	/* WRPLL_CTL1, +0x20: WRPLL_CTL2 */
	Wrpllenable	= 1<<31,
	Wrplllcpll	= 3<<28,	/* reference: the LCPLL */
	Ddibuftransreg	= 0x64E00,	/* DDI_BUF_TRANS(port): +0x60 a port, 8 an entry */
	Ddibufidle	= 1<<7,
	Ddibufenable	= 1<<31,
	Transclksel	= 0x46140,	/* TRANS_CLK_SEL(transcoder): +4 */
	Transhblank	= 0x04,
	Transhsync	= 0x08,
	Transvblank	= 0x10,
	Transvsync	= 0x14,
	Transvsyncshift	= 0x28,
	Transpipemult	= 0x2C,
	Gammamode	= 0x4A480,	/* +0x800 a pipe */
	Pipemisc	= 0x70030,	/* +0x1000 a pipe */
	Pipestate	= 1<<30,	/* PIPECONF: the pipe runs */
	Tddibpc8	= 0<<20,
	Tddipvsync	= 1<<17,
	Tddiphsync	= 1<<16,
	Tddidvi		= 1<<24,
	Dspenable	= 1<<31,
	Mchsskpd	= 0x145D10,	/* MCH_SSKPD: the memory's latencies (64 bits) */
	Wm0pipe		= 0x45100,	/* WM0_PIPE[ABC]_ILK: +4 a pipe */
	Wm1lp		= 0x45108,	/* WM1-3_LP_ILK */
	Wm1slp		= 0x45120,

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
extern ulong	transbase[4];
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

