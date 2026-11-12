/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: scatterlists (linux/scatterlist.h).  As
 * OpenBSD's, a table is one array of entries, never chained: the last
 * entry has the end mark, sg_next steps through the array.  A table
 * from pages joins the runs of pages that follow each other in physical
 * memory, up to the maximal segment.
 */
#include <linux/scatterlist.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/highmem.h>
#include <linux/err.h>

struct scatterlist *
sg_next(struct scatterlist *sg)
{
	if (sg_is_last(sg))
		return NULL;
	sg++;
	if (sg_is_chain(sg))
		sg = sg_chain_ptr(sg);
	return sg;
}

void
sg_init_table(struct scatterlist *sgl, unsigned int n)
{
	memset(sgl, 0, n * sizeof(*sgl));
	if (n > 0)
		sg_mark_end(&sgl[n - 1]);
}

void
sg_init_one(struct scatterlist *sg, const void *buf, unsigned int len)
{
	sg_init_table(sg, 1);
	sg_set_page(sg, virt_to_page(buf), len, offset_in_page(buf));
}

int
sg_alloc_table(struct sg_table *t, unsigned int n, gfp_t gfp)
{
	memset(t, 0, sizeof(*t));
	if (n == 0)
		return -EINVAL;
	t->sgl = kvmalloc_array(n, sizeof(*t->sgl), gfp);
	if (t->sgl == NULL)
		return -ENOMEM;
	sg_init_table(t->sgl, n);
	t->nents = t->orig_nents = n;
	return 0;
}

void
sg_free_table(struct sg_table *t)
{
	kvfree(t->sgl);
	t->sgl = NULL;
	t->nents = t->orig_nents = 0;
}

void
sg_free_append_table(struct sg_append_table *t)
{
	sg_free_table(&t->sgt);
	t->prv = NULL;
	t->total_nents = 0;
}

int
sg_nents(struct scatterlist *sg)
{
	int n;

	for (n = 0; sg != NULL; sg = sg_next(sg))
		n++;
	return n;
}

static inline bool
follows(struct page *a, struct page *b)
{
	return page_to_pfn(a) + 1 == page_to_pfn(b);
}

int
sg_alloc_table_from_pages_segment(struct sg_table *t, struct page **pages, unsigned int n,
    unsigned int off, unsigned long size, unsigned int max, gfp_t gfp)
{
	struct scatterlist *sg;
	unsigned int i, j, nseg, len, chunk;
	unsigned long left;
	int ret;

	if (max == 0 || n == 0)
		return -EINVAL;
	max = rounddown(max, PAGE_SIZE);
	if (max == 0)
		max = PAGE_SIZE;
	/* how many segments: runs of following pages, each up to max */
	nseg = 0;
	for (i = 0; i < n; i = j) {
		chunk = PAGE_SIZE - (i == 0 ? off : 0);
		for (j = i + 1; j < n && follows(pages[j - 1], pages[j]) && chunk + PAGE_SIZE <= max; j++)
			chunk += PAGE_SIZE;
		nseg++;
	}
	ret = sg_alloc_table(t, nseg, gfp);
	if (ret)
		return ret;
	sg = t->sgl;
	left = size;
	for (i = 0; i < n; i = j) {
		chunk = PAGE_SIZE - (i == 0 ? off : 0);
		for (j = i + 1; j < n && follows(pages[j - 1], pages[j]) && chunk + PAGE_SIZE <= max; j++)
			chunk += PAGE_SIZE;
		len = min_t(unsigned long, chunk, left);
		sg_set_page(sg, pages[i], len, i == 0 ? off : 0);
		left -= len;
		sg = sg_next(sg);
	}
	return 0;
}

/* copies between a buffer and the pages of a list (their file server addresses) */
static size_t
sgcopy(struct scatterlist *sgl, unsigned int nents, void *buf, size_t len, bool tosg)
{
	struct scatterlist *sg;
	size_t done = 0, n;
	unsigned int i;
	char *va;

	for_each_sg(sgl, sg, nents, i) {
		if (done == len)
			break;
		n = min_t(size_t, sg->length, len - done);
		va = sg_virt(sg);
		if (tosg)
			memcpy(va, (char *)buf + done, n);
		else
			memcpy((char *)buf + done, va, n);
		done += n;
	}
	return done;
}

size_t
sg_copy_from_buffer(struct scatterlist *sgl, unsigned int nents, const void *buf, size_t len)
{
	return sgcopy(sgl, nents, (void *)buf, len, true);
}

size_t
sg_copy_to_buffer(struct scatterlist *sgl, unsigned int nents, void *buf, size_t len)
{
	return sgcopy(sgl, nents, buf, len, false);
}

/* page iterators: each page of each entry, from pgoff pages into the list */
static unsigned int
sgpages(struct scatterlist *sg, unsigned int len)
{
	return PAGE_ALIGN(sg->offset + len) >> PAGE_SHIFT;
}

void
__sg_page_iter_start(struct sg_page_iter *it, struct scatterlist *sgl, unsigned int nents,
    unsigned long pgoff)
{
	it->sg = sgl;
	it->sg_pgoffset = pgoff;
	it->__nents = nents;
	it->__pg_advance = 0;
}

static bool
iternext(struct sg_page_iter *it, bool dma)
{
	unsigned int npages;

	if (it->__nents == 0 || it->sg == NULL)
		return false;
	it->sg_pgoffset += it->__pg_advance;
	it->__pg_advance = 1;
	for (;;) {
		npages = dma ? PAGE_ALIGN(sg_dma_len(it->sg)) >> PAGE_SHIFT :
		    sgpages(it->sg, it->sg->length);
		if (it->sg_pgoffset < npages)
			return true;
		it->sg_pgoffset -= npages;
		if (--it->__nents == 0)
			return false;
		it->sg = sg_next(it->sg);
		if (it->sg == NULL)
			return false;
	}
}

bool
__sg_page_iter_next(struct sg_page_iter *it)
{
	return iternext(it, false);
}

bool
__sg_page_iter_dma_next(struct sg_dma_page_iter *it)
{
	return iternext(&it->base, true);
}
