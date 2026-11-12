/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/bits.h (ours, not Linux's) */
#ifndef _KPI_LINUX_BITS_H
#define _KPI_LINUX_BITS_H

#include <linux/const.h>

#define BITS_PER_LONG		64
#define BITS_PER_LONG_LONG	64
#define BITS_PER_BYTE		8
#define BIT(nr)			(1UL << (nr))
#define BIT_ULL(nr)		(1ULL << (nr))
#define BIT_MASK(nr)		(1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)	(1ULL << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)	((nr) / BITS_PER_LONG_LONG)
#define GENMASK(h, l)		(((~0UL) - (1UL << (l)) + 1) & (~0UL >> (BITS_PER_LONG - 1 - (h))))
#define GENMASK_ULL(h, l)	(((~0ULL) - (1ULL << (l)) + 1) & (~0ULL >> (BITS_PER_LONG_LONG - 1 - (h))))

/* 9front: Linux 6.16's fixed-width bits and masks (the value cast to the type) */
#define BIT_U8(n)		((u8)BIT(n))
#define BIT_U16(n)		((u16)BIT(n))
#define BIT_U32(n)		((u32)BIT(n))
#define BIT_U64(n)		((u64)BIT_ULL(n))
#define GENMASK_U8(h, l)	((u8)GENMASK(h, l))
#define GENMASK_U16(h, l)	((u16)GENMASK(h, l))
#define GENMASK_U32(h, l)	((u32)GENMASK(h, l))
#define GENMASK_U64(h, l)	((u64)GENMASK_ULL(h, l))

/* 9front: Linux 6.13's names of the masks' inner forms */
#define __GENMASK(h, l)		GENMASK(h, l)
#define __GENMASK_ULL(h, l)	GENMASK_ULL(h, l)

#endif
