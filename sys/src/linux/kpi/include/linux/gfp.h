/* Public domain. */

#ifndef _LINUX_GFP_H
#define _LINUX_GFP_H

#include <sys/types.h>
#include <sys/malloc.h>
#include <uvm/uvm_extern.h>

#include <linux/mmzone.h>
#include <linux/topology.h>	/* 9front: as Linux's gfp.h */

#define __GFP_ZERO		M_ZERO
#define __GFP_DMA32		0x00010000
#define __GFP_NOWARN		0
#define __GFP_NORETRY		0
#define __GFP_RETRY_MAYFAIL	0
#define __GFP_MOVABLE		0
#define __GFP_COMP		0
#define __GFP_KSWAPD_RECLAIM	M_NOWAIT
#define __GFP_HIGHMEM		0
#define __GFP_RECLAIMABLE	0
#define __GFP_NOMEMALLOC	0
#define __GFP_HIGH		0
#define __GFP_ACCOUNT		0

#define GFP_ATOMIC		M_NOWAIT
#define GFP_NOWAIT		M_NOWAIT
#define GFP_KERNEL		(M_WAITOK | M_CANFAIL)
#define GFP_KERNEL_ACCOUNT	(M_WAITOK | M_CANFAIL)
#define GFP_USER		(M_WAITOK | M_CANFAIL)
#define GFP_HIGHUSER		0
#define GFP_DMA32		__GFP_DMA32
#define GFP_TRANSHUGE_LIGHT	0
#define GFP_NOFS		(M_WAITOK | M_CANFAIL)

static inline bool
gfpflags_allow_blocking(const unsigned int flags)
{
	return (flags & M_WAITOK) != 0;
}

struct page *alloc_pages(unsigned int, unsigned int);
void	__free_pages(struct page *, unsigned int);

static inline struct page *
alloc_page(unsigned int gfp_mask)
{
	return alloc_pages(gfp_mask, 0);
}

static inline void
__free_page(struct page *page)
{
	__free_pages(page, 0);
}

static inline unsigned long
__get_free_page(unsigned int gfp_mask)
{
	void *addr = km_alloc(PAGE_SIZE, &kv_page, &kp_dirty, &kd_nowait);
	return (unsigned long)addr;
}

static inline void
free_page(unsigned long addr)
{
	km_free((void *)addr, PAGE_SIZE, &kv_page, &kp_dirty);
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/numa.h>
#define __GFP_DMA		0x01u
#define __GFP_HIGH		0x20u
#define __GFP_IO		0x40u
#define __GFP_FS		0x80u
#define __GFP_NOFAIL		0x800u
#define __GFP_DIRECT_RECLAIM	0x10000u
#define __GFP_RECLAIM		(__GFP_DIRECT_RECLAIM | __GFP_KSWAPD_RECLAIM)
#define __GFP_ACCOUNT		0x80000u
#define __GFP_THISNODE		0x100000u
#define __GFP_HARDWALL		0x200000u
#define GFP_KERNEL_ACCOUNT	(GFP_KERNEL | __GFP_ACCOUNT)
#define GFP_NOIO		(__GFP_RECLAIM)
#define GFP_NOFS		(__GFP_RECLAIM | __GFP_IO)
#define GFP_DMA			__GFP_DMA
#define GFP_HIGHUSER_MOVABLE	(GFP_HIGHUSER | __GFP_MOVABLE)
#define GFP_TRANSHUGE		(GFP_TRANSHUGE_LIGHT | __GFP_DIRECT_RECLAIM)
#define current_gfp_context(g)	(g)
unsigned long	__get_free_pages(gfp_t gfp, unsigned int order);
unsigned long	get_zeroed_page(gfp_t gfp);
void	free_pages(unsigned long addr, unsigned int order);
#define alloc_pages_node(n, g, o)	alloc_pages(g, o)
struct folio	*folio_alloc(gfp_t gfp, unsigned int order);
void	put_page(struct page *p);

/* 9front: Linux 7's allocation macros: the flags an optional argument, GFP_KERNEL without */
#define __default_gfp(a, b, ...)	b
#define default_gfp(...)	__default_gfp(, ##__VA_ARGS__, GFP_KERNEL)

#endif
