/* Public domain. */

#ifndef _LINUX_LOG2_H
#define _LINUX_LOG2_H

#include <sys/types.h>
#include <sys/systm.h>

/* 9front: a constant for a constant (the Linux code has ilog2 in initialisers) */
#define ilog2(x)	(__builtin_constant_p(x) ?			\
	((x) < 1 ? -1 : 63 - __builtin_clzll((unsigned long long)(x))) :	\
	((sizeof(x) <= 4) ? (fls(x) - 1) : (flsl(x) - 1)))

int	drm_order(unsigned long);

#define is_power_of_2(x)	(((x) != 0) && (((x) - 1) & (x)) == 0)
/* 9front: constant for a constant (bitfield widths: i915's dp link caps) */
#define order_base_2(x)		(__builtin_constant_p(x) ? ((x) <= 1 ? 0 : ilog2((x) - 1) + 1) : drm_order(x))

static inline unsigned long
roundup_pow_of_two(unsigned long x)
{
	return (1UL << flsl(x - 1));
}

static inline unsigned long
rounddown_pow_of_two(unsigned long x)
{
	return (1UL << (flsl(x) - 1));
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/types.h>
static inline unsigned long __roundup_pow_of_two(unsigned long n) { return roundup_pow_of_two(n); }
static inline unsigned long __rounddown_pow_of_two(unsigned long n) { return 1UL << ilog2(n); }

#endif
