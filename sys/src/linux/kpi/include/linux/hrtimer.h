/* Public domain. */

#ifndef _LINUX_HRTIMER_H
#define _LINUX_HRTIMER_H

#include <sys/types.h>
#include <sys/timeout.h>
#include <linux/rbtree.h>

enum hrtimer_restart { HRTIMER_NORESTART, HRTIMER_RESTART };

/*
 * 9front: Linux's struct hrtimer on a timeout(9) with a nanosecond
 * deadline (OpenBSD changes the Linux code to timeouts); the function
 * runs in the timeout proc, again if it says HRTIMER_RESTART.
 * kpi/obsd/timeout.c.
 */
enum hrtimer_mode {
	HRTIMER_MODE_ABS = 0,
	HRTIMER_MODE_REL = 1,
	HRTIMER_MODE_PINNED = 2,
	HRTIMER_MODE_SOFT = 4,
	HRTIMER_MODE_REL_PINNED = HRTIMER_MODE_REL | HRTIMER_MODE_PINNED,
	HRTIMER_MODE_ABS_SOFT = HRTIMER_MODE_ABS | HRTIMER_MODE_SOFT,
	HRTIMER_MODE_REL_SOFT = HRTIMER_MODE_REL | HRTIMER_MODE_SOFT,
};

struct hrtimer {
	struct timeout	to;
	enum hrtimer_restart	(*function)(struct hrtimer *);
	union {
		int64_t	expires;	/* ktime_t: nanoseconds of the clock */
		struct { int64_t expires; } node;	/* 9front: where Linux 7 has it (timerqueue) */
	};
	int		clock;
};

#define CLOCK_REALTIME		0
#define CLOCK_MONOTONIC		1
#define CLOCK_BOOTTIME		7

void	hrtimer_init(struct hrtimer *, int, enum hrtimer_mode);
void	hrtimer_start_range_ns(struct hrtimer *, int64_t, uint64_t, enum hrtimer_mode);
uint64_t	hrtimer_forward(struct hrtimer *, int64_t, int64_t);
int	hrtimer_cancel(struct hrtimer *);
int	hrtimer_try_to_cancel(struct hrtimer *);

#define hrtimer_start(h, t, m)		hrtimer_start_range_ns(h, t, 0, m)
#define hrtimer_active(x)		timeout_pending(&(x)->to)
#define hrtimer_is_queued(h)		hrtimer_active(h)
#define hrtimer_forward_now(h, i)	hrtimer_forward(h, ktime_get(), i)
#define hrtimer_get_expires(h)		((h)->expires)
#define hrtimer_set_expires(h, t)	((h)->expires = (t))
#define hrtimer_add_expires_ns(h, n)	((h)->expires += (n))
#define hrtimer_get_remaining(h)	((h)->expires - ktime_get())

#include <linux/ktime.h>

/* 9front: Linux 6.15's hrtimer_setup, init and function in one */
static inline void
hrtimer_setup(struct hrtimer *t, enum hrtimer_restart (*fn)(struct hrtimer *), clockid_t clock, enum hrtimer_mode mode)
{
	hrtimer_init(t, clock, mode);
	t->function = fn;
}

#endif
