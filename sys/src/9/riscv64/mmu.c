#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "riscv.h"

/*
 * Sv39. The kernel half of every root table is that of L1 (made by
 * l.s, completed by meminit): KZERO maps physical memory one to one
 * with gigapages, VMAP has a shared level 1 table. The user half
 * is filled from the current process' tables by mmuswitch, as on
 * arm64; processes have address space ids.
 */

uintptr rebootpa;		/* a page for the reboot code */

#define PTEKRW	(PTEVALID|PTEREAD|PTEWRITE|PTEKERNEL)
#define PTEKRWX	(PTEKRW|PTEEXEC)

void
mmu1init(void)
{
	m->mmutop = mallocalign(BY2PG, BY2PG, 0, 0);
	if(m->mmutop == nil)
		panic("mmu1init: no memory for mmutop");
	memmove(m->mmutop, (void*)L1, BY2PG);
	mmuswitch(nil, 0);
}

uintptr
paddr(void *va)
{
	if((uintptr)va >= KZERO && (uintptr)va < KLIMIT)
		return (uintptr)va-KZERO;
	panic("paddr: va=%#p pc=%#p", va, getcallerpc(&va));
}

uintptr
cankaddr(uintptr pa)
{
	if(pa < KLIMIT-KZERO)
		return KLIMIT-KZERO - pa;
	return 0;
}

void*
kaddr(uintptr pa)
{
	if(pa < KLIMIT-KZERO)
		return (void*)(pa + KZERO);
	panic("kaddr: pa=%#p pc=%#p", pa, getcallerpc(&pa));
}

KMap*
kmap(Page *p)
{
	return kaddr(p->pa);
}

void
kunmap(KMap*)
{
}

void
kmapinval(void)
{
}

static void*
rampage(void)
{
	uintptr pa;

	if(conf.npage)
		return mallocalign(BY2PG, BY2PG, 0, 0);

	pa = conf.mem[0].base;
	assert((pa % BY2PG) == 0);
	conf.mem[0].base += BY2PG;
	return KADDR(pa);
}

/*
 * map physical memory [base, limit) at KZERO+, gigapages,
 * in the kernel's root table
 */
void
kmapram(uintptr base, uintptr limit)
{
	uintptr *l1, pa;

	l1 = (uintptr*)L1;
	for(pa = base & -PGLSZ(2); pa < limit && pa < KLIMIT-KZERO; pa += PGLSZ(2))
		l1[PTLX(KZERO+pa, 2)] = PA2PTE(pa) | PTEKRWX;
	flushtlb();
}

void
meminit(void)
{
	char *p;
	uintptr limit;

	limit = PHYSDRAM + 128*MiB;	/* qemu's default */
	if(p = getconf("*maxmem"))
		limit = strtoull(p, 0, 0);
	if(limit > KLIMIT-KZERO)
		limit = KLIMIT-KZERO;
	kmapram(PHYSDRAM, limit);

	/* the top page is kept for the reboot code */
	limit -= BY2PG;
	rebootpa = limit;

	conf.mem[0].base = PADDR(KMEMBASE);
	conf.mem[0].limit = limit;
	conf.mem[0].npage = (conf.mem[0].limit - conf.mem[0].base)/BY2PG;
}

/*
 * device registers: the first GB of the physical address space
 * is at KZERO (l.s); anything else gets 4K pages in VMAP
 */
void*
vmap(uvlong pa, vlong size)
{
	static Lock lk;
	static uintptr base = VMAP;
	uintptr va, va0, off, *l1, *l0;

	if(pa+size <= GiB)
		return (void*)(KZERO+pa);

	off = pa & BY2PG-1;
	size = PGROUND(off+size);
	pa -= off;
	lock(&lk);
	va = va0 = base;
	if(va + size > VMAP+VMAPSIZE){
		unlock(&lk);
		return nil;
	}
	base += size;
	l1 = (uintptr*)VMAPL1;
	for(; size > 0; size -= BY2PG, pa += BY2PG, va += BY2PG){
		if((l1[PTLX(va, 1)] & PTEVALID) == 0){
			l0 = rampage();
			memset(l0, 0, BY2PG);
			l1[PTLX(va, 1)] = PA2PTE(PADDR(l0)) | PTEVALID;
		}
		l0 = KADDR(PTE2PA(l1[PTLX(va, 1)]));
		l0[PTLX(va, 0)] = PA2PTE(pa) | PTEKRW;
	}
	unlock(&lk);
	flushtlb();
	return (void*)(va0 + off);
}

void
vunmap(void *, vlong)
{
}

static uintptr*
mmuwalk(uintptr va, int level)
{
	uintptr *table, pte;
	Page *pg;
	int i, x;

	x = PTLX(va, PTLEVELS-1);
	table = m->mmutop;
	for(i = PTLEVELS-2; i >= level; i--){
		pte = table[x];
		if(pte & PTEVALID)
			pte = PTE2PA(pte);
		else {
			pg = up->mmufree;
			if(pg == nil)
				return nil;
			up->mmufree = pg->next;
			pg->va = va & -PGLSZ(i+1);
			if((pg->next = up->mmuhead[i+1]) == nil)
				up->mmutail[i+1] = pg;
			up->mmuhead[i+1] = pg;
			pte = pg->pa;
			memset(KADDR(pte), 0, BY2PG);
			coherence();
			table[x] = PA2PTE(pte) | PTEVALID;
		}
		table = KADDR(pte);
		x = PTLX(va, (uintptr)i);
	}
	return &table[x];
}

static Proc *asidlist[256];

static int
allocasid(Proc *p)
{
	static Lock lk;
	Proc *x;
	int a;

	lock(&lk);
	a = p->asid;
	if(a < 0)
		a = -a;
	if(a == 0)
		a = p->pid;
	for(;; a++){
		a %= nelem(asidlist);
		if(a == 0)
			continue;	// reserved
		x = asidlist[a];
		if(x == p || x == nil || (x->asid < 0 && x->mach == nil))
			break;
	}
	p->asid = a;
	asidlist[a] = p;
	unlock(&lk);

	return x != p;
}

static void
freeasid(Proc *p)
{
	int a;

	a = p->asid;
	if(a < 0)
		a = -a;
	if(a > 0 && asidlist[a] == p)
		asidlist[a] = nil;
	p->asid = 0;
}

void
putasid(Proc *p)
{
	/* see arm64/mmu.c */
	if(conf.nmach > 1)
		mmuswitch(nil, 0);

	if(p->asid > 0)
		p->asid = -p->asid;
}

void
putmmu(uintptr va, uintptr pa, Page *pg)
{
	uintptr *pte, old, attr;
	int s;

	s = splhi();
	while((pte = mmuwalk(va, 0)) == nil){
		spllo();
		up->mmufree = newpage(0, nil);
		splhi();
	}
	attr = PTEVALID | PTEREAD | PTEUSER | PTEACCESS | PTEDIRTY;
	if(pa & PTEWRITE)
		attr |= PTEWRITE;
	if((pa & PTENOEXEC) == 0)
		attr |= PTEEXEC;
	old = *pte;
	*pte = PA2PTE(PPN(pa)) | attr;
	coherence();
	if(old & PTEVALID){
		if(soc.cip1200)	/* CIP-1200 (SiFive's errata): flush everything */
			flushtlb();
		else
			flushtlbva(va);
	}
	if(needtxtflush(pg)){
		fencei();
		donetxtflush(pg);
	}
	splx(s);
}

static void
mmufree(Proc *p)
{
	int i;

	freeasid(p);

	for(i=1; i<PTLEVELS; i++){
		if(p->mmuhead[i] == nil)
			break;
		p->mmutail[i]->next = p->mmufree;
		p->mmufree = p->mmuhead[i];
		p->mmuhead[i] = p->mmutail[i] = nil;
	}
}

void
mmuswitch(Proc *p, int newtlb)
{
	uintptr va;
	Page *t;

	for(va = UZERO; va < USTKTOP; va += PGLSZ(PTLEVELS-1))
		m->mmutop[PTLX(va, PTLEVELS-1)] = 0;

	if(p == nil){
		setsatp(Sv39 | PADDR(m->mmutop)>>PGSHIFT);
		return;
	}

	if(newtlb)
		mmufree(p);

	for(t = p->mmuhead[PTLEVELS-1]; t != nil; t = t->next){
		va = t->va;
		m->mmutop[PTLX(va, PTLEVELS-1)] = PA2PTE(t->pa) | PTEVALID;
	}

	allocasid(p);
	/* setsatp flushes all of this cpu's tlb */
	setsatp(Sv39 | (uvlong)p->asid<<Satpasidshft | PADDR(m->mmutop)>>PGSHIFT);
}

void
mmurelease(Proc *p)
{
	mmuswitch(nil, 0);
	mmufree(p);
	freepages(p->mmufree, nil, 0);
	p->mmufree = nil;
}

void
checkmmu(uintptr, uintptr)
{
}
