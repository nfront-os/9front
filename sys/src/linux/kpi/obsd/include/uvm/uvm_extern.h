/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, uvm(9) and pmap(9), as far
 * as OpenBSD's LinuxKPI headers use them.  The pages are Linux's struct
 * page (OpenBSD's struct vm_page): pages of pinned #g segments, from the
 * page pool (kpi/linux/page.c), each with its physical address and
 * where the file server sees it; heap memory has pages too (made on
 * demand, virt_to_page).  km_alloc(9) takes pages of the pool.
 */
#ifndef _OBSD_UVM_UVM_EXTERN_H
#define _OBSD_UVM_UVM_EXTERN_H

#include <sys/types.h>
#include <sys/queue.h>
#include <machine/param.h>
#include <machine/pte.h>
#include <linux/types.h>	/* struct list_head */

struct address_space;

struct page {
	unsigned long	flags;		/* Linux's page flags */
	unsigned int	pg_flags;	/* OpenBSD's: PG_PMAP_WC */
	void		*virtual;	/* where the file server sees it */
	paddr_t		phys_addr;	/* where the device does */
	unsigned long	private;
	struct list_head	lru;
	struct { volatile int counter; } _refcount;	/* atomic_t */
	struct { volatile int counter; } _mapcount;
	struct address_space	*mapping;
	unsigned long	index;
	unsigned int	order;
	struct page	*kpi_head;	/* a tail's head page (higher orders) */
	struct page	*kpi_hnext;	/* in the pfn hash */
	void		*kpi_chunk;	/* the pool's chunk it is in */
	TAILQ_ENTRY(page) pageq;	/* on a struct pglist */
};
TAILQ_HEAD(pglist, page);

#define PG_CLEAN	0x00000008	/* not written since read from its store */
#define PG_PMAP_WC	0x01000000

#define VM_PAGE_TO_PHYS(pg)	((pg)->phys_addr)
struct page	*PHYS_TO_VM_PAGE(paddr_t);	/* NULL: not a page of ours */
struct page	*uvm_atopg(vaddr_t);		/* the page at a virtual address of the file server */

#define PROT_NONE	0x00
#define PROT_READ	0x01
#define PROT_WRITE	0x02
#define PROT_EXEC	0x04
#define PROT_MASK	(PROT_READ | PROT_WRITE | PROT_EXEC)

#define PMAP_NOCACHE	0x1
#define PMAP_WC		0x2
#define PMAP_DEVICE	0x4
#define PMAP_WIRED	0x10

/* the file server's own address space: one, with user addresses only */
#define VM_MIN_ADDRESS		0UL
#define VM_MAXUSER_ADDRESS	0x00007f8000000000UL
#define VM_MIN_KERNEL_ADDRESS	VM_MIN_ADDRESS
#define VM_MAX_KERNEL_ADDRESS	VM_MAXUSER_ADDRESS

#define UVM_PLA_WAITOK	0x0001
#define UVM_PLA_NOWAIT	0x0002
#define UVM_PLA_ZERO	0x0004
#define UVM_PLA_FAILOK	0x0010

/* km_alloc(9): its modes only say wait or not, zero or not */
struct kmem_va_mode	{ int kv_wait; };
struct kmem_pa_mode	{ int kp_zero; };
struct kmem_dyn_mode	{ int kd_waitok; };
extern const struct kmem_va_mode	kv_any, kv_page;
extern const struct kmem_pa_mode	kp_none, kp_zero, kp_dirty;
extern const struct kmem_dyn_mode	kd_waitok, kd_nowait;
void	*km_alloc(size_t, const struct kmem_va_mode *, const struct kmem_pa_mode *, const struct kmem_dyn_mode *);
void	km_free(void *, size_t, const struct kmem_va_mode *, const struct kmem_pa_mode *);

/* clflush over a range of the file server's memory */
void	pmap_flush_cache(vaddr_t, vsize_t);
#define pmap_update(pm)	((void)0)
struct pmap;
#define pmap_kernel()	((struct pmap *)0)

int	uvm_pglistalloc(psize_t, paddr_t, paddr_t, paddr_t, paddr_t, struct pglist *, int, int);
void	uvm_pglistfree(struct pglist *);

extern unsigned long	physmem;	/* pages of the machine */

/* the machine's memory in pages (p9_meminfo), as uvm's counters */
struct uvmexp {
	int	npages;
	int	free;
	int	pagesize;
	int	swpages;
	int	swpginuse;
};
extern struct uvmexp	uvmexp;

#endif
