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
			csb(e);
			if(e->running != nil && nsec() - e->since > (vlong)Timeout*1000000){
				fprint(2, "%s: %s hung in context %d: head %#ux acthd %#ux ipehr %#ux eir %#ux execlist %#ux"
					" pdp0 %#.8ux%.8ux (pd0 %#llux)\n",
					argv0, e->name, e->running->ctx->id, rd(e->base+Rhead) & Headmask,
					rd(e->base+Racthd), rd(e->base+Ripehr), rd(e->base+Reir), rd(e->base+Rexecstatus),
					rd(e->base+Rpdp+4), rd(e->base+Rpdp), pdpval(e->running->ctx, 0));
				engreset(e);
				irqinit();
				kick(e);
			}
		}
		if(busy)
			rps.busy = nsec();
		else if(nsec() - rps.busy > 100*1000000LL)
			rpsidle();
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
