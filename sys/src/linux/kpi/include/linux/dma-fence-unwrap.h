/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/dma-fence-unwrap.h (ours, not Linux's) */
#ifndef _KPI_LINUX_DMA_FENCE_UNWRAP_H
#define _KPI_LINUX_DMA_FENCE_UNWRAP_H

#include <linux/dma-fence-array.h>
#include <linux/dma-fence-chain.h>

struct dma_fence_unwrap { struct dma_fence *chain; struct dma_fence *array; unsigned int index; };
struct dma_fence	*dma_fence_unwrap_first(struct dma_fence *head, struct dma_fence_unwrap *cursor);
struct dma_fence	*dma_fence_unwrap_next(struct dma_fence_unwrap *cursor);
#define dma_fence_unwrap_for_each(f, cursor, head)	for (f = dma_fence_unwrap_first(head, cursor); f; f = dma_fence_unwrap_next(cursor))
struct dma_fence	*__dma_fence_unwrap_merge(unsigned int num_fences, struct dma_fence **fences, struct dma_fence_unwrap *cursors);
#define dma_fence_unwrap_merge(...)	({ struct dma_fence *__f[] = { __VA_ARGS__ }; struct dma_fence_unwrap __c[ARRAY_SIZE(__f)]; \
	__dma_fence_unwrap_merge(ARRAY_SIZE(__f), __f, __c); })

size_t	dma_fence_dedup_array(struct dma_fence **fences, size_t n);

#endif
