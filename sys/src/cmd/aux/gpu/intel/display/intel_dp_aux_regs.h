/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2023 Intel Corporation
 */

/*
 * From Linux 6.6, drivers/gpu/drm/i915/display/intel_dp_aux_regs.h,
 * for aux/intelgpu: the definitions it uses, as they are there.
 */

#ifndef _INTELGPU_DISPLAY_INTEL_DP_AUX_REGS_H_
#define _INTELGPU_DISPLAY_INTEL_DP_AUX_REGS_H_

#define _DPA_AUX_CH_CTL		(DISPLAY_MMIO_BASE(dev_priv) + 0x64010)
#define _DPB_AUX_CH_CTL		(DISPLAY_MMIO_BASE(dev_priv) + 0x64110)
#define DP_AUX_CH_CTL(aux_ch)	_MMIO_PORT(aux_ch, _DPA_AUX_CH_CTL, _DPB_AUX_CH_CTL)
#define   DP_AUX_CH_CTL_SEND_BUSY		REG_BIT(31)
#define   DP_AUX_CH_CTL_DONE			REG_BIT(30)
#define   DP_AUX_CH_CTL_TIME_OUT_ERROR		REG_BIT(28)
#define   DP_AUX_CH_CTL_TIME_OUT_MASK		REG_GENMASK(27, 26)
#define   DP_AUX_CH_CTL_TIME_OUT_600us		REG_FIELD_PREP(DP_AUX_CH_CTL_TIME_OUT_MASK, 1)
#define   DP_AUX_CH_CTL_RECEIVE_ERROR		REG_BIT(25)
#define   DP_AUX_CH_CTL_MESSAGE_SIZE_MASK	REG_GENMASK(24, 20)
#define   DP_AUX_CH_CTL_MESSAGE_SIZE(x)		REG_FIELD_PREP(DP_AUX_CH_CTL_MESSAGE_SIZE_MASK, (x))
#define   DP_AUX_CH_CTL_PRECHARGE_2US_MASK	REG_GENMASK(19, 16) /* pre-skl */
#define   DP_AUX_CH_CTL_PRECHARGE_2US(x)	REG_FIELD_PREP(DP_AUX_CH_CTL_PRECHARGE_2US_MASK, (x))

#endif
