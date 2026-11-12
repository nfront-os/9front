/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, timeout(9): functions run
 * at a time, by the timeout proc (kpi/obsd/timeout.c), in its order of
 * deadlines.  Times are in ticks (hz) or nanoseconds of uptime.
 */
#ifndef _OBSD_SYS_TIMEOUT_H
#define _OBSD_SYS_TIMEOUT_H

#include <sys/types.h>
#include <sys/queue.h>

struct timeout {
	TAILQ_ENTRY(timeout) to_list;
	void		(*to_func)(void *);
	void		*to_arg;
	uint64_t	to_time;	/* nanoseconds of uptime */
	int		to_flags;
	int		to_kclock;
};

#define TIMEOUT_PROC		0x01
#define TIMEOUT_ONQUEUE		0x02
#define TIMEOUT_INITIALIZED	0x04
#define TIMEOUT_TRIGGERED	0x08
#define TIMEOUT_MPSAFE		0x10

#define KCLOCK_NONE	(-1)
#define KCLOCK_UPTIME	0

#define TIMEOUT_INITIALIZER_FLAGS(fn, arg, kclock, flags) \
	{ { NULL, NULL }, (fn), (arg), 0, (flags) | TIMEOUT_INITIALIZED, (kclock) }
#define TIMEOUT_INITIALIZER(fn, arg)	TIMEOUT_INITIALIZER_FLAGS(fn, arg, KCLOCK_NONE, 0)

void	timeout_set(struct timeout *, void (*)(void *), void *);
void	timeout_set_flags(struct timeout *, void (*)(void *), void *, int, int);
void	timeout_set_proc(struct timeout *, void (*)(void *), void *);
int	timeout_add(struct timeout *, int);
int	timeout_add_tv(struct timeout *, const struct timeval *);
int	timeout_add_sec(struct timeout *, int);
int	timeout_add_msec(struct timeout *, uint64_t);
int	timeout_add_usec(struct timeout *, uint64_t);
int	timeout_add_nsec(struct timeout *, uint64_t);
int	timeout_abs_ts(struct timeout *, const struct timespec *);
int	timeout_del(struct timeout *);
int	timeout_del_barrier(struct timeout *);
void	timeout_barrier(struct timeout *);

#define timeout_pending(to)	((to)->to_flags & TIMEOUT_ONQUEUE)
#define timeout_initialized(to)	((to)->to_flags & TIMEOUT_INITIALIZED)
#define timeout_triggered(to)	((to)->to_flags & TIMEOUT_TRIGGERED)

#endif
