/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: the global GTT: its free ranges, map, unmap.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/gt/intel_ggtt.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

uvlong *gtt;
uvlong nent;
Hole *holes;

/*
 * the GTT: our part from Gttlow, first fit
 */
uvlong
holealloc(Hole **list, uvlong len)
{
	Hole *h, **l;
	uvlong a;

	len = (len + 4095) & ~4095ULL;
	for(l = list; (h = *l) != nil; l = &h->next)
		if(h->len >= len){
			a = h->start;
			h->start += len;
			h->len -= len;
			if(h->len == 0){
				*l = h->next;
				free(h);
			}
			return a;
		}
	return 0;
}

/* take [a, a+len) out of the holes; -1 if not all free */
int
holetake(Hole **list, uvlong a, uvlong len)
{
	Hole *h, *n, **l;

	len = (len + 4095) & ~4095ULL;
	for(l = list; (h = *l) != nil; l = &h->next){
		if(a < h->start || a + len > h->start + h->len)
			continue;
		if(a + len < h->start + h->len){
			n = emalloc9p(sizeof *n);
			n->start = a + len;
			n->len = h->start + h->len - (a + len);
			n->next = h->next;
			h->next = n;
		}
		h->len = a - h->start;
		if(h->len == 0){
			*l = h->next;
			free(h);
		}
		return 0;
	}
	return -1;
}

void
holefree(Hole **list, uvlong a, uvlong len)
{
	Hole *h, *p, **l;

	len = (len + 4095) & ~4095ULL;
	for(l = list; (p = *l) != nil && p->start < a; l = &p->next)
		;
	h = emalloc9p(sizeof *h);
	h->start = a;
	h->len = len;
	h->next = p;
	*l = h;
	/* merge with the next, and the previous with it */
	if(p != nil && a + len == p->start){
		h->len += p->len;
		h->next = p->next;
		free(p);
	}
	for(p = *list; p != nil && p->next != h; p = p->next)
		;
	if(p != nil && p->start + p->len == h->start){
		p->len += h->len;
		p->next = h->next;
		free(h);
	}
}

ulong
gttalloc(ulong len)
{
	return holealloc(&holes, len);
}

void
gttfree(ulong a, ulong len)
{
	holefree(&holes, a, len);
}

/*
 * map len bytes to gtt address a: the pages pas[], or
 * contiguous memory at pa if pas is nil
 */
void
gttmap(ulong a, uvlong pa, uvlong *pas, ulong len)
{
	uvlong i, n, bits;

	bits = GEN8_PAGE_PRESENT | (uncached ? PPAT_UNCACHED : PPAT_CACHED);
	n = (len + 4095) / 4096;
	for(i = 0; i < n; i++)
		gtt[a/4096 + i] = (pas != nil ? pas[i] : pa + i*4096) | bits;
	wr(GFX_FLSH_CNTL_GEN6, 1);
	rd(GFX_FLSH_CNTL_GEN6);
}

void
gttunmap(ulong a, ulong len)
{
	uvlong i, n;

	n = (len + 4095) / 4096;
	for(i = 0; i < n; i++)
		gtt[a/4096 + i] = scratchgtt;
	wr(GFX_FLSH_CNTL_GEN6, 1);
	rd(GFX_FLSH_CNTL_GEN6);
}
