/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

/*
 * From Linux 6.6, drivers/gpu/drm/i915/gt/intel_gt_regs.h,
 * for aux/intelgpu: the definitions it uses, as they are there.
 */

#ifndef _INTELGPU_GT_INTEL_GT_REGS_H_
#define _INTELGPU_GT_INTEL_GT_REGS_H_

#define   GEN6_WIZ_HASHING(hi, lo)		(((hi) << 9) | ((lo) << 7))
#define   GEN6_WIZ_HASHING_16x4			GEN6_WIZ_HASHING(1, 0)
#define   GEN6_WIZ_HASHING_MASK			GEN6_WIZ_HASHING(1, 1)
#define GAM_ECOCHK				_MMIO(0x4090)
#define   HSW_ECOCHK_ARB_PRIO_SOL		(1 << 6)
#define GEN8_PRIVATE_PAT_LO			_MMIO(0x40e0)
#define GEN8_PRIVATE_PAT_HI			_MMIO(0x40e0 + 4)
#define CACHE_MODE_0_GEN7			_MMIO(0x7000) /* IVB+ */
#define   HIZ_RAW_STALL_OPT_DISABLE		(1 << 2)
#define CACHE_MODE_1				_MMIO(0x7004) /* IVB+ */
#define   GEN8_4x4_STC_OPTIMIZATION_DISABLE	REG_BIT(6)
#define GEN7_GT_MODE				_MMIO(0x7008)
#define HDC_CHICKEN0				_MMIO(0x7300)
#define   HDC_FENCE_DEST_SLM_DISABLE		(1 << 14)
#define   HDC_DONOT_FETCH_MEM_WHEN_MASKED	(1 << 11)
#define   HDC_FORCE_CONTEXT_SAVE_RESTORE_NON_COHERENT	(1 << 5)
#define   HDC_FORCE_NON_COHERENT		(1 << 4)
#define GEN8_FUSE2				_MMIO(0x9120)
#define GEN8_EU_DISABLE0			_MMIO(0x9134)
#define GEN8_EU_DISABLE1			_MMIO(0x9138)
#define GEN8_EU_DISABLE2			_MMIO(0x913c)
#define GEN6_GDRST				_MMIO(0x941c)
#define   GEN6_GRDOM_RENDER			(1 << 1)
#define   GEN6_GRDOM_BLT			(1 << 3)
#define GEN7_MISCCPCTL				_MMIO(0x9424)
#define   GEN7_DOP_CLOCK_GATE_ENABLE		REG_BIT(0)
#define GEN8_UCGCTL6				_MMIO(0x9430)
#define   GEN8_SDEUNIT_CLOCK_GATE_DISABLE	(1 << 14)
#define GEN6_RPNSWREQ				_MMIO(0xa008)
#define GEN6_RC_VIDEO_FREQ			_MMIO(0xa00c)
#define   GEN6_RC_CTL_RC6_ENABLE		(1 << 18)
#define   GEN7_RC_CTL_TO_MODE			(1 << 28)
#define   GEN6_RC_CTL_HW_ENABLE			(1 << 31)
#define GEN6_RP_DOWN_TIMEOUT			_MMIO(0xa010)
#define GEN6_RP_INTERRUPT_LIMITS		_MMIO(0xa014)
#define GEN6_RPSTAT1				_MMIO(0xa01c)
#define GEN6_RP_CONTROL				_MMIO(0xa024)
#define   GEN6_RP_MEDIA_TURBO			(1 << 11)
#define   GEN6_RP_MEDIA_HW_NORMAL_MODE		(2 << 9)
#define   GEN6_RP_MEDIA_IS_GFX			(1 << 8)
#define   GEN6_RP_ENABLE			(1 << 7)
#define   GEN6_RP_UP_BUSY_AVG			(0x2 << 3)
#define   GEN6_RP_DOWN_IDLE_AVG			(0x2 << 0)
#define GEN6_RP_UP_THRESHOLD			_MMIO(0xa02c)
#define GEN6_RP_DOWN_THRESHOLD			_MMIO(0xa030)
#define GEN6_RP_UP_EI				_MMIO(0xa068)
#define GEN6_RP_DOWN_EI				_MMIO(0xa06c)
#define GEN6_RP_IDLE_HYSTERSIS			_MMIO(0xa070)
#define GEN6_RC_CONTROL				_MMIO(0xa090)
#define GEN6_RC_STATE				_MMIO(0xa094)
#define GEN6_RC6_WAKE_RATE_LIMIT		_MMIO(0xa09c)
#define GEN6_RC_EVALUATION_INTERVAL		_MMIO(0xa0a8)
#define GEN6_RC_IDLE_HYSTERSIS			_MMIO(0xa0ac)
#define GEN6_RC_SLEEP				_MMIO(0xa0b0)
#define GEN6_RC6_THRESHOLD			_MMIO(0xa0b8)
#define GEN6_PMINTRMSK				_MMIO(0xa168)
#define   GEN8_PMINTR_DISABLE_REDIRECT_TO_GUC	(1 << 31)
#define FORCEWAKE_MT				_MMIO(0xa188) /* multi-threaded */
#define GEN8_L3SQCREG1				MCR_REG(0xb100)
#define   L3_GENERAL_PRIO_CREDITS(x)		(((x) >> 1) << 19)
#define   L3_HIGH_PRIO_CREDITS(x)		(((x) >> 1) << 14)
#define GEN8_HALF_SLICE_CHICKEN3		MCR_REG(0xe184)
#define   GEN8_SAMPLER_POWER_BYPASS_DIS		(1 << 1)
#define GEN8_ROW_CHICKEN			MCR_REG(0xe4f0)
#define   PARTIAL_INSTRUCTION_SHOOTDOWN_DISABLE	REG_BIT(8)
#define   STALL_DOP_GATING_DISABLE		REG_BIT(5)
#define GEN8_ROW_CHICKEN2			MCR_REG(0xe4f4)
#define   DOP_CLOCK_GATING_DISABLE		(1 << 0)
#define   GEN6_PM_RP_DOWN_TIMEOUT		(1 << 6)
#define   GEN6_PM_RP_UP_THRESHOLD		(1 << 5)
#define   GEN6_PM_RP_DOWN_THRESHOLD		(1 << 4)
#define   GEN6_PM_RP_UP_EI_EXPIRED		(1 << 2)
#define   GEN6_PM_RP_DOWN_EI_EXPIRED		(1 << 1)
#define GFX_FLSH_CNTL_GEN6			_MMIO(0x101008)
#define FORCEWAKE_ACK_HSW			_MMIO(0x130044)
#define   FORCEWAKE_KERNEL			BIT(0)
#define GEN6_GT_GFX_RC6				_MMIO(0x138108)

#endif
