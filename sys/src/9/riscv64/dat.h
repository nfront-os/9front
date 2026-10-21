/*
 * Time.
 *
 * HZ should divide 1000 evenly, ideally.
 * 100, 125, 200, 250 and 333 are okay.
 */
#define	HZ		100			/* clock frequency */
#define	MS2HZ		(1000/HZ)		/* millisec per clock tick */
#define	TK2SEC(t)	((t)/HZ)		/* ticks to seconds */

enum {
	Mhz	= 1000 * 1000,
};

typedef struct Conf	Conf;
typedef struct Confmem	Confmem;
typedef struct FPalloc	FPalloc;
typedef struct FPsave	FPsave;
typedef struct PFPU	PFPU;
typedef struct ISAConf	ISAConf;
typedef struct Label	Label;
typedef struct Memcache	Memcache;
typedef struct MMMU	MMMU;
typedef struct Mach	Mach;
typedef struct Page	Page;
typedef struct PhysUart	PhysUart;
typedef struct Pcidev	Pcidev;
typedef struct PMMU	PMMU;
typedef struct Proc	Proc;
typedef struct Soc	Soc;
typedef u64int		PTE;
typedef struct Uart	Uart;
typedef struct Ureg	Ureg;
typedef uvlong		Tval;
typedef void		KMap;

#pragma incomplete Pcidev
#pragma incomplete Ureg

#define MAXSYSARG	5	/* for mount(fd, mpt, flag, arg, srv) */

/*
 *  parameters for sysproc.c
 */
#define AOUT_MAGIC	(Y_MAGIC)

struct Label
{
	uintptr	sp;
	uintptr	pc;
#ifdef __GNUC__
	/*
	 * GCC port: jc treats every register as caller-saved, so its
	 * Label is sp+pc. Under the RISC-V psABI s0-s9 are callee-saved
	 * (s10 and s11 hold up and m): frames above setlabel() keep
	 * live values in them, which gotolabel() must give back (l.S).
	 * The kernel is built without floating point.
	 */
	uintptr	s[10];
#endif
};

struct FPsave
{
	uvlong	regs[32];
	ulong	fcsr;
};

struct FPalloc
{
	FPsave;

	FPalloc	*link;	/* when context nests */
};

#define KFPSTATE

struct PFPU
{
	int	fpstate;
	int	kfpstate;
	FPalloc	*fpsave;
	FPalloc	*kfpsave;
};

enum
{
	FPinit,
	FPactive,
	FPprotected,
	FPinactive,		/* fpsave valid when fpstate >= FPincative */

	/* bits or'd with the state */
	FPnotify = 0x100,
};

struct Confmem
{
	uintptr	base;
	ulong	npage;
	uintptr	limit;
	uintptr	kbase;
	uintptr	klimit;
};

struct Conf
{
	ulong	nmach;		/* processors */
	ulong	nproc;		/* processes */
	Confmem	mem[4];		/* physical memory */
	ulong	npage;		/* total physical pages of memory */
	ulong	upages;		/* user page pool */
	ulong	copymode;	/* 0 is copy on write, 1 is copy on reference */
	ulong	ialloc;		/* max interrupt time allocation in bytes */
	ulong	pipeqsize;	/* size in bytes of pipe queues */
	ulong	nimage;		/* number of page cache image headers */
	ulong	nswap;		/* number of swap pages */
	int	nswppo;		/* max # of pageouts per segment pass */
	int	monitor;	/* flag */
};

/*
 *  MMU stuff in Mach.
 */
struct MMMU
{
	PTE*	mmutop;		/* root page table */
};

/*
 *  MMU stuff in proc
 */
#define NCOLOR	1		/* 1 level cache, don't worry about VCE's */

struct PMMU
{
	union {
	Page	*mmufree;	/* mmuhead[0] is freelist head */
	Page	*mmuhead[PTLEVELS];
	};
	Page	*mmutail[PTLEVELS];
	int	asid;
};

/*
 * system call arguments, as jc lays them out on the user stack (its
 * va_arg in riscv64/include/u.h): types up to 4 bytes take 4 bytes,
 * wider ones are 8-byte aligned. The syscall stubs of gcc-built
 * programs build the same layout (scripts/build/gen-9syscall.sh).
 */
#define ARG(list, T) (sizeof(T) <= 4 ? \
	(((list) += 4), *(T*)((list) - 4)) : \
	(((list) = (uchar*)(((uintptr)(list) + sizeof(T) + 7) & ~7)), *(T*)((list) - sizeof(T))))

/*
 * the machine (board, emulator): where its devices are, set by the
 * configuration's soc file (socvirt.c, ...)
 */
struct Soc {
	char	*name;
	uintptr	plic;		/* physical address */
	int	nirq;		/* sources, the plic's first is 1 */
	int	plicctx0;	/* supervisor context of hart h: */
	int	plicctxstride;	/* plicctx0 + h*plicctxstride */
	uintptr	uart;		/* the console's 16550 */
	int	uartshift;	/* registers 1<<uartshift apart */
	int	uartwidth;	/* accessed as bytes (1) or words (4) */
	int	uartirq;
	ulong	uartfreq;
	uintptr	ether[2];	/* ethernet controllers (ethergem.c), 0 ends */
	int	etherirq[2];
	uintptr	sdhc;		/* standard sd host controller registers (sdhcimmio.c) */
	int	sdhcirq;
};
extern Soc soc;

#include "../port/portdat.h"

struct Mach
{
	int	machno;			/* physical id of processor */
	uintptr	splpc;			/* pc of last caller to splhi */
	Proc*	proc;			/* current process on this processor */
	uintptr	regsave[4];		/* trap entry scratch */
	int	hartid;			/* sbi's id of this processor */
	/* end of offsets known to asm */

	int	plicctx;		/* its supervisor context in the plic */

	MMMU;

	PMach;

	int	fpstate;
	FPalloc	*fpsave;

	int	cputype;
	ulong	delayloop;

	uvlong	stack[1];
};

struct
{
	char	machs[MAXMACH];		/* active CPUs */
	int	exiting;		/* shutdown */
}active;

extern Mach* machp[MAXMACH];
#define MACHP(n)	(machp[n])

#ifdef __GNUC__
/* GCC port: global register variables, the kernel and its libraries are compiled -ffixed-s10 -ffixed-s11 */
register Mach *m __asm__("s11");	/* x27 */
register Proc *up __asm__("s10");	/* x26 */
#else
extern register Mach* m;		/* R7 */
extern register Proc* up;		/* R6 */
#endif

/*
 *  a parsed plan9.ini line
 */
#define NISAOPT		8

struct ISAConf {
	char	*type;
	uvlong	port;
	int	irq;
	ulong	dma;
	ulong	mem;
	ulong	size;
	ulong	freq;

	int	nopt;
	char	*opt[NISAOPT];
};

/*
 * Horrid. But the alternative is 'defined'.
 */
#ifdef _DBGC_
#define DBGFLG		(dbgflg[_DBGC_])
#else
#define DBGFLG		(0)
#endif /* _DBGC_ */

int vflag;
extern char dbgflg[256];

#define dbgprint	print		/* for now */

/*
 *  hardware info about a device
 */
typedef struct {
	ulong	port;
	int	size;
} Devport;

struct DevConf
{
	ulong	intnum;			/* interrupt number */
	char	*type;			/* card type, malloced */
	int	nports;			/* Number of ports */
	Devport	*ports;			/* The ports themselves */
};
