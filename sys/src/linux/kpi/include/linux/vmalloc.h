/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/vmalloc.h (ours, not Linux's) */
#ifndef _KPI_LINUX_VMALLOC_H
#define _KPI_LINUX_VMALLOC_H

#include <linux/kernel.h>
#include <linux/mm_types.h>

void	*vmalloc(unsigned long size);
void	*vzalloc(unsigned long size);
void	vfree(const void *p);
void	*vmap(struct page **pages, unsigned int count, unsigned long flags, pgprot_t prot);
/* kpi/linux/page.c: an alias segment (segment(3)) named name of pool pages; attached here if attach */
void	*kpi_alias_pages(const char *name, struct page **pages, unsigned int count, bool attach);
void	vunmap(const void *p);
void	*vmap_pfn(unsigned long *pfns, unsigned int count, pgprot_t prot);
#define vm_map_ram(pages, n, node)	vmap(pages, n, 0, PAGE_KERNEL)
#define vm_unmap_ram(p, n)		vunmap(p)
#define vmalloc_32(s)		vmalloc(s)
#define vmalloc_user(s)		vzalloc(s)
#define vcalloc(n, s)		vzalloc((n) * (s))
#define is_vmalloc_addr(p)	0
#define vmalloc_to_page(p)	kpi_virt_to_page(p)
#define register_vmap_purge_notifier(n)	0
#define unregister_vmap_purge_notifier(n)	0
#define VM_MAP			0x00000004
#define VM_MAP_PUT_PAGES	0x00000200
#define VM_IOREMAP		0x00000001

#endif
