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
gtinit(void)
{
	u32int m;

	wr(Gamecochk, rd(Gamecochk) | Ecochkarbpriosol);
	wr(Ffthreadmode, rd(Ffthreadmode) & ~(Ffdsrefcntffme | Ffvsrefcntffme));
	wr(Rcsleeppsmictl, masked(Rcsemaidlemsgdisable, 1));
	wr(Ucgctl6, rd(Ucgctl6) | Sdeunitclockgatedisable);
	m = rd(Misccpctl);
	wr(Misccpctl, m & ~Dopclockgateenable);
	wr(L3sqcreg1, L3sqcreg1default);
	wr(Misccpctl, m);
	wr(Gttcacheen, Gttcacheenall);
}
