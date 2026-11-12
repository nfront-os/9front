/* Public domain. */
/*
 * 9front: OpenBSD's header (openbsd/), for the fence of Linux 7.3: the
 * lock is an external one (extern_lock) or the fence's own (own_lock,
 * DMA_FENCE_FLAG_INLINE_LOCK_BIT), dma_fence_spinlock gives it; ops is
 * read under RCU and NULL once a signalled fence needs nothing more of
 * its driver; DMA_FENCE_FLAG_INITIALIZED_BIT says dma_fence_init ran;
 * the signalling functions return nothing.  The functions are in
 * kpi/linux/dma_fence.c (from drm-kmod's, BSD) and drm_linux.c
 * (OpenBSD's waits, stubs, arrays and chains).  Changes marked 9front.
 */

#ifndef _LINUX_DMA_FENCE_H
#define _LINUX_DMA_FENCE_H

#include <sys/types.h>
#include <linux/kref.h>
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/rcupdate.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/ktime.h>
#include <linux/bitops.h>
#include <linux/seq_file.h>

struct dma_fence {
	struct kref refcount;
	const struct dma_fence_ops __rcu *ops;
	unsigned long flags;
	uint64_t context;
	uint64_t seqno;
	union {			/* 9front: whose lock (INLINE_LOCK) */
		spinlock_t *extern_lock;
		spinlock_t own_lock;
	};
	union {
		struct list_head cb_list;
		ktime_t timestamp;
		struct rcu_head rcu;
	};
	int error;
};

enum dma_fence_flag_bits {
	DMA_FENCE_FLAG_SEQNO64_BIT,	/* 9front: OpenBSD's SEQ64 under 7.3's name */
	DMA_FENCE_FLAG_SIGNALED_BIT,
	DMA_FENCE_FLAG_TIMESTAMP_BIT,
	DMA_FENCE_FLAG_ENABLE_SIGNAL_BIT,
	DMA_FENCE_FLAG_INLINE_LOCK_BIT,	/* 9front: own_lock */
	DMA_FENCE_FLAG_INITIALIZED_BIT,	/* 9front: dma_fence_init ran */
	DMA_FENCE_FLAG_USER_BITS,
};

struct dma_fence_ops {
	const char * (*get_driver_name)(struct dma_fence *);
	const char * (*get_timeline_name)(struct dma_fence *);
	bool (*enable_signaling)(struct dma_fence *);
	bool (*signaled)(struct dma_fence *);
	long (*wait)(struct dma_fence *, bool, long);
	void (*release)(struct dma_fence *);
	void (*set_deadline)(struct dma_fence *, ktime_t);
};

struct dma_fence_cb;
typedef void (*dma_fence_func_t)(struct dma_fence *fence, struct dma_fence_cb *cb);

struct dma_fence_cb {
	struct list_head node;
	dma_fence_func_t func;
};

/* 9front: the lock of the fence, and taking it */
static inline spinlock_t *
dma_fence_spinlock(struct dma_fence *f)
{
	if (test_bit(DMA_FENCE_FLAG_INLINE_LOCK_BIT, &f->flags))
		return &f->own_lock;
	return f->extern_lock;
}
#define dma_fence_lock_irqsave(f, fl)	spin_lock_irqsave(dma_fence_spinlock(f), (fl))
#define dma_fence_unlock_irqrestore(f, fl)	spin_unlock_irqrestore(dma_fence_spinlock(f), (fl))
#define dma_fence_assert_held(f)	lockdep_assert_held(dma_fence_spinlock(f))

/* 9front: the flags alone, no question to the driver */
static inline bool
dma_fence_test_signaled_flag(struct dma_fence *f)
{
	return test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &f->flags) != 0;
}

static inline bool
dma_fence_was_initialized(struct dma_fence *f)
{
	return f != NULL && test_bit(DMA_FENCE_FLAG_INITIALIZED_BIT, &f->flags);
}

uint64_t dma_fence_context_alloc(unsigned int);
void dma_fence_init(struct dma_fence *, const struct dma_fence_ops *,
    spinlock_t *, uint64_t, uint64_t);
void dma_fence_init64(struct dma_fence *, const struct dma_fence_ops *,
    spinlock_t *, uint64_t, uint64_t);
struct dma_fence *dma_fence_get_rcu(struct dma_fence *);
struct dma_fence *dma_fence_get_rcu_safe(struct dma_fence __rcu **);
void dma_fence_release(struct kref *);
void dma_fence_free(struct dma_fence *);
void dma_fence_signal(struct dma_fence *);
void dma_fence_signal_locked(struct dma_fence *);
void dma_fence_signal_timestamp(struct dma_fence *, ktime_t);
void dma_fence_signal_timestamp_locked(struct dma_fence *, ktime_t);
bool dma_fence_check_and_signal(struct dma_fence *);
bool dma_fence_check_and_signal_locked(struct dma_fence *);
bool dma_fence_is_signaled(struct dma_fence *);
bool dma_fence_is_signaled_locked(struct dma_fence *);
ktime_t dma_fence_timestamp(struct dma_fence *);
long dma_fence_default_wait(struct dma_fence *, bool, long);
long dma_fence_wait_any_timeout(struct dma_fence **, uint32_t, bool, long,
    uint32_t *);
long dma_fence_wait_timeout(struct dma_fence *, bool, long);
long dma_fence_wait(struct dma_fence *, bool);
void dma_fence_enable_sw_signaling(struct dma_fence *);
void dma_fence_enable_signaling(struct dma_fence *);
int dma_fence_add_callback(struct dma_fence *, struct dma_fence_cb *,
    dma_fence_func_t);
bool dma_fence_remove_callback(struct dma_fence *, struct dma_fence_cb *);
bool dma_fence_is_array(struct dma_fence *);
bool dma_fence_is_chain(struct dma_fence *);
void dma_fence_set_deadline(struct dma_fence *, ktime_t);
void dma_fence_set_error(struct dma_fence *, int);
int dma_fence_get_status(struct dma_fence *);
int dma_fence_get_status_locked(struct dma_fence *);
void dma_fence_describe(struct dma_fence *, struct seq_file *);
const char __rcu *dma_fence_driver_name(struct dma_fence *);
const char __rcu *dma_fence_timeline_name(struct dma_fence *);

struct dma_fence *dma_fence_get_stub(void);
struct dma_fence *dma_fence_allocate_private_stub(ktime_t);

/* 9front: enable signalling with the lock held; false once signalled */
bool kpi_dma_fence_arm(struct dma_fence *);

extern const struct dma_fence_ops dma_fence_array_ops;
extern const struct dma_fence_ops dma_fence_chain_ops;

/* 9front: inline here, as the drivers count on (no call) */
static inline struct dma_fence *
dma_fence_get(struct dma_fence *fence)
{
	if (fence)
		kref_get(&fence->refcount);
	return fence;
}

static inline void
dma_fence_put(struct dma_fence *fence)
{
	if (fence)
		kref_put(&fence->refcount, dma_fence_release);
}

static inline bool
dma_fence_is_container(struct dma_fence *fence)
{
	return (dma_fence_is_array(fence) || dma_fence_is_chain(fence));
}

/*
 * is a later than b
 * if a and b are the same, should return false to avoid unwanted work
 */
static inline bool
__dma_fence_is_later(struct dma_fence *f, uint64_t a, uint64_t b)
{
	uint32_t al, bl;

	if (test_bit(DMA_FENCE_FLAG_SEQNO64_BIT, &f->flags))
		return a > b;

	al = a & 0xffffffff;
	bl = b & 0xffffffff;

	return (int)(al - bl) > 0;
}

static inline bool
dma_fence_is_later(struct dma_fence *a, struct dma_fence *b)
{
	if (a->context != b->context)
		return false;
	return __dma_fence_is_later(a, a->seqno, b->seqno);
}

static inline bool
dma_fence_is_later_or_same(struct dma_fence *a, struct dma_fence *b)
{
	/* 9front: the same number counts as the same (Linux 7.3) */
	if (a == b || a->seqno == b->seqno)
		return true;
	return dma_fence_is_later(a, b);
}

static inline bool
dma_fence_begin_signalling(void)
{
	return true;
}

static inline void
dma_fence_end_signalling(bool x)
{
}

/* 9front: no lock checker here */
#define __dma_fence_might_wait()	do { } while (0)

#endif
