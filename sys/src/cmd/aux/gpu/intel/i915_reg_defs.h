/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

/*
 * From Linux 6.6, drivers/gpu/drm/i915/i915_reg_defs.h,
 * for aux/intelgpu: the definitions it uses, as they are there.
 */

#ifndef _INTELGPU_I915_REG_DEFS_H_
#define _INTELGPU_I915_REG_DEFS_H_

#define _PICK_EVEN(__index, __a, __b) ((__a) + (__index) * ((__b) - (__a)))
#define _PICK_EVEN_2RANGES(__index, __c_index, __a, __b, __c, __d)		\
	(BUILD_BUG_ON_ZERO(!__is_constexpr(__c_index)) +			\
	 ((__index) < (__c_index) ? _PICK_EVEN(__index, __a, __b) :		\
				   _PICK_EVEN((__index) - (__c_index), __c, __d)))

#endif
