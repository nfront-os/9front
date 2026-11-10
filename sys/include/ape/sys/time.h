#ifndef __SYSTIME_H
#define __SYSTIME_H
#pragma lib "/$M/lib/ape/libap.a"

#ifndef __TIMEVAL__
#define __TIMEVAL__
struct timeval {
	long	tv_sec;
	long	tv_usec;
};

struct itimerval {
	struct timeval it_interval;
	struct timeval it_value;
};

#ifdef _BSD_EXTENSION
struct timezone {
	int	tz_minuteswest;
	int	tz_dsttime;
};
#endif
#endif /* __TIMEVAL__ */

#define ITIMER_REAL 0
#define ITIMER_VIRTUAL 1
#define ITIMER_PROF 3

#if defined(__GNUC__) || defined(_APE_GNU)
struct timezone;	/* GCC port: gettimeofday's, also without _BSD_EXTENSION */
#endif
extern int gettimeofday(struct timeval *, struct timezone *);
int getitimer(int, struct itimerval *);
int setitimer(int, const struct itimerval *, struct itimerval *);

#if defined(__GNUC__) || defined(_APE_GNU)
/* GCC port: utimes (ap/gnu/posix.c: seconds, stat(5) has no more) */
#ifdef __cplusplus
extern "C" {
#endif
extern int	utimes(const char *, const struct timeval *);
#ifdef __cplusplus
}
#endif

/* GCC port: the BSD macros on timevals (glibc's and the BSDs' sys/time.h) */
#define timerisset(t)	((t)->tv_sec || (t)->tv_usec)
#define timerclear(t)	((t)->tv_sec = (t)->tv_usec = 0)
#define timercmp(a, b, op)	((a)->tv_sec == (b)->tv_sec ? (a)->tv_usec op (b)->tv_usec : (a)->tv_sec op (b)->tv_sec)
#define timeradd(a, b, r)	do{ (r)->tv_sec = (a)->tv_sec + (b)->tv_sec; (r)->tv_usec = (a)->tv_usec + (b)->tv_usec; \
	if((r)->tv_usec >= 1000000){ (r)->tv_sec++; (r)->tv_usec -= 1000000; } }while(0)
#define timersub(a, b, r)	do{ (r)->tv_sec = (a)->tv_sec - (b)->tv_sec; (r)->tv_usec = (a)->tv_usec - (b)->tv_usec; \
	if((r)->tv_usec < 0){ (r)->tv_sec--; (r)->tv_usec += 1000000; } }while(0)
#endif
#endif /* __SYSTIME_H */
