/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: linux/int_log.h (ours: Linux's is GPL).  log2 and
 * log10 of a value in 8.24 fixed point: the integer part from ilog2, the
 * fraction bit by bit from squaring the value scaled into [1, 2); log10
 * is log2 times log10(2) (0.30103, as 2^31ths).
 */
#ifndef _KPI_LINUX_INT_LOG_H
#define _KPI_LINUX_INT_LOG_H

#include <linux/types.h>
#include <linux/log2.h>

static inline unsigned int
intlog2(u32 value)
{
	unsigned int r, i;
	u64 x;

	if (value == 0)
		return 0;
	r = ilog2(value) << 24;
	x = ((u64)value << 31) >> ilog2(value);	/* 1.f as 1.31 */
	for (i = 0; i < 24; i++) {
		x = (x * x) >> 31;
		if (x >= (2ULL << 31)) {
			x >>= 1;
			r |= 1U << (23 - i);
		}
	}
	return r;
}

static inline unsigned int
intlog10(u32 value)
{
	return (unsigned int)(((u64)intlog2(value) * 646456993ULL) >> 31);
}

#endif
