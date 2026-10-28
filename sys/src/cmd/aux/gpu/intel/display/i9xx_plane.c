/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: the primary plane: a client's object shown, scanout, flips.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/display/i9xx_plane.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

Scan scan;

/* the visible plane: GTT address, stride, bits a pixel, size; -1 if none */
int
plane(ulong *surf, ulong *stride, int *bpp, int *w, int *h, int *tiled)
{
	u32int cntr, src;
	int p;

	for(p = 0; p < 3; p++){
		cntr = rd(Dspcntr + p*0x1000);
		if((cntr & 1<<31) == 0)
			continue;
		switch(cntr>>26 & 0xF){
		case 5:		*bpp = 16; break;
		case 6: case 7:	*bpp = 32; break;
		default:	*bpp = 0; break;
		}
		src = rd(Pipesrc + p*0x1000);
		*surf = rd(Dspsurf + p*0x1000) & ~0xFFF;
		*stride = rd(Dspstride + p*0x1000);
		*w = (src>>16 & 0xFFF)+1;
		*h = (src & 0xFFF)+1;
		*tiled = (cntr & 1<<10) != 0;
		return p;
	}
	return -1;
}

/*
 * map plane: the visible plane's memory into the context (uncached,
 * the display engine reads it from memory), page by page from its GTT
 * entries; with -G it is in the GTT already
 */
char*
mapplane(Ctx *c)
{
	ulong surf, stride, size, off;
	int p, bpp, w, h, tiled, i;
	uvlong pa;
	Bo *b;

	if((p = plane(&surf, &stride, &bpp, &w, &h, &tiled)) < 0)
		return "no plane";
	size = (stride*h + 4095) & ~4095;
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
	b->plane = 1;
	qlock(&lk);
	if(global)
		b->gtt = surf;
	else if((b->gtt = holealloc(&c->holes, size)) != 0)
		for(off = 0; off < size; off += 4096){
			pa = gtt[(surf + off)/4096] & 0x7FFFFFF000ULL;
			if(ppmap(c, b->gtt + off, pa, nil, 4096, Pteuncached) < 0){
				ppunmap(c, b->gtt, off);
				holefree(&c->holes, b->gtt, size);
				b->gtt = 0;
				break;
			}
		}
	qunlock(&lk);
	if(b->gtt == 0){
		free(b);
		return "no GPU address space (or page tables) left";
	}
	bos[i] = b;
	return ctxreply(c, "plane %d %#llux %lud %d %d %d%s\n", b->id, b->gtt, stride, bpp, w, h,
		tiled ? " tiled" : "");
}

/* the plane as it was (aux/vga's draw screen); with lk */
void
scanrestore(void)
{
	int o;

	if(scan.bo == nil)
		return;
	o = scan.p*0x1000;
	wr(Dspcntr+o, scan.cntr);
	wr(Dspstride+o, scan.stride);
	wr(Dspoffset+o, scan.offset);
	wr(Dspsurf+o, scan.surf);
	scan.bo = nil;
}

/*
 * ctx scanout: object id (linear x8r8g8b8, the mode's size, stride
 * bytes a row) is what the display shows, until scanout 0, its free or
 * the context's end; each scanout is a new picture (a swapchain's
 * present), taken at the next vertical blank. The display reads
 * memory, not the LLC: the object is mapped write-through for the GPU
 * from its first scanout on (i915 has display buffers write-through
 * or uncached), and into the GTT for the display.
 */
char*
scanout(Ctx *c, int id, int w, int h, int stride)
{
	ulong surf, pstride, a;
	int p, bpp, pw, ph, tiled, o;
	Bo *b;

	if(id == 0){
		qlock(&lk);
		if(scan.bo != nil && scan.bo->ctx == c)
			scanrestore();
		qunlock(&lk);
		return nil;
	}
	if((b = findbo(c, id)) == nil || b->plane)
		return "unknown object";
	if(scan.bo != nil && scan.bo->ctx != c)
		return "scanout in use";
	if((p = plane(&surf, &pstride, &bpp, &pw, &ph, &tiled)) < 0)
		return "no plane";
	if(w != pw || h != ph)
		return "not the mode's size";
	if(stride < w*4 || stride % 64 != 0 || (uvlong)stride*h > b->size)
		return "bad stride";
	if(!b->scanwt && !global && b->gtt != 0){
		if(c->nfence > c->donefence)
			fencewait(c, c->nfence);
		qlock(&lk);
		ppmap(c, b->gtt, 0, b->pa, b->size, uncached ? Pteuncached : Ptewt);
		b->scanwt = 1;
		qunlock(&lk);
	}
	qlock(&lk);
	if(b->scanbase == 0){
		if(global)
			a = b->gtt;
		else{
			if((a = gttalloc(b->size + Scanalign)) == 0){
				qunlock(&lk);
				return "no GTT space for the scanout";
			}
			b->scanbase = a;
			a = (a + Scanalign-1) & ~(Scanalign-1);
			gttmap(a, 0, b->pa, b->size);
		}
		b->scangtt = a;
	}
	o = p*0x1000;
	if(scan.bo == nil){
		scan.p = p;
		scan.cntr = rd(Dspcntr+o);
		scan.stride = rd(Dspstride+o);
		scan.offset = rd(Dspoffset+o);
		scan.surf = rd(Dspsurf+o);
	}
	wr(Dspcntr+o, (scan.cntr & ~(Dspfmtmask|Dsptiled)) | Dspbgrx);
	wr(Dspstride+o, stride);
	wr(Dspoffset+o, 0);
	wr(Dspsurf+o, b->scangtt);	/* last: the flip, at the next vertical blank */
	scan.bo = b;
	scan.flip = b->scangtt;
	qunlock(&lk);
	return nil;
}

/*
 * after a scanout: wait until the display shows the new surface (it
 * latches DSPSURF at the vertical blank), so that the picture shown
 * before is free when the request returns: a swapchain's FIFO, no
 * tearing, the frame rate the display's; at most 50 ms
 */
void
flipwait(Req *r, ulong surf, int p)
{
	vlong t0;

	srvrelease(r->srv);
	for(t0 = nsec(); nsec() - t0 < 50*1000000LL; sleep(1))
		if((rd(Dspsurflive + p*0x1000) & ~0xFFF) == surf)
			break;
	srvacquire(r->srv);
}
