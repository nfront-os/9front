/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/dma-mapping.h (ours, not Linux's) */
#ifndef _KPI_LINUX_DMA_MAPPING_H
#define _KPI_LINUX_DMA_MAPPING_H

#include <linux/kernel.h>
#include <linux/dma-direction.h>
#include <linux/scatterlist.h>
#include <linux/device.h>
#include <linux/mm.h>

/* no IOMMU: a page's DMA address is its physical address */
#define DMA_ATTR_SKIP_CPU_SYNC		(1UL << 5)
#define DMA_ATTR_NO_WARN		(1UL << 8)
#define DMA_ATTR_NO_KERNEL_MAPPING	(1UL << 4)
#define DMA_ATTR_FORCE_CONTIGUOUS	(1UL << 3)
#define DMA_ATTR_WRITE_COMBINE		(1UL << 2)
#define DMA_MAPPING_ERROR		(~(dma_addr_t)0)
#define DMA_BIT_MASK(n)			(((n) == 64) ? ~0ULL : ((1ULL << (n)) - 1))
static inline int dma_mapping_error(struct device *d, dma_addr_t a) { return a == DMA_MAPPING_ERROR; }
static inline dma_addr_t dma_map_page_attrs(struct device *d, struct page *p, size_t off, size_t size, enum dma_data_direction dir, unsigned long attrs) { return page_to_phys(p) + off; }
#define dma_map_page(d, p, o, s, r)		dma_map_page_attrs(d, p, o, s, r, 0)
#define dma_unmap_page(d, a, s, r)		do { } while (0)
#define dma_unmap_page_attrs(d, a, s, r, at)	do { } while (0)
#define dma_map_resource(d, p, s, r, a)		((dma_addr_t)(p))
#define dma_unmap_resource(d, a, s, r, at)	do { } while (0)
int	dma_map_sg_attrs(struct device *d, struct scatterlist *sg, int nents, enum dma_data_direction dir, unsigned long attrs);
int	dma_map_sgtable(struct device *d, struct sg_table *t, enum dma_data_direction dir, unsigned long attrs);
#define dma_map_sg(d, sg, n, r)			dma_map_sg_attrs(d, sg, n, r, 0)
#define dma_unmap_sg(d, sg, n, r)		do { } while (0)
#define dma_unmap_sg_attrs(d, sg, n, r, a)	do { } while (0)
#define dma_unmap_sgtable(d, t, r, a)		do { } while (0)
#define dma_sync_sg_for_cpu(d, sg, n, r)	do { } while (0)
#define dma_sync_sg_for_device(d, sg, n, r)	do { } while (0)
#define dma_sync_sgtable_for_cpu(d, t, r)	do { } while (0)
#define dma_sync_sgtable_for_device(d, t, r)	do { } while (0)
#define dma_sync_single_for_cpu(d, a, s, r)	do { } while (0)
#define dma_sync_single_for_device(d, a, s, r)	do { } while (0)
#define dma_set_mask(d, m)			0
#define dma_set_coherent_mask(d, m)		0
#define dma_set_mask_and_coherent(d, m)		0
#define dma_coerce_mask_and_coherent(d, m)	0
#define dma_set_max_seg_size(d, s)		0
#define dma_get_max_seg_size(d)			(UINT_MAX & PAGE_MASK)
#define dma_max_mapping_size(d)			SIZE_MAX
#define dma_addressing_limited(d)		0
#define dma_get_mask(d)				DMA_BIT_MASK(64)
void	*dma_alloc_attrs(struct device *d, size_t size, dma_addr_t *handle, gfp_t gfp, unsigned long attrs);
void	dma_free_attrs(struct device *d, size_t size, void *cpu, dma_addr_t handle, unsigned long attrs);
void	*dma_alloc_coherent(struct device *d, size_t size, dma_addr_t *handle, gfp_t gfp);
void	dma_free_coherent(struct device *d, size_t size, void *cpu, dma_addr_t handle);
#define dma_alloc_wc(d, s, h, g)		dma_alloc_coherent(d, s, h, g)
#define dma_free_wc(d, s, c, h)			dma_free_coherent(d, s, c, h)

#endif
