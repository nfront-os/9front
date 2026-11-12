/* Public domain. */

#ifndef _LINUX_DMA_FENCE_ARRAY_H
#define _LINUX_DMA_FENCE_ARRAY_H

#include <linux/dma-fence.h>
#include <linux/irq_work.h>

struct dma_fence_array_cb {
	struct dma_fence_cb cb;
	struct dma_fence_array *array;
};

struct dma_fence_array {
	struct dma_fence base;
	unsigned int num_fences;
	struct dma_fence **fences;
	struct spinlock lock;
	struct timeout to;
	atomic_t num_pending;	/* 9front: an atomic_t (linux/atomic.h) */
};

extern const struct dma_fence_ops dma_fence_array_ops;

static inline struct dma_fence_array *
to_dma_fence_array(struct dma_fence *fence)
{
	if (fence == NULL || !dma_fence_is_array(fence))
		return NULL;

	return container_of(fence, struct dma_fence_array, base);
}

struct dma_fence_array *dma_fence_array_alloc(int);
void dma_fence_array_init(struct dma_fence_array *, int, struct dma_fence **,
    u64, unsigned);
struct dma_fence_array *dma_fence_array_create(int, struct dma_fence **,
    u64, unsigned);
struct dma_fence *dma_fence_array_first(struct dma_fence *);
struct dma_fence *dma_fence_array_next(struct dma_fence *, unsigned int);

#define dma_fence_array_for_each(f, i, h)		\
	for (i = 0, f = dma_fence_array_first(h);	\
	     f != NULL;					\
	     ++(i), f = dma_fence_array_next(h, i))	\

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
bool	dma_fence_match_context(struct dma_fence *f, u64 context);

#endif
