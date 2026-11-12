/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: OpenBSD kernel API, sys/time.h: the clocks (p9_nsec, p9_realnsec) */
#ifndef _OBSD_SYS_TIME_H
#define _OBSD_SYS_TIME_H

#include <sys/types.h>

struct timespec {
	time_t	tv_sec;
	long	tv_nsec;
};

struct timeval {
	time_t	tv_sec;
	long	tv_usec;
};

#define SEC_TO_NSEC(s)		((uint64_t)(s) * 1000000000ULL)
#define MSEC_TO_NSEC(ms)	((uint64_t)(ms) * 1000000ULL)
#define USEC_TO_NSEC(us)	((uint64_t)(us) * 1000ULL)
#define MAXTSLP			UINT64_MAX	/* sleeps longer than this are INFSLP */

static inline uint64_t
TIMESPEC_TO_NSEC(const struct timespec *ts)
{
	return ts->tv_sec * 1000000000ULL + ts->tv_nsec;
}

static inline uint64_t
TIMEVAL_TO_NSEC(const struct timeval *tv)
{
	return tv->tv_sec * 1000000000ULL + tv->tv_usec * 1000ULL;
}

static inline void
NSEC_TO_TIMESPEC(uint64_t ns, struct timespec *ts)
{
	ts->tv_sec = ns / 1000000000ULL;
	ts->tv_nsec = ns % 1000000000ULL;
}

static inline void
NSEC_TO_TIMEVAL(uint64_t ns, struct timeval *tv)
{
	tv->tv_sec = ns / 1000000000ULL;
	tv->tv_usec = (ns % 1000000000ULL) / 1000;
}

#define timespecisset(t)	((t)->tv_sec || (t)->tv_nsec)
#define timespeccmp(a, b, op)	(((a)->tv_sec == (b)->tv_sec) ? \
	((a)->tv_nsec op (b)->tv_nsec) : ((a)->tv_sec op (b)->tv_sec))
#define timespecadd(a, b, r) do {					\
	(r)->tv_sec = (a)->tv_sec + (b)->tv_sec;			\
	(r)->tv_nsec = (a)->tv_nsec + (b)->tv_nsec;			\
	if ((r)->tv_nsec >= 1000000000L) {				\
		(r)->tv_sec++;						\
		(r)->tv_nsec -= 1000000000L;				\
	}								\
} while (0)
#define timespecsub(a, b, r) do {					\
	(r)->tv_sec = (a)->tv_sec - (b)->tv_sec;			\
	(r)->tv_nsec = (a)->tv_nsec - (b)->tv_nsec;			\
	if ((r)->tv_nsec < 0) {						\
		(r)->tv_sec--;						\
		(r)->tv_nsec += 1000000000L;				\
	}								\
} while (0)

/* uptime: the monotonic clock; time: the wall clock */
uint64_t	nsecuptime(void);
uint64_t	getnsecuptime(void);
void	nanouptime(struct timespec *);
void	getnanouptime(struct timespec *);
void	microuptime(struct timeval *);
void	nanotime(struct timespec *);
void	getnanotime(struct timespec *);
void	microtime(struct timeval *);
time_t	gettime(void);
time_t	getuptime(void);

#define USEC_TO_TIMEVAL(us, tv) do {				\
	(tv)->tv_sec = (us) / 1000000;				\
	(tv)->tv_usec = (us) % 1000000;				\
} while (0)
#define timerisset(tv)		((tv)->tv_sec || (tv)->tv_usec)
#define timerclear(tv)		((tv)->tv_sec = (tv)->tv_usec = 0)
#define timercmp(a, b, op)	(((a)->tv_sec == (b)->tv_sec) ? \
	((a)->tv_usec op (b)->tv_usec) : ((a)->tv_sec op (b)->tv_sec))
#define timeradd(a, b, r) do {					\
	(r)->tv_sec = (a)->tv_sec + (b)->tv_sec;		\
	(r)->tv_usec = (a)->tv_usec + (b)->tv_usec;		\
	if ((r)->tv_usec >= 1000000) {				\
		(r)->tv_sec++;					\
		(r)->tv_usec -= 1000000;			\
	}							\
} while (0)
#define timersub(a, b, r) do {					\
	(r)->tv_sec = (a)->tv_sec - (b)->tv_sec;		\
	(r)->tv_usec = (a)->tv_usec - (b)->tv_usec;		\
	if ((r)->tv_usec < 0) {					\
		(r)->tv_sec--;					\
		(r)->tv_usec += 1000000;			\
	}							\
} while (0)

#endif
