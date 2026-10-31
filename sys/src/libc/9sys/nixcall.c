/*
 * NIX: system calls through a queue in the Tos, after NIX's
 * libc/9sys/nixcall.c. A kernel process makes the calls
 * (port/nixcall.c); nixsyscall() starts it and wakes it.
 */
#include <u.h>
#include <libc.h>
#include <tos.h>

#define	NEXT(i)	(((i)+1)%CALLQSZ)

static void
barrier(void)
{
#ifdef __GNUC__
	__sync_synchronize();
#else
	static long x;

	ainc(&x);
#endif
}

/*
 * Queue the system call nc; wake the kernel process if it sleeps
 * and wakekernel is set. -1 if the queue is full.
 */
int
nixcall(Nixcall *nc, int wakekernel)
{
	Callq *callq;

	callq = &_tos->callq;
	if(NEXT(callq->qw) == ((volatile Callq*)callq)->qr){
		werrstr("call queue full");
		return -1;
	}
	memmove(&callq->q[callq->qw], nc, sizeof *nc);
	barrier();
	callq->qw = NEXT(callq->qw);
	barrier();
	if(wakekernel && ((volatile Callq*)callq)->ksleep)
		nixsyscall();
	return 0;
}

/*
 * The next reply: its tag, nil if there is none yet.
 */
void*
nixret(Nixret *nr)
{
	Callq *callq;
	void *tag;

	callq = &_tos->callq;
	if(callq->rr == ((volatile Callq*)callq)->rw)
		return nil;
	barrier();
	tag = callq->r[callq->rr].tag;
	if(nr != nil)
		*nr = callq->r[callq->rr];
	barrier();
	callq->rr = NEXT(callq->rr);
	return tag;
}
