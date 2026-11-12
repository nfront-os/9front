// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: DMA mapping without an IOMMU: the device reaches
 * memory at its physical address; coherent memory is a run of pages
 * from the page pool.
 */
#include <linux/dma-mapping.h>
#include <linux/scatterlist.h>
#include <linux/mm.h>

int
dma_map_sg_attrs(struct device *d, struct scatterlist *sgl, int nents, enum dma_data_direction dir, unsigned long attrs)
{
	struct scatterlist *sg;
	int i;

	for_each_sg(sgl, sg, nents, i) {
		/* a page of the heap has no address for the device (page.c) */
		if (WARN_ON_ONCE(!sg_page(sg)->kpi_chunk))
			return 0;
		sg->dma_address = sg_phys(sg);
		sg->dma_length = sg->length;
	}
	return nents;
}

int
dma_map_sgtable(struct device *d, struct sg_table *t, enum dma_data_direction dir, unsigned long attrs)
{
	int n = dma_map_sg_attrs(d, t->sgl, t->orig_nents, dir, attrs);

	if (n <= 0)
		return -EINVAL;
	t->nents = n;
	return 0;
}

void *
dma_alloc_attrs(struct device *d, size_t size, dma_addr_t *handle, gfp_t gfp, unsigned long attrs)
{
	struct page *p = alloc_pages(gfp | __GFP_ZERO, get_order(size));

	if (!p)
		return NULL;
	*handle = page_to_phys(p);
	return page_address(p);
}

void
dma_free_attrs(struct device *d, size_t size, void *cpu, dma_addr_t handle, unsigned long attrs)
{
	if (cpu)
		__free_pages(virt_to_page(cpu), get_order(size));
}

void *dma_alloc_coherent(struct device *d, size_t size, dma_addr_t *handle, gfp_t gfp) { return dma_alloc_attrs(d, size, handle, gfp, 0); }
void dma_free_coherent(struct device *d, size_t size, void *cpu, dma_addr_t handle) { dma_free_attrs(d, size, cpu, handle, 0); }
