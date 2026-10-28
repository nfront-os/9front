/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2020 Intel Corporation
 *
 * Please try to maintain the following order within this file unless it makes
 * sense to do otherwise. From top to bottom:
 * 1. typedefs
 * 2. #defines, and macros
 * 3. structure definitions
 * 4. function prototypes
 *
 * Within each section, please try to order by generation in ascending order,
 * from top to bottom (ie. gen6 on the top, gen8 on the bottom).
 */

/*
 * From Linux 6.6, drivers/gpu/drm/i915/gt/intel_gtt.h,
 * for aux/intelgpu: the definitions it uses, as they are there.
 */

#ifndef _INTELGPU_GT_INTEL_GTT_H_
#define _INTELGPU_GT_INTEL_GTT_H_

#define PPAT_UNCACHED			(_PAGE_PWT | _PAGE_PCD)
#define PPAT_CACHED			_PAGE_PAT /* WB LLCeLLC */
#define PPAT_DISPLAY_ELLC		_PAGE_PCD /* WT eLLC */
#define GEN8_PAGE_PRESENT		BIT_ULL(0)
#define GEN8_PAGE_RW			BIT_ULL(1)

#endif
