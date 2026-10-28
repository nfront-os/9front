/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: the engines: start, init.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/gt/intel_engine_cs.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

Engine eng[2] = {
	{ "rcs", RENDER_RING_BASE, GEN8_RCS_IRQ_SHIFT, GEN6_GRDOM_RENDER, Rcsstatepages, 0 },
	{ "bcs", BLT_RING_BASE, GEN8_BCS_IRQ_SHIFT, GEN6_GRDOM_BLT, Xcsstatepages, 1 },
};

/*
 * engines in execlist mode (i915's gen8_init_common_ring): the
 * engine's status page, interrupts, the run list on, the context
 * status buffer's read pointer where the hardware has it
 */
void
enginestart(Engine *e)
{
	u32int p;

	e->ok = 0;
	e->running = nil;
	wr(RING_HWSTAM(e->base), ~0);
	wr(RING_IMR(e->base), ~((GT_RENDER_USER_INTERRUPT|GT_CONTEXT_SWITCH_INTERRUPT)<<e->irqshift));
	wr(RING_HWS_PGA(e->base), e->hwsgtt);
	rd(RING_HWS_PGA(e->base));
	wr(RING_MODE_GEN7(e->base), masked(GFX_REPLAY_MODE, 0) | masked(GFX_RUN_LIST_ENABLE, 1));
	rd(RING_MODE_GEN7(e->base));
	if(e->idx == 0){
		/* i915's gen8_init_render_ring */
		wr(RING_MI_MODE(e->base), masked(ASYNC_FLIP_PERF_DISABLE, 1));
		wr(RING_INSTPM(e->base), masked(INSTPM_FORCE_ORDERING, 1));
	}
	/* after power-up or a reset the write pointer is all ones: read from entry 0 */
	p = rd(RING_CONTEXT_STATUS_PTR(e->base)) & GEN8_CSB_PTR_MASK;
	if(p == GEN8_CSB_PTR_MASK)
		p = GEN8_CSB_ENTRIES-1;
	e->csbread = p;
	e->since = nsec();
	if((rd(RING_MODE_GEN7(e->base)) & GFX_RUN_LIST_ENABLE) == 0){
		fprint(2, "%s: %s: no execlists: mode %#ux\n", argv0, e->name, rd(RING_MODE_GEN7(e->base)));
		return;
	}
	e->ok = 1;
}

void
engineinit(Engine *e)
{
	e->hws = drv + e->idx*1024;
	e->hwsgtt = drvgtt + e->idx*4096;
	memset(e->hws, 0, 4096);
	flushcache(e->hws, 4096);
	enginestart(e);
}
