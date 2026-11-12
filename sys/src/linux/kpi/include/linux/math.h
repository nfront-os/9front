/* Public domain. */

#ifndef _LINUX_MATH_H
#define _LINUX_MATH_H

/* 9front: as Linux's, without the overflow of x * n: (x / d) * n + (x % d) * n / d */
#define mult_frac(x, n, d) ({					\
	__typeof__(x) __mf_q = (x) / (d);			\
	__typeof__(x) __mf_r = (x) % (d);			\
	__mf_q * (n) + (__mf_r * (n)) / (d);			\
})

/* 9front: y a power of two, by masks as Linux's (no division: i915 rounds to an alignment of 0) */
#define __round_mask(x, y)	((__typeof__(x))((y) - 1))
#define round_up(x, y)		((((x) - 1) | __round_mask(x, y)) + 1)
#define round_down(x, y)	((x) & ~__round_mask(x, y))
#define rounddown(x, y) (((x) / (y)) * (y)) /* arbitrary y */
#define DIV_ROUND_UP(x, y)	(((x) + ((y) - 1)) / (y))
#define DIV_ROUND_UP_ULL(x, y)	DIV_ROUND_UP(x, y)
#define DIV_ROUND_DOWN(x, y)	((x) / (y))
#define DIV_ROUND_DOWN_ULL(x, y)	DIV_ROUND_DOWN(x, y)
/* 9front: as Linux's, halves away from zero for signed values too */
#define DIV_ROUND_CLOSEST(x, y) ({				\
	__typeof__(x) __dr_x = (x);				\
	__typeof__(y) __dr_y = (y);				\
	(((__typeof__(x))-1) > 0 || ((__typeof__(y))-1) > 0 ||	\
	    ((__dr_x) > 0) == ((__dr_y) > 0)) ?			\
	    (((__dr_x) + ((__dr_y) / 2)) / (__dr_y)) :		\
	    (((__dr_x) - ((__dr_y) / 2)) / (__dr_y));		\
})
#define DIV_ROUND_CLOSEST_ULL(x, y)	DIV_ROUND_CLOSEST(x, y)

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/types.h>
unsigned long	int_sqrt(unsigned long);
#define reciprocal_scale(val, ep_ro)	((u32)(((u64)(val) * (ep_ro)) >> 32))
static inline u32 int_sqrt64(u64 x) { return int_sqrt((unsigned long)x); }

/* 9front: Linux's constant form of DIV_ROUND_CLOSEST, and in_range for 32 bits */
#define __KERNEL_DIV_ROUND_CLOSEST(x, d)	(((x) + ((d) / 2)) / (d))
#define in_range32(val, start, len)	((u32)((val) - (start)) < (u32)(len))
#define in_range64(val, start, len)	((u64)((val) - (start)) < (u64)(len))

#define in_range(val, start, len)	(sizeof((start) + (len) + (val)) <= 4 ? in_range32(val, start, len) : in_range64(val, start, len))

/* base ** exp (Linux's int_pow) */
static inline u64
int_pow(u64 base, unsigned int exp)
{
	u64 r = 1;

	while (exp) {
		if (exp & 1)
			r *= base;
		exp >>= 1;
		base *= base;
	}
	return r;
}

#endif
