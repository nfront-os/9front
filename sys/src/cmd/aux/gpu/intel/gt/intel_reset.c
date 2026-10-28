/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: engine reset.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/gt/intel_reset.c (the code first from v4.4);
 * the notices are in NOTICE.
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

	wr(e->base+Rresetctl, masked(Resetrequest, 1));
	for(i = 0; i < 20 && (rd(e->base+Rresetctl) & Resetready) == 0; i++)
		sleep(1);
	wr(Gdrst, e->resetbit);
	for(i = 0; i < 500 && (rd(Gdrst) & e->resetbit) != 0; i++)
		sleep(1);
	wr(e->base+Rresetctl, masked(Resetrequest, 0));

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
		ecimage(ec);
		ec->serial = 0;
		ec->fresh = 1;
	}
	enginestart(e);
	e->nreset++;
	fprint(2, "%s: %s reset, %d contexts failed, %s\n", argv0, e->name, n,
		e->ok ? "running again" : "not running");
}
