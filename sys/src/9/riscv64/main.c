#include "u.h"
#include "tos.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/error.h"
#include "pool.h"
#include "io.h"
#include "riscv.h"
#include "ureg.h"

#include "rebootcode.i"

Conf conf;
Mach *machp[MAXMACH];
int boothart;

int
isaconfig(char *, int, ISAConf *)
{
	return 0;
}

/*
 *  starting place for first process
 */
void
init0(void)
{
	char buf[2*KNAMELEN], **sp;

	chandevinit();

	if(!waserror()){
		snprint(buf, sizeof(buf), "%s %s", "RISCV64", conffile);
		ksetenv("terminal", buf, 0);
		ksetenv("cputype", "riscv64", 0);
		if(cpuserver)
			ksetenv("service", "cpu", 0);
		else
			ksetenv("service", "terminal", 0);
		setconfenv();
		poperror();
	}
	kproc("alarm", alarmkproc, 0);

	sp = (char**)(USTKTOP-sizeof(Tos) - 8 - sizeof(sp[0])*4);
	sp[3] = sp[2] = sp[1] = nil;
	strcpy(sp[1] = (char*)&sp[4], "boot");
	sp[0] = (void*)&sp[1];

	splhi();
	fpukexit(nil);
	touser((uintptr)sp);
}

void
confinit(void)
{
	int userpcnt;
	ulong kpages;
	char *p;
	int i;

	conf.nmach = 1;
	if(p = getconf("*ncpu"))
		conf.nmach = strtol(p, 0, 0);
	if(conf.nmach > MAXMACH)
		conf.nmach = MAXMACH;
	if(conf.nmach < 1)
		conf.nmach = 1;

	if(p = getconf("service")){
		if(strcmp(p, "cpu") == 0)
			cpuserver = 1;
		else if(strcmp(p,"terminal") == 0)
			cpuserver = 0;
	}

	if(p = getconf("*kernelpercent"))
		userpcnt = 100 - strtol(p, 0, 0);
	else
		userpcnt = 0;

	if(userpcnt < 10)
		userpcnt = 60 + cpuserver*10;

	conf.npage = 0;
	for(i = 0; i < nelem(conf.mem); i++)
		conf.npage += conf.mem[i].npage;

	kpages = conf.npage - (conf.npage*userpcnt)/100;
	if(kpages > (KLIMIT-KZERO)/BY2PG)
		kpages = (KLIMIT-KZERO)/BY2PG;

	conf.upages = conf.npage - kpages;
	conf.ialloc = (kpages/2)*BY2PG;

	/* set up other configuration parameters */
	conf.nproc = 100 + ((conf.npage*BY2PG)/MB)*5;
	if(cpuserver)
		conf.nproc *= 3;
	if(conf.nproc > 4000)
		conf.nproc = 4000;
	conf.nswap = conf.npage*3;
	conf.nswppo = (10*MB)/BY2PG;
	conf.nimage = 200;

	conf.copymode = conf.nmach > 1;

	/*
	 * Guess how much is taken by the large permanent
	 * datastructures. Mntcache and Mntrpc are not accounted for.
	 */
	kpages = conf.npage - conf.upages;
	kpages *= BY2PG;
	kpages -= conf.upages*sizeof(Page)
		+ conf.nproc*sizeof(Proc*)
		+ conf.nimage*sizeof(Image)
		+ conf.nswap
		+ conf.nswppo*sizeof(Page*);
	mainmem->maxsize = kpages;
	imagmem->maxsize = kpages;
}

void
machinit(void)
{
	m->ticks = 1;
	m->perf.period = 1;
	active.machs[m->machno] = 1;
}

/*
 * start the other harts with sbi's hsm extension; they enter
 * _secondary (l.s) with the physical address of their Mach,
 * whose regsave[0] gives them the kernel's page table.
 */
void
mpinit(void)
{
	extern void _secondary(void);
	int i, hart;
	Mach *mm;

	for(i = 1, hart = 0; i < conf.nmach; i++, hart++){
		if(hart == boothart)
			hart++;
		mm = (Mach*)MACHADDR(i);
		memset(mm, 0, MACHSIZE);
		mm->machno = i;
		mm->hartid = hart;
		/* L1: still has the identity map of l.s, the cpus' mmutops do not */
		mm->regsave[0] = Sv39 | PADDR(L1)>>PGSHIFT;
		coherence();
		if(sbihartstart(hart, PADDR(_secondary), PADDR(mm)) < 0){
			print("cpu%d: hart %d did not start\n", i, hart);
			conf.nmach = i;
			break;
		}
	}
	synccycles();
}

void
cpuidprint(void)
{
	iprint("cpu%d: hart %d, riscv64 %s\n", m->machno, m->hartid, soc.name);
}

void
main(void)
{
	if(MACHP(0) == nil){
		/* the boot hart, l.s cleared bss and set m->hartid */
		m->machno = 0;
		boothart = m->hartid;
		machp[0] = m;
	} else {
		/* mpinit started us, m->machno and m->hartid are set */
		machp[m->machno] = m;
		machinit();
		trapinit();
		fpuinit();
		intrinit();
		clockinit();
		cpuidprint();
		synccycles();
		timersinit();
		mmu1init();
		m->ticks = MACHP(0)->ticks;
		schedinit();
	}
	machinit();
	uartconsinit();
	quotefmtinstall();
	bootargsinit();
	meminit();
	confinit();
	xinit();
	printinit();
	print("\nPlan 9\n");
	trapinit();
	fpuinit();
	intrinit();
	clockinit();
	cpuidprint();
	timersinit();
	pageinit();
	procinit0();
	mmu1init();
	links();
	chandevreset();
	userinit();
	mpinit();
	schedinit();
}

void
exit(int)
{
	cpushutdown();
	splhi();

	if(m->machno == 0){
		/* clear secrets */
		zeroprivatepages();
		poolreset(secrmem);

		sbireset(SRSTCOLD);
	}
	/* back to the firmware, which can start us again */
	sbihartstop();
	for(;;)
		idlehands();
}

static void
rebootjump(uintptr entry, void *code, ulong size)
{
	void (*f)(uintptr, uintptr, ulong, uintptr, uintptr);
	extern uintptr rebootpa;
	uintptr pa;

	intrcpushutdown();

	/* the reboot code runs at its physical address: map it there */
	memmove(KADDR(rebootpa), rebootcode, sizeof(rebootcode));
	pa = rebootpa & -PGLSZ(2);
	m->mmutop[PTLX(pa, 2)] = PA2PTE(pa) | PTEVALID|PTEREAD|PTEWRITE|PTEEXEC|PTEKERNEL;
	flushtlb();
	fencei();

	f = (void*)rebootpa;
	(*f)(entry, PADDR(code), size, m->hartid, dtbpa);

	for(;;);
}

void
reboot(void*, void *code, ulong size)
{
	writeconf();
	while(m->machno != 0){
		procwired(up, 0);
		sched();
	}

	cpushutdown();
	delay(2000);

	splhi();

	/* turn off buffered serial console */
	serialoq = nil;

	/* shutdown devices */
	chandevshutdown();

	/* stop the clock */
	clockshutdown();
	intrsoff();

	/* clear secrets */
	zeroprivatepages();
	poolreset(secrmem);

	/* off we go - never to return */
	rebootjump(PHYSKTZERO, code, size);
}

/* dma is coherent on qemu's virt machine */
void
dmaflush(int, void*, ulong)
{
	coherence();
}
