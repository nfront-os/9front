/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: Broadwell's clock gating (GT part).
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/intel_clock_gating.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "i915_drv.h"

/* the GT part of i915's broadwell_init_clock_gating */
void
bdw_init_clock_gating(void)
{
	u32int m;

	wr(GAM_ECOCHK, rd(GAM_ECOCHK) | HSW_ECOCHK_ARB_PRIO_SOL);
	wr(GEN7_FF_THREAD_MODE, rd(GEN7_FF_THREAD_MODE) & ~(GEN8_FF_DS_REF_CNT_FFME | GEN7_FF_VS_REF_CNT_FFME));
	wr(RING_PSMI_CTL(RENDER_RING_BASE), masked(GEN8_RC_SEMA_IDLE_MSG_DISABLE, 1));
	wr(GEN8_UCGCTL6, rd(GEN8_UCGCTL6) | GEN8_SDEUNIT_CLOCK_GATE_DISABLE);
	m = rd(GEN7_MISCCPCTL);
	wr(GEN7_MISCCPCTL, m & ~GEN7_DOP_CLOCK_GATE_ENABLE);
	wr(GEN8_L3SQCREG1, (L3_GENERAL_PRIO_CREDITS(30) | L3_HIGH_PRIO_CREDITS(2)));
	wr(GEN7_MISCCPCTL, m);
	wr(HSW_GTT_CACHE_EN, GTT_CACHE_EN_ALL);
}
