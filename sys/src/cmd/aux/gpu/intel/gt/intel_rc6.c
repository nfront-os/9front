/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: RC6: the GT sleeps when idle (timeout mode, 800 µs).
 *
 * Follows Linux 6.6's drivers/gpu/drm/i915/gt/intel_rc6.c (much of it
 * first written after v4.4); the notices are in NOTICE, UPSTREAM says
 * which functions correspond.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

/* as Linux's intel_rc6_enable with gen8_rc6_enable for Broadwell */
void
gen8_rc6_enable(void)
{
	int i;

	wr(GEN6_RC_STATE, 0);			/* software RC state: RC0 */
	wr(GEN6_RC_CONTROL, 0);		/* RC states off while programming */

	/* 2b: Program RC6 thresholds.*/
	wr(GEN6_RC6_WAKE_RATE_LIMIT, 40<<16);
	wr(GEN6_RC_EVALUATION_INTERVAL, 125000);		/* 12500 * 1280 ns */
	wr(GEN6_RC_IDLE_HYSTERSIS, 25);		/* 25 * 1280 ns */
	for(i = 0; i < nelem(eng); i++)
		wr(RING_MAX_IDLE(eng[i].base), 10);
	wr(GEN6_RC_SLEEP, 0);
	wr(GEN6_RC6_THRESHOLD, 625);		/* 800 µs / 1.28, timeout mode */

	/* 3: Enable RC6 */
	wr(GEN6_RC_CONTROL, GEN6_RC_CTL_HW_ENABLE | GEN7_RC_CTL_TO_MODE | GEN6_RC_CTL_RC6_ENABLE);
}
