#include <u.h>
#include <libc.h>
#include <thread.h>
#include "threadimpl.h"

#ifdef __GNUC__
/*
 * GCC port: front's launcherarm64(int, f, arg) relies on 7c's
 * convention (first argument in a register, the rest on the stack),
 * so _threadinitstack() leaves f and arg on the new stack. Under
 * AAPCS64 the first eight arguments are in x0-x7 and only the 9th and
 * 10th come from the stack ([sp], [sp+8] at entry): eight unused
 * register parameters put f and arg where the stack built below has
 * them. sp stays 16-byte aligned.
 */
static void
launcherarm64(uintptr, uintptr, uintptr, uintptr, uintptr, uintptr, uintptr, uintptr,
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

	t->sched[JMPBUFPC] = (uintptr)launcherarm64+JMPBUFDPC;
	t->sched[JMPBUFSP] = (uintptr)tos;
}
#else
/* first argument goes in a register; simplest just to ignore it */
static void
launcherarm64(int, void (*f)(void *arg), void *arg)
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
	*--tos = 0;	/* first arg to launcherarm64 */
	*--tos = 0;	/* place to store return PC */

	t->sched[JMPBUFPC] = (uintptr)launcherarm64+JMPBUFDPC;
	t->sched[JMPBUFSP] = (uintptr)tos;
}
#endif
