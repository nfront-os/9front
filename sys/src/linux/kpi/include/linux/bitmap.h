/*	$OpenBSD: bitmap.h,v 1.12 2026/10/20 11:59:12 jsg Exp $	*/
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

#ifndef _LINUX_BITMAP_H
#define _LINUX_BITMAP_H

#include <linux/align.h>
#include <linux/bitops.h>
#include <linux/string.h>

#define bitmap_empty(p, n)	(find_first_bit(p, n) == n)

static inline void
bitmap_set(void *p, int b, u_int n)
{
	u_int end = b + n;

	for (; b < end; b++)
		__set_bit(b, p);
}

static inline void
bitmap_clear(void *p, int b, u_int n)
{
	u_int end = b + n;

	for (; b < end; b++)
		__clear_bit(b, p);
}

static inline void
bitmap_zero(void *p, u_int n)
{
	u_int *ptr = p;
	u_int b;

	for (b = 0; b < n; b += 32)
		ptr[b >> 5] = 0;
}

static inline void
bitmap_fill(void *p, u_int n)
{
	u_int *ptr = p;
	u_int b;

	for (b = 0; b < n; b += 32)
		ptr[b >> 5] = 0xffffffff;
}

static inline void
bitmap_or(void *d, const void *s1, const void *s2, u_int n)
{
	u_int *dst = d;
	const u_int *src1 = s1;
	const u_int *src2 = s2;
	u_int b;

	for (b = 0; b < n; b += 32)
		dst[b >> 5] = src1[b >> 5] | src2[b >> 5];
}

static inline void
bitmap_andnot(void *d, void *s1, void *s2, u_int n)
{
	u_int *dst = d;
	u_int *src1 = s1;
	u_int *src2 = s2;
	u_int b;

	for (b = 0; b < n; b += 32)
		dst[b >> 5] = src1[b >> 5] & ~src2[b >> 5];
}

static inline void
bitmap_complement(void *d, void *s, u_int n)
{
	u_int *dst = d;
	u_int *src = s;
	u_int b;

	for (b = 0; b < n; b += 32)
		dst[b >> 5] = ~src[b >> 5];
}

static inline bool
bitmap_intersects(const void *s1, const void *s2, u_int n)
{
	const u_int *b1 = s1;
	const u_int *b2 = s2;
	u_int b;

	for (b = 0; b < n; b += 32)
		if (b1[b >> 5] & b2[b >> 5])
			return true;
	if ((n % 32) != 0)
		if ((b1[n >> 5] & b2[b >> 5]) & (0xffffffff >> (32 - (n % 32))))
			return true;

	return false;
}

static inline void
bitmap_copy(void *d, const void *s, u_int n)
{
	u_int *dst = d;
	const u_int *src = s;
	u_int b;

	for (b = 0; b < n; b += 32)
		dst[b >> 5] = src[b >> 5];
}

static inline void
bitmap_to_arr32(void *d, const unsigned long *src, u_int n)
{
	u_int *dst = d;
#ifdef __LP64__
	u_int b;

	for (b = 0; b < n; b += 32) {
		dst[b >> 5] = src[b >> 6] & 0xffffffff;
		b += 32;
		if (b < n)
			dst[b >> 5] = src[b >> 6] >> 32;
	}
#else
	bitmap_copy(d, src, n);
#endif
	if ((n % 32) != 0)
		dst[n >> 5] &= (0xffffffff >> (32 - (n % 32)));
}

static inline void
bitmap_from_arr32(unsigned long *dst, const void *s, u_int n)
{
#ifdef __LP64__
	const u_int *src = s;
	u_int b;

	for (b = 0; b < n; b += 32) {
		dst[b >> 6] = src[b >> 5];
		b += 32;
		if (b < n)
			dst[b >> 6] |= ((unsigned long)src[b >> 5]) << 32;
	}
	if ((n % 64) != 0)
		dst[n >> 6] &= (0xffffffffffffffffUL >> (64 - (n % 64)));
#else
	bitmap_copy(dst, s, n);
	if ((n % 32) != 0)
		dst[n >> 5] &=  (0xffffffff >> (32 - (n % 32)));
#endif
}

static inline int
bitmap_weight(const void *p, u_int n)
{
	const u_int *ptr = p;
	u_int b;
	int sum = 0;

	for (b = 0; b < n; b += 32)
		sum += hweight32(ptr[b >> 5]);
	return sum;
}

static inline int
bitmap_find_free_region(void *p, u_int n, int o)
{
	int b;

	KASSERT(o == 0);
	b = find_first_zero_bit(p, n);
	if (b == n)
		return -ENOMEM;
	__set_bit(b, p);
	return b;
}

static inline void
bitmap_release_region(void *p, u_int b, int o)
{
	KASSERT(o == 0);
	__clear_bit(b, p);
}


void		*bitmap_zalloc(u_int, gfp_t);
void		 bitmap_free(void *);
unsigned long	 bitmap_find_next_zero_area(unsigned long *, unsigned long,
		     unsigned long, unsigned int, unsigned long);

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#define BITMAP_FIRST_WORD_MASK(start)	(~0UL << ((start) & (BITS_PER_LONG - 1)))
#define BITMAP_LAST_WORD_MASK(nbits)	(~0UL >> (-(nbits) & (BITS_PER_LONG - 1)))
bool	bitmap_full(const unsigned long *m, unsigned int n);
bool	bitmap_equal(const unsigned long *a, const unsigned long *b, unsigned int n);
bool	bitmap_subset(const unsigned long *a, const unsigned long *b, unsigned int n);
bool	bitmap_and(unsigned long *d, const unsigned long *a, const unsigned long *b, unsigned int n);
void	bitmap_xor(unsigned long *d, const unsigned long *a, const unsigned long *b, unsigned int n);
void	bitmap_shift_right(unsigned long *d, const unsigned long *s, unsigned int shift, unsigned int n);
void	bitmap_shift_left(unsigned long *d, const unsigned long *s, unsigned int shift, unsigned int n);
unsigned long	*bitmap_alloc(unsigned int n, gfp_t gfp);

/*
 * 9front: an nbits-wide value (at most a long's) at bit start of map,
 * across two longs if it must be (Linux 6.8's bitmap_read/write)
 */
static inline unsigned long
bitmap_read(const unsigned long *map, unsigned long start, unsigned long nbits)
{
	unsigned long i = start / BITS_PER_LONG, off = start % BITS_PER_LONG, v;

	if (nbits == 0 || nbits > BITS_PER_LONG)
		return 0;
	v = map[i] >> off;
	if (off + nbits > BITS_PER_LONG)
		v |= map[i + 1] << (BITS_PER_LONG - off);
	return nbits == BITS_PER_LONG ? v : v & ((1UL << nbits) - 1);
}

static inline void
bitmap_write(unsigned long *map, unsigned long value, unsigned long start, unsigned long nbits)
{
	unsigned long i = start / BITS_PER_LONG, off = start % BITS_PER_LONG, m;

	if (nbits == 0 || nbits > BITS_PER_LONG)
		return;
	m = nbits == BITS_PER_LONG ? ~0UL : (1UL << nbits) - 1;
	value &= m;
	map[i] = (map[i] & ~(m << off)) | (value << off);
	if (off + nbits > BITS_PER_LONG) {
		unsigned long hi = off + nbits - BITS_PER_LONG;

		map[i + 1] = (map[i + 1] & ~((1UL << hi) - 1)) | (value >> (BITS_PER_LONG - off));
	}
}

#endif
