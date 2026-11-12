/* Public domain. */

#ifndef _LINUX_MATH64_H
#define _LINUX_MATH64_H

#include <sys/types.h>
#include <asm/div64.h>

static inline uint64_t
div_u64(uint64_t x, uint32_t y)
{
	return (x / y);
}

static inline int64_t
div_s64(int64_t x, int64_t y)
{
	return (x / y);
}

static inline uint64_t
div64_u64(uint64_t x, uint64_t y)
{
	return (x / y);
}

static inline uint64_t
div64_u64_rem(uint64_t x, uint64_t y, uint64_t *rem)
{
	*rem = x % y;
	return (x / y);
}

static inline uint64_t
div_u64_rem(uint64_t x, uint32_t y, uint32_t *rem)
{
	*rem = x % y;
	return (x / y);
}

static inline int64_t
div64_s64(int64_t x, int64_t y)
{
	return (x / y);
}

static inline uint64_t
mul_u32_u32(uint32_t x, uint32_t y)
{
	return (uint64_t)x * y;
}

static inline uint64_t
mul_u64_u32_div(uint64_t x, uint32_t y, uint32_t div)
{
	return (x * y) / div;
}

static inline uint64_t
DIV_U64_ROUND_UP(uint64_t x, uint32_t y)
{
	return div_u64(x + y - 1, y);
}

static inline uint64_t
DIV64_U64_ROUND_UP(uint64_t x, uint64_t y)
{
	return div64_u64(x + y - 1, y);
}

static inline uint64_t
DIV64_U64_ROUND_CLOSEST(uint64_t x, uint64_t y)
{
	return div64_u64(x + y / 2, y);
}

static inline uint64_t
mul_u64_u32_shr(uint64_t x, uint32_t y, unsigned int shift)
{
	uint32_t hi, lo;
	hi = x >> 32;
	lo = x & 0xffffffff;

	return (mul_u32_u32(lo, y) >> shift) +
	    (mul_u32_u32(hi, y) << (32 - shift));
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/types.h>
#include <linux/math.h>
static inline s64 div_s64_rem(s64 n, s32 d, s32 *rem) { *rem = n % d; return n / d; }
static inline u64 div64_ul(u64 n, unsigned long d) { return n / d; }
static inline u64 mul_u64_u64_shr(u64 a, u64 m, unsigned int s) { return (u64)(((unsigned __int128)a * m) >> s); }
static inline u64 mul_u64_u64_div_u64(u64 a, u64 b, u64 c) { return (u64)(((unsigned __int128)a * b) / c); }
#define DIV64_U64_ROUND_CLOSEST(n, d)	({ u64 __d = (d); div64_u64((n) + (__d / 2), __d); })
#define DIV_U64_ROUND_CLOSEST(n, d)	({ u32 __d = (d); div_u64((n) + (__d / 2), __d); })

#endif
