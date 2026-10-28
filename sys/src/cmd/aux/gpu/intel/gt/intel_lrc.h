/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2014 Intel Corporation
 */

/*
 * From Linux 6.6, drivers/gpu/drm/i915/gt/intel_lrc.h,
 * for aux/intelgpu: the definitions it uses, as they are there.
 */

#ifndef _INTELGPU_GT_INTEL_LRC_H_
#define _INTELGPU_GT_INTEL_LRC_H_

enum {
	INTEL_ADVANCED_CONTEXT = 0,
	INTEL_LEGACY_32B_CONTEXT,
	INTEL_ADVANCED_AD_CONTEXT,
	INTEL_LEGACY_64B_CONTEXT
};
#define GEN8_CTX_VALID				(1 << 0)
#define GEN8_CTX_L3LLC_COHERENT			(1 << 5)
#define GEN8_CTX_PRIVILEGE			(1 << 8)
#define GEN8_CTX_ADDRESSING_MODE_SHIFT		3

#endif
