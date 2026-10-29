#ifndef __TIME_H
#define __TIME_H
#pragma lib "/$M/lib/ape/libap.a"

#include <stddef.h>

#define CLOCKS_PER_SEC 1000

/* obsolsecent, but required */
#define CLK_TCK CLOCKS_PER_SEC

#ifndef _CLOCK_T
#define _CLOCK_T
typedef long clock_t;
#endif
#ifndef _TIME_T
#define _TIME_T
typedef long time_t;
#endif

struct tm {
	int	tm_sec;
	int	tm_min;
	int	tm_hour;
	int	tm_mday;
	int	tm_mon;
	int	tm_year;
	int	tm_wday;
	int	tm_yday;
	int	tm_isdst;
};

#ifdef __cplusplus
extern "C" {
#endif

extern clock_t clock(void);
extern double difftime(time_t, time_t);
extern time_t mktime(struct tm *);
extern time_t time(time_t *);
extern char *asctime(const struct tm *);
extern char *ctime(const time_t *);
extern struct tm *gmtime(const time_t *);
extern struct tm *localtime(const time_t *);
extern size_t strftime(char *, size_t, const char *, const struct tm *);

#if defined(_REENTRANT_SOURCE) || defined(_POSIX_SOURCE)	/* POSIX's thread-safe functions */
extern struct tm *gmtime_r(const time_t *, struct tm *);
extern struct tm *localtime_r(const time_t *, struct tm *);
extern char *ctime_r(const time_t *, char *);
#endif

#ifdef _POSIX_SOURCE
extern void tzset(void);

struct timespec {
	time_t tv_sec;
	long tv_nsec;
};
extern int nanosleep(const struct timespec *req, struct timespec *rem);
#endif

#ifdef __cplusplus
}
#endif

#ifdef _POSIX_SOURCE
extern char *tzname[2];
extern long timezone;
extern long altzone;
extern int daylight;
#endif

#if defined(__GNUC__) || defined(_APE_GNU)
/* GCC port: POSIX's clocks and C11's timespec_get (ap/gnu/c99time.c:
 * /dev/bintime). APE has struct timespec and nanosleep (_POSIX_SOURCE). */
#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif
#define CLOCK_REALTIME			0
#define CLOCK_MONOTONIC			1
#define CLOCK_PROCESS_CPUTIME_ID	2
#define CLOCK_THREAD_CPUTIME_ID		3
#define TIME_UTC	1
extern int	clock_gettime(clockid_t, struct timespec *);
extern int	clock_getres(clockid_t, struct timespec *);
extern int	timespec_get(struct timespec *, int);
#ifdef __cplusplus
}
#endif
#endif
#endif /* __TIME_H */
