/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: a context's address space: 48-bit four-level (or 32-bit three-level) PPGTT.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/gt/gen8_ppgtt.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

u32int *ptpool;		/* Ptpages pages */
uvlong ptpa;
uchar ptused[Ptpages];
ulong serial;

/*
 * contexts' address spaces: page directories and tables are pages of
 * the pool, written cached (the GPU walks them through the LLC, as
 * i915 has it where there is an LLC), flushed with -U
 */
int
ptalloc(uvlong fill)
{
	uvlong *p;
	int i, j;

	for(i = 0; i < Ptpages; i++)
		if(!ptused[i]){
			ptused[i] = 1;
			p = (uvlong*)(ptpool + i*1024);
			for(j = 0; j < 512; j++)
				p[j] = fill;
			flushcache(p, 4096);
			return i;
		}
	return -1;
}

void
ptfree(int i)
{
	if(i >= 0)
		ptused[i] = 0;
}

uvlong
ptphys(int i)
{
	return ptpa + (uvlong)i*4096;
}

uvlong*
ptmem(int i)
{
	return (uvlong*)(ptpool + i*1024);
}

/* the pool page of a table's physical address, -1 for a scratch table */
int
ptindex(uvlong pa)
{
	pa &= Ptaddr;
	if(pa < ptpa || pa >= ptpa + Ptpages*4096ULL)
		return -1;
	return (pa - ptpa)/4096;
}

/*
 * the entry for address a at a level of the context's tree (3: in the
 * PML4, 0: the PTE), with alloc the tables on the way allocated
 */
uvlong*
ppwalk(Ctx *c, uvlong a, int level, int alloc)
{
	uvlong *t;
	int lvl, i, n;

	t = ptmem(c->pml4);
	for(lvl = 3; lvl > level; lvl--){
		i = a >> (12 + 9*lvl) & 511;
		if((n = ptindex(t[i])) < 0){
			if(!alloc)
				return nil;
			if((n = ptalloc(scratchfill[lvl-1])) < 0)
				return nil;
			t[i] = ptphys(n) | GEN8_PAGE_PRESENT | GEN8_PAGE_RW | (uncached ? PPAT_UNCACHED : Pdecached);
			flushcache(&t[i], 8);
		}
		t = ptmem(n);
	}
	return &t[a >> (12 + 9*level) & 511];
}

/* free a table and the tables below it */
void
pptfree(int n, int level)
{
	uvlong *t;
	int i, m;

	if(level > 0){
		t = ptmem(n);
		for(i = 0; i < 512; i++)
			if((m = ptindex(t[i])) >= 0)
				pptfree(m, level-1);
	}
	ptfree(n);
}

/* the PDP register n of a context: with 48 bits the PML4 in PDP0 */
uvlong
pdpval(Ctx *c, int n)
{
	uvlong *e;

	if(global || c->pml4 < 0)
		return 0;
	if(!legacy32)
		return n == 0 ? ptphys(c->pml4) : 0;
	e = ppwalk(c, (uvlong)n<<30, 2, 0);
	return e != nil ? *e & Ptaddr : 0;
}

int
ppinit(Ctx *c)
{
	int n;

	c->holes = nil;
	if((c->pml4 = ptalloc(scratchfill[3])) < 0)
		return -1;
	/* -3: the four page directories exist from the start (they are registers) */
	if(legacy32)
		for(n = 0; n < 4; n++)
			if(ppwalk(c, (uvlong)n<<30, 1, 1) == nil){
				ppfree(c);
				return -1;
			}
	c->holes = emalloc9p(sizeof(Hole));
	c->holes->start = Ppbase;
	c->holes->len = (legacy32 ? Pplimit32 : Pplimit48) - Ppbase;
	c->holes->next = nil;
	/*
	 * the null render state's page: its batch runs from the GTT, but
	 * its state (STATE_BASE_ADDRESS, relocated to Rsva) is read through
	 * the context's PPGTT
	 */
	if(ppmap(c, Rsva, rspa, nil, 4096, uncached ? PPAT_UNCACHED : PPAT_CACHED) < 0){
		ppfree(c);
		return -1;
	}
	return 0;
}

void
ppfree(Ctx *c)
{
	Hole *h;

	if(c->pml4 >= 0)
		pptfree(c->pml4, 3);
	c->pml4 = -1;
	while((h = c->holes) != nil){
		c->holes = h->next;
		free(h);
	}
}

/*
 * map len bytes to the context's address a, with the PPAT bits:
 * the pages pas[], or contiguous memory at pa if pas is nil
 */
int
ppmap(Ctx *c, uvlong a, uvlong pa, uvlong *pas, uvlong len, uvlong bits)
{
	uvlong *pte, v, end;

	end = a + ((len + 4095) & ~4095ULL);
	for(v = a; v < end; v += 4096, pa += 4096){
		if(pas != nil)
			pa = pas[(v - a)/4096];
		if((pte = ppwalk(c, v, 0, 1)) == nil)
			return -1;
		*pte = pa | GEN8_PAGE_PRESENT | GEN8_PAGE_RW | bits;
		flushcache(pte, 8);
	}
	c->serial = ++serial;	/* the engines invalidate their TLBs before its next batch */
	return 0;
}

void
ppunmap(Ctx *c, uvlong a, uvlong len)
{
	uvlong *pte, v, end;

	end = a + ((len + 4095) & ~4095ULL);
	for(v = a; v < end; v += 4096){
		if((pte = ppwalk(c, v, 0, 0)) == nil)
			continue;
		*pte = scratchpte;
		flushcache(pte, 8);
	}
	c->serial = ++serial;
}
