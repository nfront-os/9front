/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: engine reset.
 *
 * Follows Linux 6.6's drivers/gpu/drm/i915/gt/intel_reset.c (much of
 * it first written after v4.4); the notices are in NOTICE, UPSTREAM
 * says which functions correspond.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

/*
 * an engine reset (i915's gen8 reset_prepare and gen6_reset_engines):
 * the engine asked to stop, reset through GDRST, started again; the
 * contexts it ran or had queued fail, their work counts as done and
 * their images start afresh.  With lk.
 */
void
engreset(Engine *e)
{
	Ectx *ec, *bad[Nctx+1];
	Ctx *c;
	int i, n;

	wr(RING_RESET_CTL(e->base), _MASKED_BIT_ENABLE(RESET_CTL_REQUEST_RESET));
	for(i = 0; i < 20 && (rd(RING_RESET_CTL(e->base)) & RESET_CTL_READY_TO_RESET) == 0; i++)
		sleep(1);
	wr(GEN6_GDRST, e->resetbit);
	for(i = 0; i < 500 && (rd(GEN6_GDRST) & e->resetbit) != 0; i++)
		sleep(1);
	wr(RING_RESET_CTL(e->base), _MASKED_BIT_DISABLE(RESET_CTL_REQUEST_RESET));

	n = 0;
	if(e->running != nil)
		bad[n++] = e->running;
	for(ec = e->queue; ec != nil && n < nelem(bad); ec = ec->next)
		bad[n++] = ec;
	e->queue = nil;
	for(i = 0; i < n; i++){
		ec = bad[i];
		c = ec->ctx;
		c->failed = 1;
		*ec->seqno = ec->nseq;
		flushcache(ec->seqno, 4);
		ec->queued = 0;
		ec->next = nil;
		lrc_init_regs(ec);
		ec->serial = 0;
		ec->fresh = 1;
	}
	enginestart(e);
	e->nreset++;
	fprint(2, "%s: %s reset, %d contexts failed, %s\n", argv0, e->name, n,
		e->ok ? "running again" : "not running");
}
