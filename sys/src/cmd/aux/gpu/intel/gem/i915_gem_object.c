/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: buffer objects: #g segments mapped into a context.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/gem/i915_gem_object.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

Bo *bos[Nbo];

Bo*
findbo(Ctx *c, int id)
{
	if(id < 1 || id > Nbo || bos[id-1] == nil || bos[id-1]->ctx != c)
		return nil;
	return bos[id-1];
}

/*
 * map an object at GPU address at (Anyaddr: the driver's choice, 0:
 * nowhere); with lk
 */
char*
bomap(Bo *b, uvlong at)
{
	Ctx *c;

	c = b->ctx;
	if(at == 0)
		return nil;
	if(global){
		if(at != Anyaddr)
			return "no GPU address of the client's choice with -G";
		if((b->gtt = gttalloc(b->size)) == 0)
			return "no GPU address space left";
		gttmap(b->gtt, 0, b->pa, b->size);
		return nil;
	}
	if(at == Anyaddr){
		if((at = holealloc(&c->holes, b->size)) == 0)
			return "no GPU address space left";
	}else if((at & 4095) != 0 || holetake(&c->holes, at, b->size) < 0)
		return "GPU address in use or out of range";
	if(ppmap(c, at, 0, b->pa, b->size, uncached ? PPAT_UNCACHED : b->scanwt ? PPAT_DISPLAY_ELLC : PPAT_CACHED) < 0){
		ppunmap(c, at, b->size);
		holefree(&c->holes, at, b->size);
		return "no page tables left";
	}
	b->gtt = at;
	return nil;
}

/* with lk */
void
bounmap(Bo *b)
{
	if(b->gtt == 0)
		return;
	if(global){
		if(!b->plane){
			gttunmap(b->gtt, b->size);
			gttfree(b->gtt, b->size);
		}
	}else{
		ppunmap(b->ctx, b->gtt, b->size);
		holefree(&b->ctx->holes, b->gtt, b->size);
	}
	b->gtt = 0;
}

char*
bonew(Ctx *c, uvlong size, uvlong at)
{
	char *err;
	Bo *b;
	int i;

	if(size == 0 || size > Bomax)
		return "bad size";
	size = (size + 4095) & ~4095ULL;
	for(i = 0; i < Nbo; i++)
		if(bos[i] == nil)
			break;
	if(i == Nbo)
		return "too many objects";
	b = emalloc9p(sizeof *b);
	memset(b, 0, sizeof *b);
	b->id = i+1;
	b->ctx = c;
	b->size = size;
	b->va = Bova + (uvlong)i*Bomax;
	snprint(b->seg, sizeof b->seg, "gpu0bo%d", b->id);
	if((b->pa = stickyseg(b->seg, b->va, size, 0770)) == nil){
		rmseg(b->seg);
		free(b);
		return "no memory";
	}
	qlock(&lk);
	err = bomap(b, at);
	qunlock(&lk);
	if(err != nil){
		rmseg(b->seg);
		free(b->pa);
		free(b);
		return err;
	}
	bos[i] = b;
	return ctxreply(c, "bo %d %s %#llux %#llux %#llux\n", b->id, b->seg, b->va, b->size, b->gtt);
}

/* the object to GPU address at (0: nowhere), once the context's work is done */
char*
bobind(Bo *b, uvlong at)
{
	Ctx *c;
	char *err;

	c = b->ctx;
	if(b->plane)
		return "the plane stays where it is";
	if(at == b->gtt)
		return nil;
	if(c->nfence > c->donefence)
		fencewait(c, c->nfence);
	qlock(&lk);
	bounmap(b);
	err = bomap(b, at);
	qunlock(&lk);
	return err;
}

void
bofree(Bo *b)
{
	qlock(&lk);
	if(scan.bo == b)
		scanrestore();
	if(b->scanbase != 0){
		gttunmap(b->scangtt, b->size);
		gttfree(b->scanbase, b->size + Scanalign);
	}
	bounmap(b);
	qunlock(&lk);
	if(!b->plane)
		rmseg(b->seg);
	bos[b->id-1] = nil;
	free(b->pa);
	free(b);
}
