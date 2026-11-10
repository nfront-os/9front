/*
 * GCC port: MSG_NOSIGNAL (send, sendto, sendmsg: bsd/send.c, bsd/sendmsg.c).
 * A write on a pipe whose reader is gone fails and the kernel posts the
 * note "sys: write on closed pipe" to the proc that wrote (devpipe.c), seen
 * as the write returns; it is SIGPIPE, which kills by default. With
 * MSG_NOSIGNAL the write only fails: the proc is marked for the time of
 * the write, and _notehandler (plan9/signal.c) drops the note for a marked
 * proc. The marks are pids in a table, taken and given back with atomic
 * operations, no lock: the note handler reads them, and it may run while
 * another proc of the program holds a lock.
 */
#include <unistd.h>
#include <errno.h>

enum {
	Nmark = 64,	/* procs writing with MSG_NOSIGNAL at once */
};

static int marks[Nmark];

/* this proc marked: the slot, or -1 when the table is full (the note then kills, as before) */
int
__nosigpipe_begin(void)
{
	int i, pid, zero;

	pid = getpid();
	for(i = 0; i < Nmark; i++){
		zero = 0;
		if(__atomic_compare_exchange_n(&marks[i], &zero, pid, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
			return i;
	}
	return -1;
}

/* the mark taken away again; n the write's result: EPIPE, as POSIX says, for a gone reader */
long
__nosigpipe_end(int slot, long n)
{
	if(slot >= 0)
		__atomic_store_n(&marks[slot], 0, __ATOMIC_SEQ_CST);
	if(n < 0 && errno == ESHUTDOWN)
		errno = EPIPE;
	return n;
}

/* 1 if this proc writes with MSG_NOSIGNAL now (the note handler asks) */
int
__nosigpipe(void)
{
	int i, pid;

	pid = getpid();
	for(i = 0; i < Nmark; i++)
		if(__atomic_load_n(&marks[i], __ATOMIC_SEQ_CST) == pid)
			return 1;
	return 0;
}
