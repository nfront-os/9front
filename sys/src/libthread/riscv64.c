#include <u.h>
#include <libc.h>
#include <thread.h>
#include "threadimpl.h"

#ifdef __GNUC__
/*
 * GCC port: jc's launcher below takes the first argument in R8 and the
 * rest from the stack. Under the RISC-V psABI the first eight integer
 * arguments are in a0-a7 and only the 9th and 10th come from the stack
 * (0(sp), 8(sp) at entry): eight unused register parameters put f and
 * arg where the stack built below has them. sp stays 16-byte aligned.
 */
static void
launcherriscv64(uintptr, uintptr, uintptr, uintptr, uintptr, uintptr, uintptr, uintptr,
	void (*f)(void *arg), void *arg)
{
	(*f)(arg);
	threadexits(nil);
}

void
_threadinitstack(Thread *t, void (*f)(void*), void *arg)
{
	uintptr *tos;

	tos = (uintptr*)&t->stk[t->stksize&~15];
	*--tos = (uintptr)arg;
	*--tos = (uintptr)f;

	t->sched[JMPBUFPC] = (uintptr)launcherriscv64+JMPBUFDPC;
	t->sched[JMPBUFSP] = (uintptr)tos;
}
#else
/* first argument goes in a register; simplest just to ignore it */
static void
launcherriscv64(int, void (*f)(void *arg), void *arg)
{
	if (f == nil)
		sysfatal("launcherriscv64: nil f passed: arg %#p", arg);
	(*f)(arg);
	threadexits(nil);
}

void
_threadinitstack(Thread *t, void (*f)(void*), void *arg)
{
	uintptr *tos;

	tos = (uintptr *)&t->stk[t->stksize&~7];
	*--tos = (uintptr)arg;
	*--tos = (uintptr)f;
	t->sched[JMPBUFPC] = (uintptr)launcherriscv64+JMPBUFDPC;
	t->sched[JMPBUFSP] = (uintptr)tos - 2*sizeof(uintptr); /* 1st arg, return PC */
}
#endif
