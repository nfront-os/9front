#ifndef __SIGNAL_H
#define __SIGNAL_H

#if defined(__GNUC__) || defined(_APE_GNU)
/* GCC port: APE declares signal handlers K&R-style, "void (*)()", deliberately
 * without parameter types (some take the note and a Ureg too). C++ and C23 read
 * "()" as (void): no handler fits. For them the POSIX declarations, void (*)(int),
 * of the same libap functions; APE's are renamed while the header is read
 * (struct sigaction keeps its layout). C up to C17 gets APE's as they are. */
#if defined __cplusplus || (defined __STDC_VERSION__ && __STDC_VERSION__ > 201710L)
#define __APE_POSIX_SIGNAL
#define signal		__ape_signal
#define sigaction	__ape_sigaction
#endif
#endif
#pragma lib "/$M/lib/ape/libap.a"

typedef int sig_atomic_t;

/*
 * We don't give arg types for signal handlers, in spite of ANSI requirement
 * that it be 'int' (the signal number), because some programs need an
 * additional context argument.  So the real type of signal handlers is
 *      void handler(int sig, char *, struct Ureg *)
 * where the char * is the Plan 9 message and Ureg is defined in <ureg.h>
 */
#define SIG_DFL ((void (*)())0)
#define SIG_ERR ((void (*)())-1)
#define SIG_IGN ((void (*)())1)

#define	SIGHUP	1	/* hangup */
#define	SIGINT	2	/* interrupt */
#define	SIGQUIT	3	/* quit */
#define	SIGILL	4	/* illegal instruction (not reset when caught)*/
#define SIGABRT 5	/* used by abort */
#define	SIGFPE	6	/* floating point exception */
#define	SIGKILL	7	/* kill (cannot be caught or ignored) */
#define	SIGSEGV	8	/* segmentation violation */
#define	SIGPIPE	9	/* write on a pipe with no one to read it */
#define	SIGALRM	10	/* alarm clock */
#define	SIGTERM	11	/* software termination signal from kill */
#define	SIGUSR1	12	/* user defined signal 1 */
#define	SIGUSR2	13	/* user defined signal 2 */
#define	SIGBUS	14	/* bus error */

/* The following symbols must be defined, but the signals needn't be supported */
#define SIGCHLD	15	/* child process terminated or stopped */
#define SIGCONT 16	/* continue if stopped */
#define SIGSTOP 17	/* stop */
#define SIGTSTP	18	/* interactive stop */
#define SIGTTIN	19	/* read from ctl tty by member of background */
#define SIGTTOU	20	/* write to ctl tty by member of background */
#define SIGVTALRM 21 /* virtual alarm clock */
#define SIGPROF 22  /* profiling alarm clock */
#define SIGSYS	23	/* bad system call (note "sys: bad sys call") */
#define SIGTRAP	24	/* breakpoint (note "sys: breakpoint"; POSIX XSI) */

#ifdef _BSD_EXTENSION
#define NSIG 25
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void (*signal(int, void (*)()))();
extern int raise(int);

#ifdef __cplusplus
}
#endif

#ifdef _POSIX_SOURCE

typedef long sigset_t;
/* 9front-ng: POSIX's, here for every compiler (was gcc's only) */
typedef struct __siginfo {
	int	si_signo;
	int	si_code;
	int	si_pid;
	int	si_uid;
	void	*si_addr;
	int	si_status;
} siginfo_t;
#define SA_ONSTACK	0x08000000
#define SA_RESETHAND	0x80000000
#define SA_NODEFER	0x40000000
#define SA_SIGINFO	0x00000004

struct sigaction {
	void		(*sa_handler)();
	sigset_t	sa_mask;
	int		sa_flags;
	/* 9front-ng: with SA_SIGINFO (plan9/signal.c: si_signo, si_addr from the note) */
	void		(*sa_sigaction)(int, siginfo_t *, void *);
};
/* values for sa_flags */
#define SA_NOCLDSTOP	1

/* first argument to sigprocmask */
#define SIG_BLOCK	1
#define SIG_UNBLOCK	2
#define SIG_SETMASK	3

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __TYPES_H
extern int kill(pid_t, int);
#endif
extern int sigemptyset(sigset_t *);
extern int sigfillset(sigset_t *);
extern int sigaddset(sigset_t *, int);
extern int sigdelset(sigset_t *, int);
extern int sigismember(const sigset_t *, int);
extern int sigaction(int, const struct sigaction *, struct sigaction *);
extern int sigprocmask(int, sigset_t *, sigset_t *);
extern int sigpending(sigset_t *);
extern int sigsuspend(const sigset_t *);

#ifdef __cplusplus
}
#endif

#endif /* _POSIX_SOURCE */

#if defined(__GNUC__) || defined(_APE_GNU)
#ifdef __APE_POSIX_SIGNAL
#undef signal
#undef sigaction
#undef SIG_DFL
#undef SIG_ERR
#undef SIG_IGN
#define SIG_DFL	((void (*)(int))0)
#define SIG_ERR	((void (*)(int))-1)
#define SIG_IGN	((void (*)(int))1)
#ifdef __cplusplus
extern "C" {
#endif
extern void	(*signal(int, void (*)(int)))(int);
#ifdef _POSIX_SOURCE
struct sigaction {
	void		(*sa_handler)(int);
	sigset_t	sa_mask;
	int		sa_flags;
	void		(*sa_sigaction)(int, siginfo_t *, void *);
};
/* the same layout as APE's (a negative array size otherwise) */
typedef char __ape_sigaction_layout[
	sizeof(struct sigaction) == sizeof(struct __ape_sigaction) ? 1 : -1];
extern int	sigaction(int, const struct sigaction *, struct sigaction *);
#endif
#ifdef __cplusplus
}
#endif
#endif
/* POSIX's pthread_sigmask (ap/gnu/pthread.c): the proc's mask, APE's
 * sigprocmask. siginfo_t and the flags of POSIX's sigaction (LLVM's names);
 * 9front-ng: a handler with SA_SIGINFO gets si_signo and, for a fault,
 * si_addr (plan9/signal.c), its third argument is the Ureg. */
#ifdef __cplusplus
extern "C" {
#endif
extern int	pthread_sigmask(int, const sigset_t *, sigset_t *);
#ifdef __cplusplus
}
#endif
#endif
#endif /* __SIGNAL_H */
