/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: what the Linux code uses of dma-fence besides
 * OpenBSD's (drm_linux.c): walking the fences in chains and arrays
 * (dma_fence_unwrap), merging them into one array, a fence's status;
 * and xa_cmpxchg on OpenBSD's xarray.
 */
#include <linux/dma-fence.h>
#include <linux/dma-fence-array.h>
#include <linux/dma-fence-chain.h>
#include <linux/dma-fence-unwrap.h>
#include <linux/seq_file.h>
#include <linux/xarray.h>
#include <linux/slab.h>
#include <linux/component.h>

/* the first fence in f if it is an array, else f */
static struct dma_fence *
unwraparray(struct dma_fence_unwrap *c)
{
	c->array = dma_fence_chain_contained(c->chain);
	c->index = 0;
	return dma_fence_array_first(c->array);
}

struct dma_fence *
dma_fence_unwrap_first(struct dma_fence *head, struct dma_fence_unwrap *c)
{
	c->chain = dma_fence_get(head);
	return unwraparray(c);
}

struct dma_fence *
dma_fence_unwrap_next(struct dma_fence_unwrap *c)
{
	struct dma_fence *f;

	f = dma_fence_array_next(c->array, ++c->index);
	if (f != NULL)
		return f;
	c->chain = dma_fence_chain_walk(c->chain);
	if (c->chain == NULL)
		return NULL;
	return unwraparray(c);
}

/*
 * 9front: of the references in fences, one per context, the later one
 * (Linux 6.17); the others are put; the order as they came: how many stay
 */
size_t
dma_fence_dedup_array(struct dma_fence **fences, size_t n)
{
	size_t i, j, m = 0;

	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++)
			if (fences[j]->context == fences[i]->context)
				break;
		if (j == m) {
			fences[m++] = fences[i];
			continue;
		}
		if (dma_fence_is_later(fences[i], fences[j])) {
			dma_fence_put(fences[j]);
			fences[j] = fences[i];
		} else
			dma_fence_put(fences[i]);
	}
	return m;
}

/*
 * the unsignaled fences of all, one per context (the later one), in
 * the order of the contexts, as one fence: none a signaled stub, one
 * itself, more an array
 */
struct dma_fence *
__dma_fence_unwrap_merge(unsigned int num, struct dma_fence **fences, struct dma_fence_unwrap *c)
{
	struct dma_fence **a, *f, *r;
	struct dma_fence_array *dfa;
	unsigned int i, j, n, max;

	max = 0;
	for (i = 0; i < num; i++)
		dma_fence_unwrap_for_each(f, &c[i], fences[i])
			max++;
	if (max == 0)
		return dma_fence_get_stub();
	a = kmalloc_array(max, sizeof(*a), GFP_KERNEL);
	if (a == NULL)
		return NULL;
	n = 0;
	for (i = 0; i < num; i++) {
		dma_fence_unwrap_for_each(f, &c[i], fences[i]) {
			if (dma_fence_is_signaled(f))
				continue;
			for (j = 0; j < n; j++)
				if (a[j]->context == f->context)
					break;
			if (j < n) {
				if (dma_fence_is_later(f, a[j])) {
					dma_fence_put(a[j]);
					a[j] = dma_fence_get(f);
				}
				continue;
			}
			if (n < max)
				a[n++] = dma_fence_get(f);
		}
	}
	if (n == 0) {
		kfree(a);
		return dma_fence_get_stub();
	}
	/* in the order of their contexts, as Linux's (the tests unwrap them so) */
	for (i = 1; i < n; i++) {
		f = a[i];
		for (j = i; j > 0 && a[j - 1]->context > f->context; j--)
			a[j] = a[j - 1];
		a[j] = f;
	}
	if (n == 1) {
		r = a[0];
		kfree(a);
		return r;
	}
	dfa = dma_fence_array_create(n, a, dma_fence_context_alloc(1), 1);
	if (dfa == NULL) {
		for (j = 0; j < n; j++)
			dma_fence_put(a[j]);
		kfree(a);
		return NULL;
	}
	return &dfa->base;
}

/* 0 pending, 1 signaled, < 0 the error it was signaled with */
int
dma_fence_get_status(struct dma_fence *f)
{
	unsigned long flags;
	int status;

	dma_fence_lock_irqsave(f, flags);
	status = dma_fence_get_status_locked(f);
	dma_fence_unlock_irqrestore(f, flags);
	return status;
}

void
dma_fence_describe(struct dma_fence *f, struct seq_file *seq)
{
	rcu_read_lock();
	seq_printf(seq, "%s %s seq %llu %ssignalled\n",
	    dma_fence_driver_name(f), dma_fence_timeline_name(f),
	    (unsigned long long)f->seqno, dma_fence_is_signaled(f) ? "" : "un");
	rcu_read_unlock();
}

/* xa_cmpxchg: the entry at index becomes entry if it is old; the one there before */
void *
__xa_cmpxchg(struct xarray *xa, unsigned long index, void *old, void *entry, gfp_t gfp)
{
	void *cur;

	cur = __xa_load(xa, index);
	if (cur == old) {
		if (entry != NULL)
			__xa_store(xa, index, entry, gfp);
		else
			__xa_erase(xa, index);
	}
	return cur;
}

void *
xa_cmpxchg(struct xarray *xa, unsigned long index, void *old, void *entry, gfp_t gfp)
{
	void *cur;

	xa_lock(xa);
	cur = __xa_cmpxchg(xa, index, old, entry, gfp);
	xa_unlock(xa);
	return cur;
}

/* the order of a size: the smallest n with 2^n >= size */
int
drm_order(unsigned long size)
{
	int n;

	for (n = 0; (1UL << n) < size; n++)
		;
	return n;
}

/* the component framework (drivers/base): none here, the drivers' audio components stay unbound */
int
component_add_typed(struct device *dev, const struct component_ops *ops, int subcomponent)
{
	return 0;
}

int
component_add(struct device *dev, const struct component_ops *ops)
{
	return 0;
}
