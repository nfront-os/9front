/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

/*
 * From Linux 6.6, drivers/gpu/drm/i915/gt/intel_engine_regs.h,
 * for aux/intelgpu: the definitions it uses, as they are there.
 */

#ifndef _INTELGPU_GT_INTEL_ENGINE_REGS_H_
#define _INTELGPU_GT_INTEL_ENGINE_REGS_H_

#define RING_TAIL(base)				_MMIO((base) + 0x30)
#define RING_HEAD(base)				_MMIO((base) + 0x34)
#define   HEAD_ADDR				0x001FFFFC
#define RING_START(base)			_MMIO((base) + 0x38)
#define RING_CTL(base)				_MMIO((base) + 0x3c)
#define   RING_VALID				0x00000001
#define RING_PSMI_CTL(base)			_MMIO((base) + 0x50)
#define   GEN8_RC_SEMA_IDLE_MSG_DISABLE		REG_BIT(12)
#define RING_MAX_IDLE(base)			_MMIO((base) + 0x54)
#define RING_IPEHR(base)			_MMIO((base) + 0x68)
#define RING_ACTHD(base)			_MMIO((base) + 0x74)
#define RING_HWS_PGA(base)			_MMIO((base) + 0x80)
#define RING_HWSTAM(base)			_MMIO((base) + 0x98)
#define RING_MI_MODE(base)			_MMIO((base) + 0x9c)
#define   ASYNC_FLIP_PERF_DISABLE		REG_BIT(14)
#define RING_IMR(base)				_MMIO((base) + 0xa8)
#define RING_EIR(base)				_MMIO((base) + 0xb0)
#define RING_INSTPM(base)			_MMIO((base) + 0xc0)
#define GEN8_R_PWR_CLK_STATE(base)		_MMIO((base) + 0xc8)
#define RING_RESET_CTL(base)			_MMIO((base) + 0xd0)
#define   RESET_CTL_READY_TO_RESET		REG_BIT(1)
#define   RESET_CTL_REQUEST_RESET		REG_BIT(0)
#define RING_BBSTATE(base)			_MMIO((base) + 0x110)
#define RING_SBBADDR(base)			_MMIO((base) + 0x114) /* hsw+ */
#define RING_SBBSTATE(base)			_MMIO((base) + 0x118) /* hsw+ */
#define RING_SBBADDR_UDW(base)			_MMIO((base) + 0x11c) /* gen8+ */
#define RING_BBADDR(base)			_MMIO((base) + 0x140)
#define RING_BBADDR_UDW(base)			_MMIO((base) + 0x168) /* gen8+ */
#define RING_BB_PER_CTX_PTR(base)		_MMIO((base) + 0x1c0) /* gen8+ */
#define RING_INDIRECT_CTX(base)			_MMIO((base) + 0x1c4) /* gen8+ */
#define RING_INDIRECT_CTX_OFFSET(base)		_MMIO((base) + 0x1c8) /* gen8+ */
#define RING_ELSP(base)				_MMIO((base) + 0x230)
#define RING_EXECLIST_STATUS_LO(base)		_MMIO((base) + 0x234)
#define RING_CONTEXT_CONTROL(base)		_MMIO((base) + 0x244)
#define	  CTX_CTRL_ENGINE_CTX_RESTORE_INHIBIT	REG_BIT(0)
#define   CTX_CTRL_RS_CTX_ENABLE		REG_BIT(1)
#define	  CTX_CTRL_INHIBIT_SYN_CTX_SWITCH	REG_BIT(3)
#define GEN8_RING_PDP_UDW(base, n)		_MMIO((base) + 0x270 + (n) * 8 + 4)
#define GEN8_RING_PDP_LDW(base, n)		_MMIO((base) + 0x270 + (n) * 8)
#define RING_MODE_GEN7(base)			_MMIO((base) + 0x29c)
#define   GFX_RUN_LIST_ENABLE			(1 << 15)
#define   GFX_REPLAY_MODE			(1 << 11)
#define RING_CONTEXT_STATUS_PTR(base)		_MMIO((base) + 0x3a0)
#define RING_CTX_TIMESTAMP(base)		_MMIO((base) + 0x3a8) /* gen8+ */

#endif
