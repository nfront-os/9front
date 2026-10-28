/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: contexts: their pages, create, destroy.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/gem/i915_gem_context.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

u32int *ctxpool;		/* Nctx slots of ctxslot() bytes */
uvlong ctxpoolpa;
ulong ctxpoolgtt;
Ctx *ctxs[Nctx];

/*
 * a context's pages: its slot of the pool (attached before the driver's
 * procs are forked: a segment attached later is not theirs), mapped
 * into the GTT with it: the status page (the sequence numbers), then
 * per engine a context image and a ring
 */
ulong
ctxslot(void)
{
	ulong n;
	int k;

	n = 4096;
	for(k = 0; k < nelem(eng); k++)
		n += (1 + eng[k].statepages + Ringpages)*4096;
	return n;
}

int
ctxmem(Ctx *c, int i)
{
	ulong off;
	int k;

	c->size = ctxslot();
	c->mem = ctxpool + (uvlong)i*c->size/4;
	c->pa = ctxpoolpa + (uvlong)i*c->size;
	c->gtt = ctxpoolgtt + i*c->size;
	memset(c->mem, 0, c->size);
	qlock(&lk);
	off = 4096;
	for(k = 0; k < nelem(eng); k++){
		ecinit(c, &c->ec[k], &eng[k], off);
		off += (1 + eng[k].statepages + Ringpages)*4096;
	}
	qunlock(&lk);
	return 0;
}

void
ctxmemfree(Ctx *c)
{
	memset(c->mem, 0, c->size);
}

Ctx*
newctx(void)
{
	char name[16], *user;
	Ctx *c;
	int i;

	for(i = 0; i < Nctx; i++)
		if(ctxs[i] == nil)
			break;
	if(i == Nctx){
		werrstr("too many contexts");
		return nil;
	}
	c = emalloc9p(sizeof *c);
	memset(c, 0, sizeof *c);
	c->id = i+1;
	c->pml4 = -1;
	if(ctxmem(c, i) < 0){
		free(c);
		return nil;
	}
	qlock(&lk);
	c->serial = ++serial;
	if(!global && gen8_ppgtt_create(c) < 0){
		qunlock(&lk);
		ctxmemfree(c);
		free(c);
		werrstr("no page tables left");
		return nil;
	}
	qunlock(&lk);
	user = getuser();
	snprint(name, sizeof name, "%d", c->id);
	c->dir = createfile(ctxdir, name, user, DMDIR|0555, nil);
	c->ctl = createfile(c->dir, "ctl", user, 0660, (void*)(uintptr)(Xctxbase*(i+1)+Xctxctl));
	c->submit = createfile(c->dir, "submit", user, 0220, (void*)(uintptr)(Xctxbase*(i+1)+Xctxsubmit));
	c->fence = createfile(c->dir, "fence", user, 0660, (void*)(uintptr)(Xctxbase*(i+1)+Xctxfence));
	qlock(&lk);
	ctxs[i] = c;
	qunlock(&lk);
	ctxreply(c, "ctx %d\n", c->id);
	return c;
}

/* last fid gone: its work done (or given up), its objects freed */
void
freectx(Ctx *c)
{
	int i;

	if(c->nfence > c->donefence)
		fencewait(c, c->nfence);
	for(i = 0; i < Nbo; i++)
		if(bos[i] != nil && bos[i]->ctx == c)
			bofree(bos[i]);
	removefile(c->ctl);
	removefile(c->submit);
	removefile(c->fence);
	removefile(c->dir);
	closefile(c->ctl);
	closefile(c->submit);
	closefile(c->fence);
	closefile(c->dir);
	/* off the engines: not running, not queued */
	qlock(&lk);
	for(i = 0; i < nelem(eng); i++){
		while(eng[i].running == &c->ec[i] && eng[i].ok && nsec() - eng[i].since < (vlong)Timeout*1000000)
			rsleep(&done);
		if(eng[i].running == &c->ec[i]){
			engreset(&eng[i]);
			gen8_irq_postinstall();
		}
		if(c->ec[i].queued){
			Ectx **l;

			for(l = &eng[i].queue; *l != nil; l = &(*l)->next)
				if(*l == &c->ec[i]){
					*l = c->ec[i].next;
					break;
				}
			c->ec[i].queued = 0;
		}
	}
	ctxs[c->id-1] = nil;
	if(!global)
		gen8_ppgtt_cleanup(c);
	qunlock(&lk);
	ctxmemfree(c);
	for(i = 0; i < nelem(eng); i++){
		qlock(&lk);
		execlists_dequeue(&eng[i]);
		qunlock(&lk);
	}
	free(c->reply);
	free(c);
}
