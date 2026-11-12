/* Public domain. */

#ifndef _LINUX_DELAY_H
#define _LINUX_DELAY_H

#include <sys/param.h>
#include <sys/systm.h>

static inline void
udelay(unsigned long usecs)
{
	DELAY(usecs);
}

static inline void
ndelay(unsigned long nsecs)
{
	DELAY(MAX(nsecs / 1000, 1));
}

/* 9front: sleeps of 100 us and more sleep, shorter ones spin */
static inline void
usleep_range(unsigned long min, unsigned long max)
{
	unsigned long us = (min + max) / 2;

	if (us >= 100)
		tsleep_nsec(&nowake, PWAIT, "usleep", USEC_TO_NSEC(us));
	else
		DELAY(us);
}

/* XXX assumes state is TASK_UNINTERRUPTIBLE */
static inline void
usleep_range_state(unsigned long min, unsigned long max, unsigned int state)
{
	usleep_range(min, max);
}

static inline void
mdelay(unsigned long msecs)
{
	int loops = msecs;
	while (loops--)
		DELAY(1000);
}

#define drm_msleep(x)		mdelay(x)

/* 9front: Linux's msleep (OpenBSD changes the Linux code to drm_msleep), a sleep */
static inline void
msleep(unsigned int msecs)
{
	tsleep_nsec(&nowake, PWAIT, "msleep", MSEC_TO_NSEC(msecs));
}

static inline void
fsleep(unsigned long usecs)
{
	DELAY(usecs);
}

static inline unsigned int
msleep_interruptible(unsigned int msecs)
{
	int r = tsleep_nsec(&nowake, PWAIT|PCATCH, "msleepi",
	    MSEC_TO_NSEC(msecs));
	if (r == EINTR)
		return 1;
	return 0;
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#define usleep_range_state(a, b, s)	usleep_range(a, b)
static inline void ssleep(unsigned int s) { msleep(s * 1000); }
/* 9front: from OpenBSD 6.18 */
/* INT_MAX / 1000, delay() has an int microseconds arg */
#define MAX_UDELAY_MS 2147483

#endif
