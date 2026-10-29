#ifndef __RESOURCE_H__
#define __RESOURCE_H__

/* GCC port: a program that includes it wants it: _BSD_EXTENSION for
 * this header (bsd.h's prototypes stay off) */
#ifndef _BSD_EXTENSION
#define _BSD_EXTENSION
#define __GCC_BSD_SYS_RESOURCE_H
#endif
/* GCC port: struct rusage needs struct timeval */
#include <sys/time.h>

struct rusage {
	struct timeval ru_utime;	/* user time used */
	struct timeval ru_stime;	/* system time used */
	long	ru_maxrss;		/* max resident set size */
#define	ru_first	ru_ixrss
	long	ru_ixrss;		/* integral shared memory size */
	long	ru_idrss;		/* integral unshared data " */
	long	ru_isrss;		/* integral unshared stack " */
	long	ru_minflt;		/* page reclaims */
	long	ru_majflt;		/* page faults */
	long	ru_nswap;		/* swaps */
	long	ru_inblock;		/* block input operations */
	long	ru_oublock;		/* block output operations */
	long	ru_msgsnd;		/* messages sent */
	long	ru_msgrcv;		/* messages received */
	long	ru_nsignals;		/* signals received */
	long	ru_nvcsw;		/* voluntary context switches */
	long	ru_nivcsw;		/* involuntary " */
#define	ru_last		ru_nivcsw
};

#ifdef __GCC_BSD_SYS_RESOURCE_H
#undef _BSD_EXTENSION
#undef __GCC_BSD_SYS_RESOURCE_H
#endif
#if defined(__GNUC__) || defined(_APE_GNU)
/* GCC port: POSIX's resource limits (ap/gnu/posix.c): Plan 9 has none to ask
 * for */
typedef unsigned long long rlim_t;
#define RLIM_INFINITY	(~(rlim_t)0)
#define RLIM_SAVED_MAX	RLIM_INFINITY
#define RLIM_SAVED_CUR	RLIM_INFINITY
struct rlimit {
	rlim_t	rlim_cur;
	rlim_t	rlim_max;
};
#define RLIMIT_CPU	0
#define RLIMIT_FSIZE	1
#define RLIMIT_DATA	2
#define RLIMIT_STACK	3
#define RLIMIT_CORE	4
#define RLIMIT_NOFILE	7
#define RLIMIT_AS	9
#ifdef __cplusplus
extern "C" {
#endif
extern int	getrlimit(int, struct rlimit *);
extern int	setrlimit(int, const struct rlimit *);
#ifdef __cplusplus
}
#endif
#endif
#endif /* !__RESOURCE_H__ */
