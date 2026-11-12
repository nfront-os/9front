/* Public domain. */

#ifndef _LINUX_RANDOM_H
#define _LINUX_RANDOM_H

#include <sys/types.h>
#include <sys/systm.h>
#include <linux/prandom.h>	/* 9front: as Linux's random.h */

static inline uint32_t
get_random_u32(void)
{
	return arc4random();
}

static inline uint32_t
get_random_u32_below(uint32_t x)
{
	return arc4random_uniform(x);
}

static inline uint64_t
get_random_u64(void)
{
	uint64_t r;
	arc4random_buf(&r, sizeof(r));
	return r;
}

static inline unsigned long
get_random_long(void)
{
#ifdef __LP64__
	return get_random_u64();
#else
	return get_random_u32();
#endif
}

static inline void
get_random_bytes(void *buf, size_t nbytes)
{
	arc4random_buf(buf, nbytes);
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
static inline u32 get_random_u32_inclusive(u32 lo, u32 hi) { return lo + get_random_u32_below(hi - lo + 1); }

#endif
