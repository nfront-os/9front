/*
 * Performance counters, pmc(3), arm64 part: the event counters
 * of the PMUv3 (PMEVCNTRn_EL0, PMEVTYPERn_EL0), after NIX's k8/pmcio.c
 * (Lsub, Bell Labs, Sandia, Vita Nuova; MIT license, see
 * /lib/legal/nix), which knew AMD K8's only.
 *
 * A configuration is an event number (Arm's common events, as in
 * PMCEID0_EL0) or a name of ctrdesc, which lists those the
 * processor has. The counters are 32 bits wide (PMUv3 before
 * v3p5). clock.c's cycle counter (PMCCNTR_EL0, PMCR_EL0.E) stays
 * as it is. While a counter is enabled, user mode may read the
 * event counters (PMUSERENR_EL0.ER), as RDPMC on amd64.
 *
 * The counters are reached through PMSELR_EL0 and PMXEV*_EL0
 * (sysrd/syswr make an instruction per register, sysreg.c, of a
 * small buffer); always at splhi.
 */
#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"
#include	"io.h"
#include	"sysreg.h"
#include	"../port/pmc.h"

#define PMCNTENCLR		SYSREG(3,3,9,12,2)
#define PMOVSCLR_EL0		SYSREG(3,3,9,12,3)
#define PMSELR_EL0		SYSREG(3,3,9,12,5)
#define PMCEID0_EL0		SYSREG(3,3,9,12,6)
#define PMXEVTYPER_EL0		SYSREG(3,3,9,13,1)
#define PMXEVCNTR_EL0		SYSREG(3,3,9,13,2)
#define PMINTENCLR_EL1		SYSREG(3,0,9,14,2)

enum {
	PmcrE		= 1<<0,		/* PMCR_EL0: enable */
	PmcrNshift	= 11,
	PmcrNmask	= 0x1f,

	EvtP		= 1u<<31,	/* PMEVTYPER: do not count EL1 */
	EvtU		= 1<<30,	/* do not count EL0 */

	UserenCR	= 1<<2,		/* PMUSERENR: cycle counter, clock.c */
	UserenER	= 1<<3,		/* event counters */
};

static int	pmcn;		/* counters */
static u32int	evtmask;	/* event number bits */
static u32int	ceid0;		/* PMCEID0_EL0: events 0-31 there */

static PmcCtlCtrId pmcids[] = {
	{"sw incr", "0x00"},
	{"L1I refill", "0x01"},
	{"L1I TLB refill", "0x02"},
	{"L1D refill", "0x03"},
	{"L1D access", "0x04"},
	{"L1D TLB refill", "0x05"},
	{"loads", "0x06"},
	{"stores", "0x07"},
	{"instructions", "0x08"},
	{"exceptions", "0x09"},
	{"exception returns", "0x0a"},
	{"branch misses", "0x10"},
	{"cycles", "0x11"},
	{"branches", "0x12"},
	{"mem access", "0x13"},
	{"L1I access", "0x14"},
	{"L2D access", "0x16"},
	{"L2D refill", "0x17"},
	{"bus access", "0x19"},
	{"instr spec", "0x1b"},
	{"bus cycles", "0x1d"},
	{"", ""},
};

static void
pmcipiintr(void)
{
	pmcintr();
}

int
pmcarchinit(void)
{
	int ver;

	pmcn = 0;
	ver = (sysrd(ID_AA64DFR0_EL1)>>8) & 0xf;	/* PMUVer */
	if(ver == 0 || ver == 0xf)
		return 0;
	evtmask = ver >= 4? 0xffff: 0x3ff;	/* 16 bits from PMUv3p1 */
	pmcn = (sysrd(PMCR_EL0)>>PmcrNshift) & PmcrNmask;
	if(pmcn > PmcMaxCtrs)
		pmcn = PmcMaxCtrs;
	ceid0 = sysrd(PMCEID0_EL0);
	pmcintrfn = pmcipiintr;
	return pmcn;
}

void
pmcipi(int machno)
{
	sendsgi(MACHP(machno), IRQpmc);
}

static u32int
rdtype(int regno)
{
	syswr(PMSELR_EL0, regno);
	return sysrd(PMXEVTYPER_EL0);
}

static void
wrtype(int regno, u32int v)
{
	syswr(PMSELR_EL0, regno);
	syswr(PMXEVTYPER_EL0, v);
}

/* user mode may read the counters while one is enabled */
static void
pmcuserenab(void)
{
	u64int r;

	r = sysrd(PMUSERENR_EL0);
	if((sysrd(PMCNTENSET) & ((1ull<<pmcn)-1)) != 0)
		r |= UserenER;
	else
		r &= ~UserenER;
	syswr(PMUSERENR_EL0, r | UserenCR);
}

int
pmcgetctl(PmcCtl *p, int regno)
{
	u32int t;

	if(regno < 0 || regno >= pmcn)
		return -1;
	t = rdtype(regno);
	p->enab = (sysrd(PMCNTENSET) & (1ull<<regno)) != 0;
	p->user = (t & EvtU) == 0;
	p->os = (t & EvtP) == 0;
	snprint(p->descstr, KNAMELEN, "%#ux", t & evtmask);
	p->nodesc = 0;
	p->reset = 0;
	return 0;
}

int
pmcsetctl(PmcCtl *p, int regno)
{
	u32int t;

	if(regno < 0 || regno >= pmcn)
		return -1;
	syswr(PMINTENCLR_EL1, 1ull<<regno);	/* no overflow interrupt */
	t = rdtype(regno);
	if(p->user != PmcCtlNullval){
		if(p->user)
			t &= ~EvtU;
		else
			t |= EvtU;
	}
	if(p->os != PmcCtlNullval){
		if(p->os)
			t &= ~EvtP;
		else
			t |= EvtP;
	}
	if(p->nodesc == 0){
		t &= ~evtmask;
		t |= strtoul(p->descstr, nil, 0) & evtmask;
	}
	if(p->reset != PmcCtlNullval && p->reset){
		/* as on amd64: counts nothing, off, 0 */
		syswr(PMCNTENCLR, 1ull<<regno);
		t = EvtP|EvtU;
		syswr(PMSELR_EL0, regno);
		syswr(PMXEVCNTR_EL0, 0);
		syswr(PMOVSCLR_EL0, 1ull<<regno);
	}
	wrtype(regno, t);
	if(p->enab != PmcCtlNullval){
		if(p->enab){
			syswr(PMCR_EL0, sysrd(PMCR_EL0) | PmcrE);
			syswr(PMCNTENSET, 1ull<<regno);
		}else
			syswr(PMCNTENCLR, 1ull<<regno);
	}
	pmcuserenab();
	return 0;
}

int
pmcgetctr(u64int *v, int regno)
{
	if(regno < 0 || regno >= pmcn)
		return -1;
	syswr(PMSELR_EL0, regno);
	*v = sysrd(PMXEVCNTR_EL0);
	return 0;
}

int
pmcsetctr(u64int v, int regno)
{
	if(regno < 0 || regno >= pmcn)
		return -1;
	syswr(PMSELR_EL0, regno);
	syswr(PMXEVCNTR_EL0, v);
	return 0;
}

int
pmcdescstr(char *str, int nstr)
{
	PmcCtlCtrId *pi;
	int ns, e;

	ns = 0;
	if(pmcn == 0)
		return ns;
	for(pi = pmcids; pi->portdesc[0] != '\0'; pi++){
		e = strtoul(pi->archdesc, nil, 0);
		if(e < 32 && (ceid0 & (1<<e)) == 0 && e != 0)
			continue;	/* not there; sw incr always is */
		ns += snprint(str + ns, nstr - ns, "%s\n", pi->portdesc);
	}
	return ns;
}

/*
 * In process context, before the request goes to the processor:
 * a name of ctrdesc becomes its event number, a number is checked.
 */
int
pmctrans(PmcCtl *p)
{
	PmcCtlCtrId *pi;
	char *e;
	ulong v;

	for(pi = pmcids; pi->portdesc[0] != '\0'; pi++)
		if(strcmp(p->descstr, pi->portdesc) == 0){
			strncpy(p->descstr, pi->archdesc, KNAMELEN);
			return 0;
		}
	v = strtoul(p->descstr, &e, 0);
	if(e == p->descstr || v > evtmask)
		return -1;
	while(*e == ' ' || *e == '\t')
		e++;
	return *e == '\0'? 0: -1;
}
