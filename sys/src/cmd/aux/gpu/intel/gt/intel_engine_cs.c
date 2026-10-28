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
	{ "rcs", 0x2000, 0, 1<<1, Rcsstatepages, 0 },	/* GEN6_GRDOM_RENDER */
	{ "bcs", 0x22000, 16, 1<<3, Xcsstatepages, 1 },	/* GEN6_GRDOM_BLT */
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
	wr(e->base+Rhwstam, ~0);
	wr(e->base+Rimr, ~((Userintr|Ctxswitchintr)<<e->irqshift));
	wr(e->base+Rhws, e->hwsgtt);
	rd(e->base+Rhws);
	wr(e->base+Rmode, masked(Replaymode, 0) | masked(Runlist, 1));
	rd(e->base+Rmode);
	if(e->idx == 0){
		/* i915's gen8_init_render_ring */
		wr(e->base+Rmimode, masked(Asyncflipperfdisable, 1));
		wr(e->base+Rinstpm, masked(Instpmforceordering, 1));
	}
	/* after power-up or a reset the write pointer is all ones: read from entry 0 */
	p = rd(e->base+Rcsbptr) & Csbptrmask;
	if(p == Csbptrmask)
		p = Csbentries-1;
	e->csbread = p;
	e->since = nsec();
	if((rd(e->base+Rmode) & Runlist) == 0){
		fprint(2, "%s: %s: no execlists: mode %#ux\n", argv0, e->name, rd(e->base+Rmode));
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
