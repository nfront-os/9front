/*
 * Performance counters, pmc(3), amd64 part: AMD's counters
 * (4 at PerfEvtbase, 6 at PerfExtbase with PerfCtrExtCore) and
 * Intel's architectural ones (CPUID leaf 0xA); after NIX's
 * k8/pmcio.c (Lsub, Bell Labs, Sandia, Vita Nuova; MIT license,
 * see /lib/legal/nix), which knew AMD K8's only.
 *
 * A configuration is "event umask", numbers or a name of ctrdesc.
 * While a counter of a processor is enabled, user mode may read
 * the processor's counters with RDPMC (CR4.PCE), as in NIX.
 */
#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"
#include	"io.h"
#include	"../port/pmc.h"

enum {
	CR4Pce		= 1<<8,		/* RDPMC in user mode */

	PerfExtbase	= 0xc0010200,	/* AMD PerfCtrExtCore: sel 2n, ctr 2n+1 */
	PerfExtCore	= 1<<23,	/* CPUID 0x80000001 cx */

	IntelEvtbase	= 0x186,	/* IA32_PERFEVTSELn */
	IntelCtrbase	= 0xc1,		/* IA32_PMCn */
	IntelGlobalCtl	= 0x38f,	/* IA32_PERF_GLOBAL_CTRL, version 2 on */
};

/* event select, the same on AMD and Intel but PeEvMskH (64 bit: no enum) */
#define	PeEvMskH	0x0000000f00000000ull	/* AMD: event 11:8 */
#define	PeCtMsk		0x00000000ff000000ull	/* counter mask */
#define	PeInMsk		0x0000000000800000ull	/* invert mask */
#define	PeCtEna		0x0000000000400000ull	/* counter enable */
#define	PeInEna		0x0000000000100000ull	/* interrupt enable */
#define	PeEdg		0x0000000000040000ull	/* edge detect */
#define	PeOS		0x0000000000020000ull	/* OS mode */
#define	PeUsr		0x0000000000010000ull	/* user mode */
#define	PeUnMsk		0x000000000000ff00ull	/* unit mask */
#define	PeEvMskL	0x00000000000000ffull	/* event 7:0 */
#define	PeEvMsksh	32

static int	pmcvendor;	/* 'A', 'I' or 0 */
static int	pmcn;		/* counters */
static int	pmcintelv;	/* Intel's version */
static u32int	evtsel0, evtstep, ctr0, ctrstep;
static u64int	evtmask;	/* bits of the event select we set */

static PmcCtlCtrId amdids[] = {
	{"cycles", "0x076 0x0"},
	{"instructions", "0x0c0 0x0"},
	{"branches", "0x0c2 0x0"},
	{"branch misses", "0x0c3 0x0"},
	/* NIX's, for the K8 */
	{"locked instr", "0x024 0x0"},
	{"SMI intr", "0x02b 0"},
	{"data access", "0x040 0x0"},
	{"data miss", "0x041 0x0"},
	{"L1 DTLB miss", "0x045 0x7"},	/* L2 hit */
	{"L2 DTLB miss", "0x046 0x7"},
	{"L1 DTLB hit", "0x04d 0x3"},
	{"L2 hit", "0x07d 0x3f"},
	{"L2 miss", "0x07e 0xf"},
	{"instr miss", "0x081 0x0"},
	{"L1 ITLB miss", "0x084 0"},	/* L2 hit */
	{"L2 ITLB miss", "0x085 0x3"},
	{"DRAM access", "0x0e0 0x3f"},
	{"L3 miss", "0x4e1 0x3"},	/* one core, can be set to more */
	{"", ""},
};

/* the architectural events, in the order of CPUID.0AH:EBX */
static PmcCtlCtrId intelids[] = {
	{"cycles", "0x3c 0x0"},
	{"instructions", "0xc0 0x0"},
	{"ref cycles", "0x3c 0x1"},
	{"LLC ref", "0x2e 0x4f"},
	{"LLC miss", "0x2e 0x41"},
	{"branches", "0xc4 0x0"},
	{"branch misses", "0xc5 0x0"},
	{"", ""},
};
static ulong	intelnone;	/* CPUID.0AH:EBX: events not there */

static PmcCtlCtrId*
pmcids(void)
{
	if(pmcvendor == 'I')
		return intelids;
	return amdids;
}

static void
pmcipiintr(Ureg*, void*)
{
	pmcintr();
}

/* counter 0 keeps an event written to it, then off again */
static int
pmcprobe(void)
{
	vlong v, w;

	if(rdmsr(evtsel0, &v) < 0)
		return 0;
	if(wrmsr(evtsel0, 0x76) < 0)
		return 0;
	if(rdmsr(evtsel0, &w) < 0)
		w = 0;
	wrmsr(evtsel0, v);
	return (w & PeEvMskL) == 0x76;
}

int
pmcarchinit(void)
{
	ulong r[4];

	pmcn = 0;
	if(strncmp(m->cpuidid, "AuthenticAMD", 12) == 0){
		pmcvendor = 'A';
		evtmask = PeEvMskH|PeEvMskL|PeCtEna|PeOS|PeUsr|PeUnMsk;
		cpuid(0x80000000, 0, r);
		if(r[0] >= 0x80000001){
			cpuid(0x80000001, 0, r);
			if(r[2] & PerfExtCore){
				evtsel0 = PerfExtbase;
				ctr0 = PerfExtbase+1;
				evtstep = ctrstep = 2;
				pmcn = 6;
			}
		}
		if(pmcn == 0){
			evtsel0 = PerfEvtbase;
			ctr0 = PerfCtrbase;
			evtstep = ctrstep = 1;
			pmcn = 4;
		}
	} else if(strncmp(m->cpuidid, "GenuineIntel", 12) == 0){
		pmcvendor = 'I';
		evtmask = PeEvMskL|PeCtEna|PeOS|PeUsr|PeUnMsk;
		cpuid(0, 0, r);
		if(r[0] >= 0xa){
			cpuid(0xa, 0, r);
			pmcintelv = r[0] & 0xff;
			if(pmcintelv > 0)
				pmcn = (r[0]>>8) & 0xff;
			intelnone = r[1];
		}
		evtsel0 = IntelEvtbase;
		ctr0 = IntelCtrbase;
		evtstep = ctrstep = 1;
	}
	/*
	 * a virtual machine may have none: KVM without pmu=on reads
	 * AMD's as 0 and ignores writes; others may fault.
	 */
	if(pmcn > 0 && !pmcprobe())
		pmcn = 0;
	if(pmcn > PmcMaxCtrs)
		pmcn = PmcMaxCtrs;
	if(pmcn == 0){
		pmcvendor = 0;
		return 0;
	}
	intrenable(IrqPMC, pmcipiintr, nil, BUSUNKNOWN, "pmc");
	return pmcn;
}

void
pmcipi(int machno)
{
	lapicipiirq(machno, IrqPMC);
}

static int
rdevt(int regno, u64int *v)
{
	return rdmsr(evtsel0 + regno*evtstep, (vlong*)v);
}

static int
wrevt(int regno, u64int v)
{
	return wrmsr(evtsel0 + regno*evtstep, v);
}

/* user mode may RDPMC while a counter is enabled */
static void
pmcuserenab(void)
{
	u64int v, cr4;
	int i, any;

	any = 0;
	for(i = 0; i < pmcn; i++)
		if(rdevt(i, &v) >= 0 && (v & PeCtEna) != 0)
			any = 1;
	cr4 = getcr4();
	if(any != ((cr4 & CR4Pce) != 0))
		putcr4(any? cr4|CR4Pce: cr4&~CR4Pce);
}

int
pmcgetctl(PmcCtl *p, int regno)
{
	u64int r, e, u;

	if(regno < 0 || regno >= pmcn)
		return -1;
	if(rdevt(regno, &r) < 0)
		return -1;
	p->enab = (r&PeCtEna) != 0;
	p->user = (r&PeUsr) != 0;
	p->os = (r&PeOS) != 0;
	e = (r&PeEvMskL) | ((r&PeEvMskH)>>(PeEvMsksh-8));
	u = (r&PeUnMsk)>>8;
	snprint(p->descstr, KNAMELEN, "%#llux %#llux", e, u);
	p->nodesc = 0;
	p->reset = 0;
	return 0;
}

/* the configuration in p's descstr as pmctrans left it: "event umask" */
static void
descparse(char *s, u64int *e, u64int *u)
{
	char *p;

	*e = strtoull(s, &p, 0);
	*u = strtoull(p, nil, 0);
}

int
pmcsetctl(PmcCtl *p, int regno)
{
	u64int v, e, u, g;

	if(regno < 0 || regno >= pmcn)
		return -1;
	if(rdevt(regno, &v) < 0)
		return -1;
	v &= evtmask;
	if(p->enab != PmcCtlNullval){
		if(p->enab)
			v |= PeCtEna;
		else
			v &= ~PeCtEna;
	}
	if(p->user != PmcCtlNullval){
		if(p->user)
			v |= PeUsr;
		else
			v &= ~PeUsr;
	}
	if(p->os != PmcCtlNullval){
		if(p->os)
			v |= PeOS;
		else
			v &= ~PeOS;
	}
	if(p->nodesc == 0){
		descparse(p->descstr, &e, &u);
		v &= ~(PeEvMskL|PeEvMskH|PeUnMsk);
		v |= e & PeEvMskL;
		if(pmcvendor == 'A')
			v |= (e<<(PeEvMsksh-8)) & PeEvMskH;
		v |= (u<<8) & PeUnMsk;
	}
	if(p->reset != PmcCtlNullval && p->reset){
		v = 0;
		wrmsr(ctr0 + regno*ctrstep, 0);
	}
	if(wrevt(regno, v) < 0)
		return -1;
	/* Intel from version 2: the global enable of the counter too */
	if(pmcvendor == 'I' && pmcintelv >= 2 && (v & PeCtEna) != 0){
		if(rdmsr(IntelGlobalCtl, (vlong*)&g) >= 0 && (g & (1ull<<regno)) == 0)
			wrmsr(IntelGlobalCtl, g | 1ull<<regno);
	}
	pmcuserenab();
	return 0;
}

int
pmcgetctr(u64int *v, int regno)
{
	if(regno < 0 || regno >= pmcn)
		return -1;
	return rdmsr(ctr0 + regno*ctrstep, (vlong*)v);
}

int
pmcsetctr(u64int v, int regno)
{
	if(regno < 0 || regno >= pmcn)
		return -1;
	return wrmsr(ctr0 + regno*ctrstep, v);
}

int
pmcdescstr(char *str, int nstr)
{
	PmcCtlCtrId *pi;
	int ns, i;

	ns = 0;
	if(pmcn == 0)
		return ns;
	for(pi = pmcids(), i = 0; pi->portdesc[0] != '\0'; pi++, i++){
		if(pmcvendor == 'I' && (intelnone & (1<<i)) != 0)
			continue;
		ns += snprint(str + ns, nstr - ns, "%s\n", pi->portdesc);
	}
	return ns;
}

/*
 * In process context, before the request goes to the processor:
 * a name of ctrdesc becomes its configuration, numbers are checked.
 */
int
pmctrans(PmcCtl *p)
{
	PmcCtlCtrId *pi;
	char *s, *e;

	for(pi = pmcids(); pi->portdesc[0] != '\0'; pi++)
		if(strcmp(p->descstr, pi->portdesc) == 0){
			strncpy(p->descstr, pi->archdesc, KNAMELEN);
			return 0;
		}
	s = p->descstr;
	strtoull(s, &e, 0);
	if(e == s)
		return -1;
	s = e;
	while(*s == ' ' || *s == '\t')
		s++;
	if(*s == '\0')
		return 0;		/* umask 0 */
	strtoull(s, &e, 0);
	if(e == s)
		return -1;
	while(*e == ' ' || *e == '\t')
		e++;
	return *e == '\0'? 0: -1;
}
