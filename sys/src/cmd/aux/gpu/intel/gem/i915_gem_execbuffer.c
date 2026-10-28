/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: submit: a batch of a context's on an engine.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/gem/i915_gem_execbuffer.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

char*
submit(Ctx *c, int boid, ulong off, char *engname)
{
	Engine *e;
	Bo *b;
	u32int seq;
	char *err;
	int i;

	if((b = findbo(c, boid)) == nil)
		return "unknown object";
	if(off >= b->size || (off & 7) != 0)
		return "bad offset";
	if(b->gtt == 0)
		return "object not mapped";
	e = nil;
	for(i = 0; i < nelem(eng); i++)
		if(strcmp(eng[i].name, engname) == 0)
			e = &eng[i];
	if(e == nil)
		return "unknown engine";
	qlock(&lk);
	if(c->nfence+1 - c->donefence > Fwin){
		qunlock(&lk);
		return "too many fences in flight";
	}
	if((err = ecsubmit(&c->ec[e->idx], b->gtt + off, &seq)) != nil){
		qunlock(&lk);
		return err;
	}
	c->nfence++;
	c->fe[c->nfence % Fwin] = &c->ec[e->idx];
	c->fseq[c->nfence % Fwin] = seq;
	c->ftime[c->nfence % Fwin] = nsec();
	qunlock(&lk);
	return nil;
}
