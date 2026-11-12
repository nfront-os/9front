/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/scatterlist.h (ours, not Linux's) */
#ifndef _KPI_LINUX_SCATTERLIST_H
#define _KPI_LINUX_SCATTERLIST_H

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/string.h>

/* scatter lists: kpi/scatterlist.c (ours: Linux's is GPL) */
struct scatterlist {
	unsigned long	page_link;
	unsigned int	offset;
	unsigned int	length;
	dma_addr_t	dma_address;
	unsigned int	dma_length;
};
struct sg_table { struct scatterlist *sgl; unsigned int nents; unsigned int orig_nents; };
struct sg_append_table { struct sg_table sgt; struct scatterlist *prv; unsigned int total_nents; };
#define SG_CHAIN		0x01UL
#define SG_END			0x02UL
#define SG_PAGE_LINK_MASK	(SG_CHAIN | SG_END)
#define SG_MAX_SINGLE_ALLOC	(PAGE_SIZE / sizeof(struct scatterlist))
#define sg_is_chain(sg)		((sg)->page_link & SG_CHAIN)
#define sg_is_last(sg)		((sg)->page_link & SG_END)
#define sg_chain_ptr(sg)	((struct scatterlist *)((sg)->page_link & ~SG_PAGE_LINK_MASK))
#define sg_dma_address(sg)	((sg)->dma_address)
#define sg_dma_len(sg)		((sg)->dma_length)
static inline struct page *sg_page(struct scatterlist *sg) { return (struct page *)(sg->page_link & ~SG_PAGE_LINK_MASK); }
static inline void sg_assign_page(struct scatterlist *sg, struct page *p) { sg->page_link = (sg->page_link & SG_PAGE_LINK_MASK) | (unsigned long)p; }
static inline void sg_set_page(struct scatterlist *sg, struct page *p, unsigned int len, unsigned int off) { sg_assign_page(sg, p); sg->offset = off; sg->length = len; }
static inline void sg_set_folio(struct scatterlist *sg, struct folio *f, size_t len, size_t off) { sg_set_page(sg, &f->page, len, off); }
static inline void sg_mark_end(struct scatterlist *sg) { sg->page_link |= SG_END; sg->page_link &= ~SG_CHAIN; }
static inline void sg_unmark_end(struct scatterlist *sg) { sg->page_link &= ~SG_END; }
static inline void *sg_virt(struct scatterlist *sg) { return (char *)page_address(sg_page(sg)) + sg->offset; }
static inline dma_addr_t sg_phys(struct scatterlist *sg) { return page_to_phys(sg_page(sg)) + sg->offset; }
struct scatterlist	*sg_next(struct scatterlist *sg);
void	sg_init_table(struct scatterlist *sg, unsigned int n);
void	sg_init_one(struct scatterlist *sg, const void *buf, unsigned int len);
int	sg_alloc_table(struct sg_table *t, unsigned int n, gfp_t gfp);
void	sg_free_table(struct sg_table *t);
void	sg_free_append_table(struct sg_append_table *t);
int	sg_alloc_table_from_pages_segment(struct sg_table *t, struct page **pages, unsigned int n, unsigned int off, unsigned long size, unsigned int max, gfp_t gfp);
#define sg_alloc_table_from_pages(t, p, n, o, s, g)	sg_alloc_table_from_pages_segment(t, p, n, o, s, UINT_MAX, g)
int	sg_nents(struct scatterlist *sg);
size_t	sg_copy_from_buffer(struct scatterlist *sg, unsigned int n, const void *buf, size_t len);
size_t	sg_copy_to_buffer(struct scatterlist *sg, unsigned int n, void *buf, size_t len);
#define for_each_sg(sgl, sg, n, i)	for ((i) = 0, (sg) = (sgl); (i) < (n); (i)++, (sg) = sg_next(sg))
#define for_each_sgtable_sg(t, sg, i)	for_each_sg((t)->sgl, sg, (t)->orig_nents, i)
#define for_each_sgtable_dma_sg(t, sg, i)	for_each_sg((t)->sgl, sg, (t)->nents, i)
struct sg_page_iter { struct scatterlist *sg; unsigned int sg_pgoffset; unsigned int __nents; int __pg_advance; };
struct sg_dma_page_iter { struct sg_page_iter base; };
void	__sg_page_iter_start(struct sg_page_iter *it, struct scatterlist *sgl, unsigned int nents, unsigned long pgoff);
bool	__sg_page_iter_next(struct sg_page_iter *it);
bool	__sg_page_iter_dma_next(struct sg_dma_page_iter *it);
#define sg_page_iter_page(it)		nth_page(sg_page((it)->sg), (it)->sg_pgoffset)
#define sg_page_iter_dma_address(it)	(sg_dma_address((it)->base.sg) + ((dma_addr_t)(it)->base.sg_pgoffset << PAGE_SHIFT))
#define for_each_sg_page(sgl, it, n, off)	for (__sg_page_iter_start((it), (sgl), (n), (off)); __sg_page_iter_next(it);)
#define for_each_sg_dma_page(sgl, it, n, off)	for (__sg_page_iter_start(&(it)->base, sgl, n, off); __sg_page_iter_dma_next(it);)
#define for_each_sgtable_page(t, it, off)	for_each_sg_page((t)->sgl, it, (t)->orig_nents, off)
#define for_each_sgtable_dma_page(t, it, off)	for_each_sg_dma_page((t)->sgl, it, (t)->nents, off)

#endif
