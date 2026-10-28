/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: fences: a context's sequence numbers done, the hang watch.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/i915_request.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "i915_drv.h"

/*
 * fences
 */
int
fencedone(Ctx *c, uvlong k)
{
	Ectx *ec;

	if(k <= c->donefence)
		return 1;
	if(k > c->nfence)
		return 0;
	ec = c->fe[k % Fwin];
	return ec == nil || (int)(ecdone(ec) - c->fseq[k % Fwin]) >= 0;
}

/* advance every context's donefence; with lk */
void
reap(void)
{
	Ctx *c;
	int i;

	for(i = 0; i < Nctx; i++){
		if((c = ctxs[i]) == nil)
			continue;
		while(c->donefence < c->nfence && fencedone(c, c->donefence+1))
			c->donefence++;
	}
}

/*
 * now and then: the status buffers (an interrupt may be lost), the
 * waiters' timeouts, an engine that runs one context too long hung
 */
void
tickproc(void)
{
	Engine *e;
	int i, busy;

	for(;;){
		sleep(100);
		qlock(&lk);
		busy = 0;
		for(i = 0; i < nelem(eng); i++){
			e = &eng[i];
			/* idle engines are left alone: a register read wakes the GT */
			if(e->running == nil && e->queue == nil)
				continue;
			busy = 1;
			process_csb(e);
			if(e->running != nil && nsec() - e->since > (vlong)Timeout*1000000){
				fprint(2, "%s: %s hung in context %d: head %#ux acthd %#ux ipehr %#ux eir %#ux execlist %#ux"
					" pdp0 %#.8ux%.8ux (pd0 %#llux)\n",
					argv0, e->name, e->running->ctx->id, rd(RING_HEAD(e->base)) & HEAD_ADDR,
					rd(RING_ACTHD(e->base)), rd(RING_IPEHR(e->base)), rd(RING_EIR(e->base)), rd(RING_EXECLIST_STATUS_LO(e->base)),
					rd(GEN8_RING_PDP_UDW(e->base, 0)), rd(GEN8_RING_PDP_LDW(e->base, 0)), pdpval(e->running->ctx, 0));
				engreset(e);
				gen8_irq_postinstall();
				execlists_dequeue(e);
			}
		}
		if(busy)
			rps.busy = nsec();
		else if(nsec() - rps.busy > 100*1000000LL)
			intel_rps_park();
		reap();
		rwakeupall(&done);
		qunlock(&lk);
	}
}

/* waits for fence k; gives up after Timeout: the engine hung */
int
fencewait(Ctx *c, uvlong k)
{
	qlock(&lk);
	while(!fencedone(c, k)){
		if(nsec() - c->ftime[k % Fwin] > (vlong)Timeout*1000000){
			c->failed = 1;
			qunlock(&lk);
			return -1;
		}
		rsleep(&done);
	}
	qunlock(&lk);
	return 0;
}
