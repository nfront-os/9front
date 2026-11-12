/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/ratelimit.h (ours, not Linux's) */
#ifndef _KPI_LINUX_RATELIMIT_H
#define _KPI_LINUX_RATELIMIT_H

#include <linux/types.h>
#include <sys/mutex.h>	/* not spinlock_types.h: kernel.h includes this */

struct ratelimit_state { struct spinlock lock;	/* a raw_spinlock_t */
	 int interval; int burst; int printed; int missed; unsigned long begin; };
#define DEFAULT_RATELIMIT_INTERVAL	(5 * 1000)	/* HZ: hz is a variable here */
#define DEFAULT_RATELIMIT_BURST		10
#define RATELIMIT_STATE_INIT(n, i, b)	{ .lock = MUTEX_INITIALIZER(IPL_NONE), .interval = (i), .burst = (b) }
#define DEFINE_RATELIMIT_STATE(n, i, b)	struct ratelimit_state n = RATELIMIT_STATE_INIT(n, i, b)
#define ratelimit_state_init(r, i, b)	do { mtx_init(&(r)->lock, IPL_NONE); (r)->interval = (i); (r)->burst = (b); (r)->printed = (r)->missed = 0; (r)->begin = 0; } while (0)
#define ratelimit_set_flags(r, f)	do { } while (0)
int	__ratelimit(struct ratelimit_state *r);	/* kpi/linux/lib.c */
#define RATELIMIT_MSG_ON_RELEASE	1

/* 9front: how many messages were held back since the last time (and from now 0) */
static inline int
ratelimit_state_get_miss(struct ratelimit_state *rs)
{
	int m = rs->missed;

	rs->missed = 0;
	return m;
}
static inline void
ratelimit_state_reset_interval(struct ratelimit_state *rs, int interval)
{
	rs->interval = interval;
	rs->printed = rs->missed = 0;
	rs->begin = 0;
}

#endif
