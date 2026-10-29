#ifndef __RESOURCE_H__
#define __RESOURCE_H__

/* GCC port: a program that includes it wants it: _BSD_EXTENSION for
 * this header (bsd.h's prototypes stay off) */
#ifndef _BSD_EXTENSION
#define _BSD_EXTENSION
#define __GCC_BSD_SYS_RESOURCE_H
#endif

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
#endif /* !__RESOURCE_H__ */
