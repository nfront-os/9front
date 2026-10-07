#include <u.h>
#include <libc.h>
#include <thread.h>
#include "threadimpl.h"

/*
 * GCC port: front's launcheramd64(int, f, arg) relies on 6c's
 * convention (first argument in a register, the rest on the stack),
 * so _threadinitstack() leaves f and arg on the new stack. Under System
 * V the first six integer arguments are in registers and only the 7th
 * onwards come from the stack (8(%rsp), 16(%rsp), ... at entry), so
 * the six unused register parameters push the stack layout built
 * below into the zero/f/arg positions.
 */
static void
launcheramd64(uintptr, uintptr, uintptr, uintptr, uintptr, uintptr,
	uintptr, void (*f)(void *arg), void *arg)
{
	(*f)(arg);
	threadexits(nil);
}

void
_threadinitstack(Thread *t, void (*f)(void*), void *arg)
{
	uvlong *tos;

	/* GCC port: 16-byte align (SysV: %rsp+8 aligned at function entry) */
	tos = (uvlong*)((uintptr)&t->stk[t->stksize] & ~(uintptr)15);
	*--tos = 0;	/* keep alignment */
	*--tos = (uvlong)arg;
	*--tos = (uvlong)f;
	*--tos = 0;	/* first arg to launcheramd64 */
	t->sched[JMPBUFPC] = (uvlong)launcheramd64+JMPBUFDPC;
	t->sched[JMPBUFSP] = (uvlong)tos - 2*8;		/* old PC and new PC */
}
