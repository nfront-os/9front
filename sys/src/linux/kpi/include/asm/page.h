/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: asm/page.h: the x86-64 page sizes (linux/mm.h has the rest) */
#ifndef _KPI_ASM_PAGE_H
#define _KPI_ASM_PAGE_H

#include <machine/param.h>

#define PFN_DOWN(x)		((x) >> PAGE_SHIFT)
#define PFN_UP(x)		(((x) + PAGE_SIZE - 1) >> PAGE_SHIFT)
#define PFN_PHYS(x)		((phys_addr_t)(x) << PAGE_SHIFT)
#define HPAGE_SHIFT		21
#define HPAGE_SIZE		(1UL << HPAGE_SHIFT)
#define HPAGE_PMD_SIZE		HPAGE_SIZE
#define HPAGE_PMD_ORDER		(HPAGE_SHIFT - PAGE_SHIFT)
#define MAX_ORDER		10	/* the largest alloc_pages order: 4 MB */

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/sizes.h>
#include <linux/kernel.h>
#include <linux/mm_types.h>
#include <linux/log2.h>
#define PAGE_ALIGNED(a)		IS_ALIGNED((unsigned long)(a), PAGE_SIZE)
#define PHYS_PFN(x)		((unsigned long)((x) >> PAGE_SHIFT))
struct page	*kpi_virt_to_page(const void *p);
void	*kpi_page_address(const struct page *p);
#define page_address(p)		kpi_page_address(p)
struct page	*kpi_pfn_to_page(unsigned long pfn);
struct page	*kpi_nth_page(struct page *p, unsigned long n);	/* nth_page */
int	page_is_ram(unsigned long pfn);	/* kpi/linux/page.c */
#define virt_to_phys(p)		page_to_phys(virt_to_page(p))
#define __pa(p)			virt_to_phys(p)
#define compound_head(p)	((p)->kpi_head ? (p)->kpi_head : (p))
#define page_count(p)		atomic_read(&compound_head(p)->_refcount)
#define get_page(p)		atomic_inc(&compound_head(p)->_refcount)
#define page_ref_count(p)	page_count(p)
#define page_folio(p)		((struct folio *)compound_head(p))
#define virt_to_folio(p)	page_folio(virt_to_page(p))
#define folio_file_page(f, i)	(&(f)->page + ((i) & ((1UL << (f)->page.order) - 1)))
#define folio_page(f, n)	(&(f)->page + (n))
#define folio_nr_pages(f)	(1L << (f)->page.order)
#define folio_put(f)		put_page(&(f)->page)
#define folio_get(f)		get_page(&(f)->page)
#define folio_address(f)	page_address(&(f)->page)
#define folio_size(f)		(PAGE_SIZE << (f)->page.order)
#define folio_pfn(f)		page_to_pfn(&(f)->page)
#define compound_order(p)	(compound_head(p)->order)
#define page_size(p)		(PAGE_SIZE << (p)->order)
#define folio_mark_dirty(f)	((void)(f), 1)
#define folio_mark_accessed(f)	do { } while (0)
#define SetPageReclaim(p)	do { } while (0)
#define ClearPageReclaim(p)	do { } while (0)
#define set_page_dirty_lock(p)	do { } while (0)
#define mark_page_accessed(p)	do { } while (0)
#define unlock_page(p)		do { } while (0)
#define lock_page(p)		do { } while (0)
#define PageWriteback(p)	0
#define PageSwapCache(p)	0
#define PageDirty(p)		0
#define SetPageDirty(p)		do { } while (0)
#define ClearPageDirty(p)	do { } while (0)
#define page_to_nid(p)		0
#define page_mapped(p)		(atomic_read(&(p)->_mapcount) > 0)
#define page_mapcount(p)	atomic_read(&(p)->_mapcount)

#endif
