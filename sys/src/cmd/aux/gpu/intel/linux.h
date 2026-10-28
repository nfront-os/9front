/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: Linux's names for what the code taken from Linux's
 * i915 uses, so that it can stay as it is there.
 */

typedef uchar	u8;
typedef ushort	u16;
typedef u32int	u32;
typedef uvlong	u64;
typedef s32int	s32;
typedef vlong	s64;

#define ARRAY_SIZE(a)	(sizeof(a)/sizeof((a)[0]))
