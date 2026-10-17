#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"
#include "ureg.h"
#include "riscv.h"

/*
 * the time csr counts at the timebase frequency, 10MHz on
 * qemu's virt machine; sbi's set_timer raises the supervisor
 * timer interrupt.
 */
static uvlong freq = 10*Mhz;

void
clockshutdown(void)
{
	setsie(getsie() & ~Stie);
}

void
clockintr(Ureg *ureg)
{
	/* the next timerset() rearms; until then no more interrupts */
	sbisettimer(~0ULL);
	timerintr(ureg, 0);
}

void
clockinit(void)
{
	char *p;

	if(m->machno == 0){
		if(p = getconf("*timebase"))
			freq = strtoull(p, 0, 0);
		print("timer frequency %lld Hz\n", freq);
	}
	m->cyclefreq = freq;
	sbisettimer(~0ULL);
	setsie(getsie() | Stie);
}

void
timerset(uvlong next)
{
	sbisettimer(next);
}

uvlong
fastticks(uvlong *hz)
{
	if(hz)
		*hz = freq;
	return vcycles();
}

ulong
perfticks(void)
{
	return fastticks(nil);
}

ulong
µs(void)
{
	uvlong hz;
	uvlong t = fastticks(&hz);
	return (t * 1000000ULL) / hz;
}

void
microdelay(int n)
{
	ulong now;

	now = µs();
	while(µs() - now < n);
}

void
delay(int n)
{
	while(--n >= 0)
		microdelay(1000);
}

void
synccycles(void)
{
	static Ref r1, r2;
	int s;

	s = splhi();
	r2.ref = 0;
	incref(&r1);
	while(*(volatile long*)&r1.ref != conf.nmach)	/* GCC port: volatile, gcc keeps the value of a load in a loop (kencc reloads) */
		;
	incref(&r2);
	while(*(volatile long*)&r2.ref != conf.nmach)	/* GCC port: volatile, gcc keeps the value of a load in a loop (kencc reloads) */
		;
	r1.ref = 0;
	splx(s);
}
