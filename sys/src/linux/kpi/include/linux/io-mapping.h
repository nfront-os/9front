/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/io-mapping.h (ours, not Linux's) */
#ifndef _KPI_LINUX_IO_MAPPING_H
#define _KPI_LINUX_IO_MAPPING_H

#include <linux/io.h>
#include <linux/mm.h>

/* a write-combined mapping of a range of a BAR (ours: Linux's is GPL) */
struct io_mapping { resource_size_t base; unsigned long size; pgprot_t prot; void __iomem *iomem; };
struct io_mapping	*io_mapping_init_wc(struct io_mapping *m, resource_size_t base, unsigned long size);
void	io_mapping_fini(struct io_mapping *m);
struct io_mapping	*io_mapping_create_wc(resource_size_t base, unsigned long size);
void	io_mapping_free(struct io_mapping *m);
static inline void __iomem *io_mapping_map_wc(struct io_mapping *m, unsigned long off, unsigned long size) { return (char __iomem *)m->iomem + off; }
static inline void __iomem *io_mapping_map_atomic_wc(struct io_mapping *m, unsigned long off) { return (char __iomem *)m->iomem + off; }
static inline void __iomem *io_mapping_map_local_wc(struct io_mapping *m, unsigned long off) { return (char __iomem *)m->iomem + off; }
#define io_mapping_unmap(a)		do { (void)(a); } while (0)
#define io_mapping_unmap_atomic(a)	do { (void)(a); } while (0)
#define io_mapping_unmap_local(a)	do { (void)(a); } while (0)
int	io_mapping_map_user(struct io_mapping *m, struct vm_area_struct *v, unsigned long addr, unsigned long pfn, unsigned long size);

#endif
