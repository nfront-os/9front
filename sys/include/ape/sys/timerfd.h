/*
 * 9front-ng: Linux's timerfd for APE (ap/gnu/epoll.c): a pipe's end that
 * becomes readable (8 bytes: the expirations) once its time has come; the
 * time is kept by epoll_wait of an epoll set with it (libwayland's timers).
 */
#ifndef _SYS_TIMERFD_H
#define _SYS_TIMERFD_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TFD_CLOEXEC	02000000
#define TFD_NONBLOCK	04000
#define TFD_TIMER_ABSTIME	1

#ifndef _STRUCT_ITIMERSPEC
#define _STRUCT_ITIMERSPEC
struct itimerspec {
	struct timespec	it_interval;
	struct timespec	it_value;
};
#endif

extern int timerfd_create(int, int);
extern int timerfd_settime(int, int, const struct itimerspec *, struct itimerspec *);
extern int timerfd_gettime(int, struct itimerspec *);

#ifdef __cplusplus
}
#endif

#endif
