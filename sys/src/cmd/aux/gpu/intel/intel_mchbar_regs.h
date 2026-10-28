/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

/*
 * From Linux 6.6, drivers/gpu/drm/i915/intel_mchbar_regs.h,
 * for aux/intelgpu: the definitions it uses, as they are there.
 */

#ifndef _INTELGPU_INTEL_MCHBAR_REGS_H_
#define _INTELGPU_INTEL_MCHBAR_REGS_H_

#define MCHBAR_MIRROR_BASE_SNB			0x140000
#define GEN6_RP_STATE_CAP			_MMIO(MCHBAR_MIRROR_BASE_SNB + 0x5998)
#define MCH_SSKPD				_MMIO(MCHBAR_MIRROR_BASE_SNB + 0x5d10)

#endif
