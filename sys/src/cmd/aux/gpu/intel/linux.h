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

/*
 * what i915_reg_defs.h, the bitops and the display's register macros
 * give there: registers are plain offsets here (_MMIO), the checks at
 * compile time are left out
 */
#define BUILD_BUG_ON_ZERO(e)	0
#define __is_constexpr(x)	1
#define BIT(n)			(1UL << (n))
#define BIT_ULL(n)		(1ULL << (n))
#define GENMASK(h, l)		((~0UL << (l)) & (~0UL >> (8*sizeof(long) - 1 - (h))))
#define U32_MAX			((u32)~0U)
#define REG_BIT(n)		((u32)BIT(n))
#define REG_GENMASK(h, l)	((u32)GENMASK(h, l))
#define REG_FIELD_PREP(mask, val)	((u32)((((u32)(val)) << __builtin_ctzll(mask)) & (mask)))
#define REG_FIELD_GET(mask, val)	((u32)(((val) & (mask)) >> __builtin_ctzll(mask)))
#define _MMIO(r)		(r)
#define MCR_REG(r)		(r)
#define _MASKED_FIELD(mask, value)	(((mask) << 16) | (value))
#define _MASKED_BIT_ENABLE(a)	_MASKED_FIELD((a), (a))
#define _MASKED_BIT_DISABLE(a)	_MASKED_FIELD((a), 0)

/* x86's page bits, which the PPAT index is made of (PPAT_*) */
#define _PAGE_PWT		(1<<3)
#define _PAGE_PCD		(1<<4)
#define _PAGE_PAT		(1<<7)

/* Broadwell GT3 (fence destination SLM workaround): not told apart yet, the GT2 we have */
#define IS_BROADWELL_GT3(i915)	0

/* the display on Broadwell: its pipes' and transcoders' offsets, GMBUS in the PCH */
#define DISPLAY_INFO(i915)	(&bdw_display_info)
#define DISPLAY_MMIO_BASE(i915)	(DISPLAY_INFO(i915)->mmio_offset)
#define GMBUS_MMIO_BASE(i915)	0xC0000		/* PCH_DISPLAY_BASE */
