/* Public domain. */
/*
 * 9front: a pool of preallocated elements in Linux (allocations that must
 * not fail); here a size, each element from kmalloc
 */
#ifndef _LINUX_MEMPOOL_H
#define _LINUX_MEMPOOL_H

#include <linux/slab.h>

typedef struct mempool { size_t size; } mempool_t;

static inline mempool_t *
mempool_create_kmalloc_pool(int min, size_t size)
{
	mempool_t *p = kmalloc(sizeof(*p), GFP_KERNEL);

	if (p)
		p->size = size;
	return p;
}
#define mempool_destroy(p)		kfree(p)
#define mempool_alloc(p, gfp)		kmalloc(((mempool_t *)(p))->size, (gfp))
#define mempool_alloc_preallocated(p)	kmalloc(((mempool_t *)(p))->size, GFP_ATOMIC)	/* (a void * too, as amdgpu's RAS keeps it) */
#define mempool_free(e, p)		kfree(e)

#endif
