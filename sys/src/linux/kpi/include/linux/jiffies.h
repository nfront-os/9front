/* Public domain. */

#ifndef _LINUX_JIFFIES_H
#define _LINUX_JIFFIES_H

#include <sys/types.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/limits.h>
#include <sys/kernel.h>

/* 9front: jiffies from the clock (sys/systm.h): there is no tick to count them */
#define jiffies		kpi_jiffies()
/* 9front: from OpenBSD 6.18 */
#include <linux/cache.h>
#define jiffies_64 jiffies /* XXX */
#undef HZ
#define HZ	1000	/* 9front: hz (kpi/obsd/sleep.c), as a constant: static initializers (ratelimits) use it */

#define MAX_JIFFY_OFFSET	((INT_MAX >> 1) - 1)

#define time_in_range(x, min, max) ((x) >= (min) && (x) <= (max))

static inline unsigned int
jiffies_to_msecs(const unsigned long x)
{
	return (((uint64_t)(x)) * 1000 / hz);
}

static inline unsigned int
jiffies_to_usecs(const unsigned long x)
{
	return (((uint64_t)(x)) * 1000000 / hz);
}

static inline uint64_t
jiffies_to_nsecs(const unsigned long x)
{
	return (((uint64_t)(x)) * 1000000000 / hz);
}

#define secs_to_jiffies(x)	(((uint64_t)(x)) * hz)
#define msecs_to_jiffies(x)	(((uint64_t)(x)) * hz / 1000)
#define usecs_to_jiffies(x)	(((uint64_t)(x)) * hz / 1000000)
#define nsecs_to_jiffies(x)	(((uint64_t)(x)) * hz / 1000000000)
#define nsecs_to_jiffies64(x)	(((uint64_t)(x)) * hz / 1000000000)

static inline uint64_t
get_jiffies_64(void)
{
	return jiffies;
}

static inline int
time_after(const unsigned long a, const unsigned long b)
{
	return((long)(b - a) < 0);
}
#define time_before(a,b)	time_after(b,a)

static inline int
time_after_eq(const unsigned long a, const unsigned long b)
{
	return((long)(b - a) <= 0);
}

static inline int
time_after_eq64(const unsigned long long a, const unsigned long long b)
{
	return((long long)(b - a) <= 0);
}

#define time_after32(a,b)	((int32_t)((uint32_t)(b) - (uint32_t)(a)) < 0)

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/types.h>
#include <linux/limits.h>
#include <linux/compiler.h>
#define USER_HZ			100
#define PSEC_PER_NSEC		1000L
#define FSEC_PER_SEC		1000000000000000LL
#define INITIAL_JIFFIES		0
#define time_before_eq(a, b)	time_after_eq(b, a)
/* 9front: whether t (in jiffies) is past, still to come, past or now */
#define time_is_before_jiffies(t)	time_before((t), jiffies)
#define time_is_after_jiffies(t)	time_after((t), jiffies)
#define time_is_before_eq_jiffies(t)	time_before_eq((t), jiffies)
#define time_before32(a, b)	time_after32(b, a)
#define time_after64(a, b)	((s64)((b) - (a)) < 0)
#define time_before64(a, b)	time_after64(b, a)
static inline u64 jiffies64_to_nsecs(u64 j) { return j * 1000000; }
#define jiffies_delta_to_msecs(d)	jiffies_to_msecs(d)
#define round_jiffies_relative(j)	(j)
#define round_jiffies(j)		(j)

#endif
