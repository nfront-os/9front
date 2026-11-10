#include "lib.h"
#include "sys9.h"
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <setjmp.h>
#include <stdlib.h>

extern sigset_t	_psigblocked;

static struct {
	char	*msg;	/* just check prefix */
	int	num;
} sigtab[] = {
	{"hangup",				SIGHUP},
	{"child",				SIGCHLD},	/* GCC port: gnu/sigchld.c's watcher */
	{"interrupt",				SIGINT},
	{"quit",				SIGQUIT},
	{"alarm",				SIGALRM},
	{"sys: trap: illegal instruction",	SIGILL},
	{"sys: trap: reserved instruction",	SIGILL},
	{"sys: trap: reserved",			SIGILL},
	{"sys: trap: arithmetic overflow",	SIGFPE},
	{"abort",				SIGABRT},
	{"sys: fp:",				SIGFPE},
	{"exit",				SIGKILL},
	{"die",					SIGKILL},
	{"kill",				SIGKILL},
	{"sys: trap: fault",			SIGSEGV},	/* 9front next generation: 9front's page faults (port/fault.c: "sys: trap: fault read addr=...") */
	{"sys: trap: bus error",		SIGSEGV},
	{"sys: trap: address error",		SIGSEGV},
	{"sys: trap: TLB",			SIGSEGV},
	{"sys: write on closed pipe",		SIGPIPE},
	{"term",				SIGTERM},
	{"usr1",				SIGUSR1},
	{"usr2",				SIGUSR2},
	{"virtual alarm",			SIGVTALRM},
	{"profiling alarm",			SIGPROF},
	{"sys: bad sys call",			SIGSYS},
	{"sys: breakpoint",			SIGTRAP},
};
#define NSIGTAB ((sizeof sigtab)/(sizeof (sigtab[0])))

void	(*_sighdlr[MAXSIG+1])(int, char*, Ureg*); /* 0 initialized: SIG_DFL */

/*
 * 9front-ng: sigaction's SA_SIGINFO handlers: the signal's APE handler is
 * siginfotramp, which gives them si_signo and, from a fault's note
 * ("sys: trap: fault read addr=0x..."), si_addr
 */
static void	(*sigaction3[MAXSIG+1])(int, siginfo_t*, void*);

static void
siginfotramp(int sig, char *msg, Ureg *u)
{
	siginfo_t si;
	char *a;

	memset(&si, 0, sizeof si);
	si.si_signo = sig;
	if(msg != 0 && (a = strstr(msg, "addr=")) != 0)
		si.si_addr = (void*)strtoull(a+5, 0, 0);
	(*sigaction3[sig])(sig, &si, u);
}

void
(*signal(int sig, void (*func)(int, char*, Ureg*)))(int, char*, Ureg*)
{
	void(*oldf)(int, char*, Ureg*);

	if(sig <= 0 || sig > MAXSIG){
		errno = EINVAL;
		return SIG_ERR;
	}
	oldf = _sighdlr[sig];
	if(sig == SIGKILL)
		return oldf;	/* can't catch or ignore SIGKILL */
	_sighdlr[sig] = func;
	return oldf;
}

/* BAD CODE - see /sys/src/ape/lib/ap/$objtype/setjmp.s for real code
int
sigsetjmp(sigjmp_buf buf, int savemask)
{
	int r;

	buf[0] = savemask;
	buf[1] = _psigblocked;
	return setjmp(&buf[2]);
}
*/

/*
 * BUG: improper handling of process signal mask
 */
int
sigaction(int sig, const struct sigaction *act, struct sigaction *oact)	/* GCC port: types as in the prototype, kencc ignored the difference */
{
	if(sig <= 0 || sig > MAXSIG || sig == SIGKILL){
		errno = EINVAL;
		return -1;
	}
	if(oact){
		oact->sa_handler = _sighdlr[sig];
		oact->sa_mask = _psigblocked;
		oact->sa_flags = 0;
		oact->sa_sigaction = 0;
		if(_sighdlr[sig] == siginfotramp){
			oact->sa_handler = 0;
			oact->sa_sigaction = sigaction3[sig];
			oact->sa_flags = SA_SIGINFO;
		}
	}
	if(act){
		if((act->sa_flags & SA_SIGINFO) && act->sa_sigaction != 0){
			sigaction3[sig] = act->sa_sigaction;
			_sighdlr[sig] = siginfotramp;
		}else
			_sighdlr[sig] = act->sa_handler;
	}
	return 0;
}

/* GCC port: handlers run, ever (gnu/epoll.c: epoll_pwait's window between mask and wait) */
int _sigdelivered;

/* this is registered in _envsetup */
int
_notehandler(void *u, char *msg)	/* GCC port: types as in the prototype, kencc ignored the difference */
{
	int i;
	void(*f)(int, char*, Ureg*);
	extern void _doatexits(void);	/* in stdio/exit.c */

	if(_finishing)
		_finish(0, 0);
	/* GCC port: a fault in a segment another proc of the program mapped (gnu/mman.c) */
	if(strncmp(msg, "sys: trap: fault", 16) == 0){
		extern int __shmfault(unsigned long long);
		char *a;

		if((a = strstr(msg, "addr=")) != 0 && __shmfault(strtoull(a+5, 0, 0))){
			_NOTED(0); /* NCONT: the instruction again */
			return 0;
		}
	}
	/* GCC port: a write with MSG_NOSIGNAL (gnu/nosigpipe.c): it only fails */
	if(strncmp(msg, "sys: write on closed pipe", 25) == 0){
		extern int __nosigpipe(void);

		if(__nosigpipe()){
			_NOTED(0);
			return 0;
		}
	}
	for(i = 0; i<NSIGTAB; i++){
		if(strncmp(msg, sigtab[i].msg, strlen(sigtab[i].msg)) == 0){
			f = _sighdlr[sigtab[i].num];
			if(f == SIG_DFL && sigtab[i].num == SIGCHLD){	/* GCC port: POSIX's default: ignored */
				_NOTED(0);
				return 0;
			}
			if(f == SIG_DFL || f == SIG_ERR)
				break;
			if(f != SIG_IGN) {
				_sigdelivered++;	/* GCC port: gnu/epoll.c's epoll_pwait looks */
				_notetramp(sigtab[i].num, f, u);
				/* notetramp is machine-dependent; doesn't return to here */
			}
			_NOTED(0); /* NCONT */
			return 0;
		}
	}
	_doatexits();
	_NOTED(1); /* NDFLT */
	return 0;
}

int
_stringsig(char *nam)
{
	int i;

	for(i = 0; i<NSIGTAB; i++)
		if(strncmp(nam, sigtab[i].msg, strlen(sigtab[i].msg)) == 0)
			return sigtab[i].num;
	return 0;
}

char *
_sigstring(int sig)
{
	int i;

	for(i=0; i<NSIGTAB; i++)
		if(sigtab[i].num == sig)
			return sigtab[i].msg;
	return "unknown signal";
}
