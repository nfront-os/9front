/*	$OpenBSD: bitops.h,v 1.7 2025/03/06 02:50:20 jsg Exp $	*/
/*
 * Copyright (c) 2013, 2014, 2015 Mark Kettenis
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _LINUX_BITOPS_H
#define _LINUX_BITOPS_H

#include <sys/types.h>
#include <sys/param.h>
#include <lib/libkern/libkern.h>

#include <asm/bitsperlong.h>
#include <linux/atomic.h>

#define BIT(x)		(1UL << (x))
#define BIT_ULL(x)	(1ULL << (x))
#define BIT_MASK(x)	(1UL << ((x) % BITS_PER_LONG))
#define BITS_PER_BYTE	8

#define GENMASK(h, l)		(((~0UL) >> (BITS_PER_LONG - (h) - 1)) & ((~0UL) << (l)))
#define GENMASK_ULL(h, l)	(((~0ULL) >> (BITS_PER_LONG_LONG - (h) - 1)) & ((~0ULL) << (l)))

#define BITS_PER_TYPE(x)	(8 * sizeof(x))
#define BITS_TO_LONGS(x)	howmany((x), 8 * sizeof(long))

/* despite the name these are really ctz */
#define __ffs(x)		__builtin_ctzl(x)
#define __ffs64(x)		__builtin_ctzll(x)
#define ffz(x)			__ffs(~(x))

static inline unsigned int
hweight8(unsigned int x)
{
	x = (x & 0x55) + ((x & 0xaa) >> 1);
	x = (x & 0x33) + ((x & 0xcc) >> 2);
	x = (x + (x >> 4)) & 0x0f;
	return (x);
}

static inline unsigned int
hweight16(unsigned int x)
{
	x = (x & 0x5555) + ((x & 0xaaaa) >> 1);
	x = (x & 0x3333) + ((x & 0xcccc) >> 2);
	x = (x + (x >> 4)) & 0x0f0f;
	x = (x + (x >> 8)) & 0x00ff;
	return (x);
}

static inline unsigned int
hweight32(unsigned int x)
{
	x = (x & 0x55555555) + ((x & 0xaaaaaaaa) >> 1);
	x = (x & 0x33333333) + ((x & 0xcccccccc) >> 2);
	x = (x + (x >> 4)) & 0x0f0f0f0f;
	x = (x + (x >> 8));
	x = (x + (x >> 16)) & 0x000000ff;
	return x;
}

static inline unsigned long
hweight64(uint64_t x)
{
	x = (x & 0x5555555555555555ULL) + ((x & 0xaaaaaaaaaaaaaaaaULL) >> 1);
	x = (x & 0x3333333333333333ULL) + ((x & 0xccccccccccccccccULL) >> 2);
	x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
	x = (x + (x >> 8));
	x = (x + (x >> 16));
	x = (x + (x >> 32)) & 0x000000ff;
	return x;
}

static inline unsigned long
hweight_long(unsigned long x)
{
#ifdef __LP64__
	return hweight64(x);
#else
	return hweight32(x);
#endif
}

static inline int64_t
sign_extend64(uint64_t value, int index)
{
	uint8_t shift = 63 - index;
	return (int64_t)(value << shift) >> shift;
}

static inline int
fls64(long long mask)
{
	int bit;

	if (mask == 0)
		return (0);
	for (bit = 1; mask != 1; bit++)
		mask = (unsigned long long)mask >> 1;
	return (bit);
}

static inline int
__fls(long mask)
{
	return (flsl(mask) - 1);
}

static inline uint32_t
ror32(uint32_t word, unsigned int shift)
{
	return (word >> shift) | (word << (32 - shift));
}

static inline void
__assign_bit(u_int b, volatile void *p, bool set)
{
	if (set)
		__set_bit(b, p);
	else
		__clear_bit(b, p);
}
/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/bits.h>
#include <linux/types.h>
#include <linux/compiler.h>
#include <linux/bitmap.h>
#define BITS_TO_U64(n)		DIV_ROUND_UP(n, BITS_PER_TYPE(u64))
#define BITS_TO_U32(n)		DIV_ROUND_UP(n, BITS_PER_TYPE(u32))
#define BITS_TO_BYTES(n)	DIV_ROUND_UP(n, BITS_PER_TYPE(char))
static inline int fls_long(unsigned long x) { return fls64(x); }
static inline u32 rol32(u32 w, unsigned int s) { return (w << (s & 31)) | (w >> ((-s) & 31)); }
static inline u64 rol64(u64 w, unsigned int s) { return (w << (s & 63)) | (w >> ((-s) & 63)); }
static inline int get_bitmask_order(unsigned int c) { return fls(c); }
static inline int get_count_order(unsigned int c) { return c == 0 ? -1 : fls(c - 1); }
static inline s32 sign_extend32(u32 v, int i) { u8 s = 31 - i; return (s32)(v << s) >> s; }
#define __kpi_bw(nr, a)		(((volatile unsigned long *)(a)) + (nr) / BITS_PER_LONG)
#define __kpi_bm(nr)		(1UL << ((nr) % BITS_PER_LONG))
static inline void change_bit(long nr, volatile unsigned long *a) { __atomic_fetch_xor(__kpi_bw(nr, a), __kpi_bm(nr), __ATOMIC_SEQ_CST); }
static inline bool test_and_change_bit(long nr, volatile unsigned long *a) { return (__atomic_fetch_xor(__kpi_bw(nr, a), __kpi_bm(nr), __ATOMIC_SEQ_CST) & __kpi_bm(nr)) != 0; }
#define test_bit_acquire(nr, a)	test_bit(nr, a)
#define test_and_set_bit_lock(nr, a)	test_and_set_bit(nr, a)
#define set_bit_lock(nr, a)	set_bit(nr, a)
unsigned long	_find_first_bit(const unsigned long *addr, unsigned long size);
unsigned long	_find_next_bit(const unsigned long *addr, unsigned long size, unsigned long off);
unsigned long	_find_first_zero_bit(const unsigned long *addr, unsigned long size);
unsigned long	_find_next_zero_bit(const unsigned long *addr, unsigned long size, unsigned long off);
unsigned long	_find_last_bit(const unsigned long *addr, unsigned long size);
#define find_last_bit(a, s)		_find_last_bit(a, s)
#define for_each_set_bit_from(bit, addr, size) \
	for ((bit) = find_next_bit((addr), (size), (bit)); (bit) < (size); (bit) = find_next_bit((addr), (size), (bit) + 1))

#endif
