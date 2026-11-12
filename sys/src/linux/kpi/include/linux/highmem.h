/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/highmem.h (ours, not Linux's) */
#ifndef _KPI_LINUX_HIGHMEM_H
#define _KPI_LINUX_HIGHMEM_H

#include <linux/mm.h>
#include <linux/string.h>

#define kmap(p)			page_address(p)
#define kunmap(p)		do { (void)(p); } while (0)
#define kmap_atomic(p)		page_address(p)
#define kunmap_atomic(a)	do { (void)(a); } while (0)
#define kmap_local_page(p)	page_address(p)
#define kmap_local_folio(f, o)	((void *)((char *)folio_address(f) + (o)))
#define kmap_local_page_prot(p, pr)	page_address(p)
#define kunmap_local(a)		do { (void)(a); } while (0)
#define kmap_local_page_try_from_panic(p)	page_address(p)
static inline void clear_page(void *p) { memset(p, 0, PAGE_SIZE); }
static inline void copy_page(void *d, const void *s) { memcpy(d, s, PAGE_SIZE); }
static inline void copy_highpage(struct page *t, struct page *f) { copy_page(page_address(t), page_address(f)); }
static inline void clear_highpage(struct page *p) { clear_page(page_address(p)); }
static inline void memcpy_from_page(char *to, struct page *p, size_t off, size_t len) { memcpy(to, (char *)page_address(p) + off, len); }
static inline void memcpy_to_page(struct page *p, size_t off, const char *from, size_t len) { memcpy((char *)page_address(p) + off, from, len); }
static inline void memzero_page(struct page *p, size_t off, size_t len) { memset((char *)page_address(p) + off, 0, len); }

#endif
