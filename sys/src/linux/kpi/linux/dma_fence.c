/*-
 * Copyright (c) 2022 Beckhoff Automation GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 * 9front: drm-kmod's drivers/dma-buf/dma-fence.c (freebsd/drm-kmod,
 * drm_v6.14), the fence itself; the waits, the stubs, arrays and chains
 * stay OpenBSD's (drm_linux.c), unwrap and the status in fence.c.
 * Changes are marked 9front, most for the fence of Linux 7.3: the lock
 * through dma_fence_spinlock (an external one or the fence's own) and
 * taken with interrupts off; ops read under RCU and dropped when the
 * fence is signalled and needs nothing more of its driver; 64-bit
 * sequence numbers a flag; signalling functions without a result.
 */

#include <linux/dma-fence.h>
#include <linux/dma-fence-array.h>
#include <linux/dma-fence-chain.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

/* 9front: the ops, NULL once the fence left its driver (read under RCU) */
static const struct dma_fence_ops *
fence_ops(struct dma_fence *fence)
{

	return (rcu_dereference(fence->ops));
}

/*
 * 9front: ask the driver to signal (enable_signaling), once, with the
 * lock held; false if the fence is signalled (or the driver did so)
 */
bool
kpi_dma_fence_arm(struct dma_fence *fence)
{
	const struct dma_fence_ops *ops;
	bool was_enabled, armed;

	dma_fence_assert_held(fence);
	was_enabled = test_and_set_bit(DMA_FENCE_FLAG_ENABLE_SIGNAL_BIT,
	    &fence->flags);
	if (test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags))
		return (false);
	armed = true;
	rcu_read_lock();
	ops = fence_ops(fence);
	if (was_enabled == false && ops && ops->enable_signaling &&
	    ops->enable_signaling(fence) == false)
		armed = false;
	rcu_read_unlock();
	if (!armed)
		dma_fence_signal_locked(fence);
	return (armed);
}

/*
 * signal completion of a fence
 */
void
dma_fence_signal_timestamp_locked(struct dma_fence *fence,
				  ktime_t timestamp)
{
	struct dma_fence_cb *cur, *tmp;
	struct list_head cb_list;
	const struct dma_fence_ops *ops;

	if (fence == NULL)
		return;
	dma_fence_assert_held(fence);
	if (test_and_set_bit(DMA_FENCE_FLAG_SIGNALED_BIT,
	      &fence->flags))
		return;

	/* 9front: a fence that needs no more of its driver leaves it */
	ops = rcu_dereference_protected(fence->ops, 1);
	if (ops->release == NULL && ops->wait == NULL)
		RCU_INIT_POINTER(fence->ops, NULL);

	list_replace(&fence->cb_list, &cb_list);

	fence->timestamp = timestamp;
	set_bit(DMA_FENCE_FLAG_TIMESTAMP_BIT, &fence->flags);

	list_for_each_entry_safe(cur, tmp, &cb_list, node) {
		INIT_LIST_HEAD(&cur->node);
		cur->func(fence, cur);
	}
}

/*
 * signal completion of a fence
 */
void
dma_fence_signal_timestamp(struct dma_fence *fence, ktime_t timestamp)
{
	unsigned long flags;

	if (WARN_ON(fence == NULL))
		return;

	dma_fence_lock_irqsave(fence, flags);
	dma_fence_signal_timestamp_locked(fence, timestamp);
	dma_fence_unlock_irqrestore(fence, flags);
}

/*
 * signal completion of a fence
 */
void
dma_fence_signal_locked(struct dma_fence *fence)
{
	ktime_t now;

	now = ktime_get();
	dma_fence_signal_timestamp_locked(fence, now);
}

/*
 * signal completion of a fence
 */
void
dma_fence_signal(struct dma_fence *fence)
{

	dma_fence_signal_timestamp(fence, ktime_get());
}

/* 9front: signal the fence; true if it had been signalled before */
bool
dma_fence_check_and_signal_locked(struct dma_fence *fence)
{
	bool before;

	before = test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags);
	if (!before)
		dma_fence_signal_locked(fence);
	return (before);
}

bool
dma_fence_check_and_signal(struct dma_fence *fence)
{
	unsigned long flags;
	bool before;

	dma_fence_lock_irqsave(fence, flags);
	before = dma_fence_check_and_signal_locked(fence);
	dma_fence_unlock_irqrestore(fence, flags);
	return (before);
}

/*
 * get completion timestamp of a fence
 */
ktime_t
dma_fence_timestamp(struct dma_fence *fence)
{
	if (WARN_ON(!dma_fence_test_signaled_flag(fence)))
		return (ktime_get());

	while (!test_bit(DMA_FENCE_FLAG_TIMESTAMP_BIT, &fence->flags))
		cpu_relax();

	return (fence->timestamp);
}

/*
 * sleep until the fence gets signaled or until timeout elapses
 */
long
dma_fence_wait_timeout(struct dma_fence *fence, bool intr, long timeout)
{
	const struct dma_fence_ops *ops;

	if (fence == NULL || WARN_ON(timeout < 0))
		return (-EINVAL);

	dma_fence_enable_sw_signaling(fence);

	/* 9front: a fence with a wait of its own keeps its ops */
	rcu_read_lock();
	ops = fence_ops(fence);
	rcu_read_unlock();
	if (ops && ops->wait != NULL)
		return (ops->wait(fence, intr, timeout));
	return (dma_fence_default_wait(fence, intr, timeout));
}

/*
 * sleep until the fence gets signaled
 */
long
dma_fence_wait(struct dma_fence *fence, bool intr)
{
	long ret;

	ret = dma_fence_wait_timeout(fence, intr, MAX_SCHEDULE_TIMEOUT);

	return (ret < 0 ? ret : 0);
}

/*
 * default relese function for fences
 */
void
dma_fence_release(struct kref *kref)
{
	struct dma_fence *fence;
	const struct dma_fence_ops *ops;
	unsigned long flags;

	fence = container_of(kref, struct dma_fence, refcount);

	/*
	 * 9front: a fence released while callbacks still wait on it is
	 * signalled with an error, so that none of them waits forever
	 */
	if (!test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags) &&
	    !list_empty(&fence->cb_list)) {
		WARN(1, "dma_fence %llu:%llu freed before it was signalled\n",
		    (unsigned long long)fence->context,
		    (unsigned long long)fence->seqno);
		dma_fence_lock_irqsave(fence, flags);
		fence->error = -EDEADLK;
		dma_fence_signal_locked(fence);
		dma_fence_unlock_irqrestore(fence, flags);
	}

	rcu_read_lock();
	ops = fence_ops(fence);
	if (ops && ops->release)
		ops->release(fence);
	else
		dma_fence_free(fence);
	rcu_read_unlock();
}

/*
 * default release function for dma_fence
 */
void
dma_fence_free(struct dma_fence *fence)
{

	kfree_rcu(fence, rcu);
}

/* 9front: enable signalling (Linux 6.x's name, and the older one) */
void
dma_fence_enable_signaling(struct dma_fence *fence)
{
	unsigned long flags;

	dma_fence_lock_irqsave(fence, flags);
	kpi_dma_fence_arm(fence);
	dma_fence_unlock_irqrestore(fence, flags);
}

/*
 * enable signaling on fence
 */
void
dma_fence_enable_sw_signaling(struct dma_fence *fence)
{

	dma_fence_enable_signaling(fence);
}

/*
 * add a callback to be called when the fence is signaled
 */
int
dma_fence_add_callback(struct dma_fence *fence, struct dma_fence_cb *cb,
			   dma_fence_func_t func)
{
	unsigned long flags;
	int rv = 0;

	if (WARN_ON(fence == NULL || func == NULL))
		return (-EINVAL);

	if (test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags)) {
		INIT_LIST_HEAD(&cb->node);
		return (-ENOENT);
	}

	dma_fence_lock_irqsave(fence, flags);
	if (!kpi_dma_fence_arm(fence))
		rv = -ENOENT;

	if (!rv) {
		cb->func = func;
		list_add_tail(&cb->node, &fence->cb_list);
	} else
		INIT_LIST_HEAD(&cb->node);
	dma_fence_unlock_irqrestore(fence, flags);

	return (rv);
}

/*
 * remove a callback from the signaling list
 */
bool
dma_fence_remove_callback(struct dma_fence *fence, struct dma_fence_cb *cb)
{
	unsigned long flags;
	bool rv;

	dma_fence_lock_irqsave(fence, flags);
	rv = !list_empty(&cb->node);
	if (rv)
		list_del_init(&cb->node);
	dma_fence_unlock_irqrestore(fence, flags);
	return (rv);
}

/*
 * 9front: the names of the driver and the timeline while the fence is
 * its driver's; afterwards fixed ones (the driver may be gone)
 */
const char __rcu *
dma_fence_driver_name(struct dma_fence *fence)
{
	const struct dma_fence_ops *ops;

	ops = fence_ops(fence);
	if (ops == NULL || test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags))
		return ((const char __rcu *)"detached-driver");
	return ((const char __rcu *)ops->get_driver_name(fence));
}

const char __rcu *
dma_fence_timeline_name(struct dma_fence *fence)
{
	const struct dma_fence_ops *ops;

	ops = fence_ops(fence);
	if (ops == NULL || test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags))
		return ((const char __rcu *)"signaled-timeline");
	return ((const char __rcu *)ops->get_timeline_name(fence));
}

/*
 * Initialize a custom fence.
 */
static void
fence_init(struct dma_fence *fence, const struct dma_fence_ops *ops,
    spinlock_t *lock, u64 context, u64 seqno, unsigned long initflags)
{

	/* 9front: a driver must name itself and its timeline */
	BUG_ON(ops == NULL || ops->get_driver_name == NULL ||
	    ops->get_timeline_name == NULL);
	kref_init(&fence->refcount);
	INIT_LIST_HEAD(&fence->cb_list);
	RCU_INIT_POINTER(fence->ops, ops);
	fence->context = context;
	fence->seqno = seqno;
	fence->error = 0;
	/* 9front: without a lock given, the fence's own one */
	initflags |= BIT(DMA_FENCE_FLAG_INITIALIZED_BIT);
	if (lock == NULL) {
		initflags |= BIT(DMA_FENCE_FLAG_INLINE_LOCK_BIT);
		spin_lock_init(&fence->own_lock);
	} else
		fence->extern_lock = lock;
	fence->flags = initflags;
}

void
dma_fence_init(struct dma_fence *fence, const struct dma_fence_ops *ops,
    spinlock_t *lock, u64 context, u64 seqno)
{

	fence_init(fence, ops, lock, context, seqno, 0);
}

/* 9front: the same with 64-bit sequence numbers */
void
dma_fence_init64(struct dma_fence *fence, const struct dma_fence_ops *ops,
    spinlock_t *lock, u64 context, u64 seqno)
{

	fence_init(fence, ops, lock, context, seqno,
	    1UL << DMA_FENCE_FLAG_SEQNO64_BIT);
}

/*
 * get a fence from a dma_resv_list with rcu read lock
 */
struct dma_fence *
dma_fence_get_rcu(struct dma_fence *fence)
{

	if (fence != NULL && kref_get_unless_zero(&fence->refcount))
		return (fence);
	else
		return (NULL);
}

/*
 * acquire a reference to an RCU tracked fence
 */
struct dma_fence *
dma_fence_get_rcu_safe(struct dma_fence __rcu **fencep)
{

	if (fencep == NULL)
		return (NULL);
	do {
		struct dma_fence *fence;

		fence = rcu_dereference(*fencep);
		if (!fence)
			return (NULL);

		if (!dma_fence_get_rcu(fence))
			continue;

		if (fence == rcu_access_pointer(*fencep))
			return (fence);

		dma_fence_put(fence);
	} while (1);
}

/*
 * Return an indication if the fence is signaled yet.
 */
bool
dma_fence_is_signaled_locked(struct dma_fence *fence)
{
	const struct dma_fence_ops *ops;
	bool done;

	if (test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags))
		return (true);

	rcu_read_lock();
	ops = fence_ops(fence);
	done = ops && ops->signaled && ops->signaled(fence);
	rcu_read_unlock();
	if (done) {
		dma_fence_signal_locked(fence);
		return (true);
	}

	return (false);
}

/*
 * Return an indication if the fence is signaled yet.
 */
bool
dma_fence_is_signaled(struct dma_fence *fence)
{
	const struct dma_fence_ops *ops;
	bool done;

	if (test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags))
		return (true);

	rcu_read_lock();
	ops = fence_ops(fence);
	done = ops && ops->signaled && ops->signaled(fence);
	rcu_read_unlock();
	if (done) {
		dma_fence_signal(fence);
		return (true);
	}

	return (false);
}

/*
 * returns the status upon completion
 */
int
dma_fence_get_status_locked(struct dma_fence *fence)
{

	dma_fence_assert_held(fence);
	if (dma_fence_is_signaled_locked(fence))
		return (fence->error ?: 1);
	else
		return (0);
}

/*
 * flag an error condition on the fence
 */
void
dma_fence_set_error(struct dma_fence *fence,
    int error)
{

	/* 9front: before it is signalled, and an error number */
	WARN_ON(dma_fence_test_signaled_flag(fence));
	WARN_ON(error > -1 || error < -MAX_ERRNO);
	fence->error = error;
}

/* 9front: the containers by their ops */
bool
dma_fence_is_array(struct dma_fence *fence)
{

	return (rcu_access_pointer(fence->ops) == &dma_fence_array_ops);
}

bool
dma_fence_is_chain(struct dma_fence *fence)
{

	return (rcu_access_pointer(fence->ops) == &dma_fence_chain_ops);
}
