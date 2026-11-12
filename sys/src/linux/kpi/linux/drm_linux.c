/*	$OpenBSD: drm_linux.c,v 1.120 2025/02/07 03:03:08 jsg Exp $	*/
/*
 * Copyright (c) 2013 Jonathan Gray <jsg@openbsd.org>
 * Copyright (c) 2015, 2016 Mark Kettenis <kettenis@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * 9front: OpenBSD's drm_linux.c (sys/src/linux/openbsd), the parts that
 * are no platform's: tasklets, sleeping and waking (on kpi/obsd's
 * tsleep), work flushing, kthreads (kpi/obsd procs), idr/ida/xarray,
 * dma-fence with its arrays and chains, dma-buf and sync_file (on our
 * files, kpi/linux/file.c), bit waits.  Pages, PCI, ACPI, DMI, i2c,
 * backlight, files and DMA are our own (kpi/linux).  Changes are
 * marked 9front.
 */

#include <sys/types.h>
#include <sys/systm.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/pool.h>
#include <sys/fcntl.h>

#include <linux/dma-buf.h>
#include <linux/mod_devicetable.h>
#include <linux/pagevec.h>
#include <linux/dma-fence-array.h>
#include <linux/dma-fence-chain.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/idr.h>
#include <linux/scatterlist.h>
#include <linux/i2c.h>
#include <linux/pci.h>
#include <linux/notifier.h>
#include <linux/backlight.h>
#include <linux/shrinker.h>
#include <linux/fb.h>
#include <linux/xarray.h>
#include <linux/interval_tree.h>
#include <linux/kthread.h>
#include <linux/processor.h>
#include <linux/sync_file.h>
#include <linux/slab.h>

#include <drm/drm_device.h>
#include <drm/drm_connector.h>
#include <drm/drm_print.h>
#include <drm/drm_drv.h>

/* allowed to sleep */
void
tasklet_unlock_wait(struct tasklet_struct *ts)
{
	while (test_bit(TASKLET_STATE_RUN, &ts->state))
		cpu_relax();
}

/* must not sleep */
void
tasklet_unlock_spin_wait(struct tasklet_struct *ts)
{
	while (test_bit(TASKLET_STATE_RUN, &ts->state))
		cpu_relax();
}

void
tasklet_run(void *arg)
{
	struct tasklet_struct *ts = arg;

	clear_bit(TASKLET_STATE_SCHED, &ts->state);
	if (tasklet_trylock(ts)) {
		if (!atomic_read(&ts->count)) {
			if (ts->use_callback)
				ts->callback(ts);
			else
				ts->func(ts->data);
		}
		tasklet_unlock(ts);
	}
}

/* 32 bit powerpc lacks 64 bit atomics */

void
set_current_state(int state)
{
	int prio = state;

	KASSERT(state != TASK_RUNNING);
	/* check if already on the sleep list */
	if (curproc->p_wchan != NULL)
		return;
	sleep_setup(curproc, prio, "schto");
}

void
__set_current_state(int state)
{
	struct task_struct *p = curproc;

	KASSERT(state == TASK_RUNNING);
	unsleep(p);
	p->p_stat = SONPROC;
	atomic_clearbits_int(&p->p_flag, P_WSLEEP);
}

void
schedule(void)
{
	schedule_timeout(MAX_SCHEDULE_TIMEOUT);
}

long
schedule_timeout(long timeout)
{
	unsigned long deadline;
	int timo = 0;

	KASSERT(!cold);

	if (timeout != MAX_SCHEDULE_TIMEOUT)
		timo = timeout;
	if (timeout != MAX_SCHEDULE_TIMEOUT)
		deadline = jiffies + timeout;
	sleep_finish(timo > 0 ? (uint64_t)timo * (1000000000ULL / hz) : INFSLP, timeout > 0);
	if (timeout != MAX_SCHEDULE_TIMEOUT)
		timeout = deadline - jiffies;

	return timeout > 0 ? timeout : 0;
}

long
schedule_timeout_uninterruptible(long timeout)
{
	tsleep(curproc, PWAIT, "schtou", timeout);
	return 0;
}

int
wake_up_process(struct task_struct *p)
{
	int rv;

	rv = wakeup_proc(p, 0);
	return rv;
}

int
autoremove_wake_function(struct wait_queue_entry *wqe, unsigned int mode,
    int sync, void *key)
{
	if (wqe->private)
		wake_up_process(wqe->private);
	list_del_init(&wqe->entry);
	return 0;
}

/* 9front: the waiter sees WQ_FLAG_WOKEN set before it is woken (i915's must_wait_woken) */
int
woken_wake_function(struct wait_queue_entry *wqe, unsigned int mode,
    int sync, void *key)
{
	smp_mb();
	wqe->flags |= WQ_FLAG_WOKEN;
	return wake_up_process(wqe->private);
}

void
prepare_to_wait(wait_queue_head_t *wqh, wait_queue_entry_t *wqe, int state)
{
	mtx_enter(&wqh->lock);
	if (list_empty(&wqe->entry))
		__add_wait_queue(wqh, wqe);
	mtx_leave(&wqh->lock);

	set_current_state(state);
}

void
finish_wait(wait_queue_head_t *wqh, wait_queue_entry_t *wqe)
{
	__set_current_state(TASK_RUNNING);

	mtx_enter(&wqh->lock);
	if (!list_empty(&wqe->entry))
		list_del_init(&wqe->entry);
	mtx_leave(&wqh->lock);
}

void
flush_workqueue(struct workqueue_struct *wq)
{
	if (cold)
		return;

	if (wq)
		taskq_barrier((struct taskq *)wq);
}

bool
flush_work(struct work_struct *work)
{
	if (cold)
		return false;

	if (work->tq)
		taskq_barrier(work->tq);
	return false;
}

bool
flush_delayed_work(struct delayed_work *dwork)
{
	bool ret = false;

	if (cold)
		return false;

	while (timeout_pending(&dwork->to)) {
		tsleep(dwork, PWAIT, "fldwto", 1);
		ret = true;
	}

	if (dwork->tq)
		taskq_barrier(dwork->tq);
	return ret;
}

struct kthread {
	int (*func)(void *);
	void *data;
	struct task_struct *proc;
	volatile u_int flags;
	int ret;		/* 9front: the function's result, for kthread_stop */
#define KTHREAD_SHOULDSTOP	0x0000001
#define KTHREAD_STOPPED		0x0000002
#define KTHREAD_SHOULDPARK	0x0000004
#define KTHREAD_PARKED		0x0000008
	LIST_ENTRY(kthread) next;
};

BSD_LIST_HEAD(, kthread) kthread_list = LIST_HEAD_INITIALIZER(kthread_list);	/* 9front: queue.h's LIST_HEAD */
/*
 * 9front: no kernel lock here: the list has a lock of its own, and a
 * thread is on it, and knows its proc, before it runs
 */
static struct spinlock kthread_mtx = MUTEX_INITIALIZER(IPL_NONE);

void
kthread_func(void *arg)
{
	struct kthread *thread = arg;
	int ret;

	thread->proc = curproc;		/* 9front: before the creator may have set it */
	ret = thread->func(thread->data);
	thread->ret = ret;
	membar_producer();
	thread->flags |= KTHREAD_STOPPED;
	wakeup(thread);
	kthread_exit(ret);
}

struct task_struct *
__kthread_run(int (*func)(void *), void *data, const char *name)
{
	struct kthread *thread;

	thread = kpi_malloc(sizeof(*thread), M_DRM, M_WAITOK);
	thread->func = func;
	thread->data = data;
	thread->flags = 0;
	thread->proc = NULL;

	mtx_enter(&kthread_mtx);
	LIST_INSERT_HEAD(&kthread_list, thread, next);
	mtx_leave(&kthread_mtx);
	if (kthread_create(kthread_func, thread, &thread->proc, name)) {
		mtx_enter(&kthread_mtx);
		LIST_REMOVE(thread, next);
		mtx_leave(&kthread_mtx);
		kpi_free(thread, M_DRM, sizeof(*thread));
		return ERR_PTR(-ENOMEM);
	}
	return thread->proc;
}

struct kthread_worker *
kthread_run_worker(unsigned int flags, const char *fmt, ...)
{
	char name[MAXCOMLEN+1];
	va_list ap;

	struct kthread_worker *w = kpi_malloc(sizeof(*w), M_DRM, M_WAITOK);
	va_start(ap, fmt);
	vsnprintf(name, sizeof(name), fmt, ap);
	va_end(ap);
	w->tq = taskq_create(name, 1, IPL_HIGH, 0);
	
	return w;
}

void
kthread_destroy_worker(struct kthread_worker *worker)
{
	taskq_destroy(worker->tq);
	kpi_free(worker, M_DRM, sizeof(*worker));
	
}

void
kthread_init_work(struct kthread_work *work, void (*func)(struct kthread_work *))
{
	work->tq = NULL;
	task_set(&work->task, (void (*)(void *))func, work);
}

bool
kthread_queue_work(struct kthread_worker *worker, struct kthread_work *work)
{
	work->tq = worker->tq;
	return task_add(work->tq, &work->task);
}

bool
kthread_cancel_work_sync(struct kthread_work *work)
{
	return task_del(work->tq, &work->task);
}

void
kthread_flush_work(struct kthread_work *work)
{
	if (cold)
		return;

	if (work->tq)
		taskq_barrier(work->tq);
}

void
kthread_flush_worker(struct kthread_worker *worker)
{
	if (cold)
		return;

	if (worker->tq)
		taskq_barrier(worker->tq);
}

struct kthread *
kthread_lookup(struct task_struct *p)
{
	struct kthread *thread;

	mtx_enter(&kthread_mtx);
	LIST_FOREACH(thread, &kthread_list, next) {
		if (thread->proc == p)
			break;
	}
	mtx_leave(&kthread_mtx);
	KASSERT(thread);

	return thread;
}

int
kthread_should_park(void)
{
	struct kthread *thread = kthread_lookup(curproc);
	return (thread->flags & KTHREAD_SHOULDPARK);
}

void
kthread_parkme(void)
{
	struct kthread *thread = kthread_lookup(curproc);

	while (thread->flags & KTHREAD_SHOULDPARK) {
		thread->flags |= KTHREAD_PARKED;
		wakeup(thread);
		tsleep_nsec(thread, PPAUSE, "parkme", MSEC_TO_NSEC(10));	/* 9front: see kthread_stop */
		thread->flags &= ~KTHREAD_PARKED;
	}
}

void
kthread_park(struct task_struct *p)
{
	struct kthread *thread = kthread_lookup(p);

	while ((thread->flags & KTHREAD_PARKED) == 0) {
		thread->flags |= KTHREAD_SHOULDPARK;
		wake_up_process(thread->proc);
		tsleep_nsec(thread, PPAUSE, "park", MSEC_TO_NSEC(10));	/* 9front: see kthread_stop */
	}
}

void
kthread_unpark(struct task_struct *p)
{
	struct kthread *thread = kthread_lookup(p);

	thread->flags &= ~KTHREAD_SHOULDPARK;
	wakeup(thread);
}

int
kthread_should_stop(void)
{
	struct kthread *thread = kthread_lookup(curproc);
	return (thread->flags & KTHREAD_SHOULDSTOP);
}

int
kthread_stop(struct task_struct *p)
{
	struct kthread *thread = kthread_lookup(p);
	int ret;

	while ((thread->flags & KTHREAD_STOPPED) == 0) {
		thread->flags |= KTHREAD_SHOULDSTOP;
		kthread_unpark(p);
		wake_up_process(thread->proc);
		/* 9front: a time limit: no kernel lock orders this sleep before the thread's wakeup */
		tsleep_nsec(thread, PPAUSE, "stop", MSEC_TO_NSEC(10));
	}
	ret = thread->ret;
	mtx_enter(&kthread_mtx);
	LIST_REMOVE(thread, next);
	mtx_leave(&kthread_mtx);
	kpi_free(thread, M_DRM, sizeof(*thread));
	return ret;	/* 9front: Linux's result */
}


vaddr_t kmap_atomic_va;
int kmap_atomic_inuse;

void
print_hex_dump(const char *level, const char *prefix_str, int prefix_type,
    int rowsize, int groupsize, const void *buf, size_t len, bool ascii)
{
	const uint8_t *cbuf = buf;
	int i;

	for (i = 0; i < len; i++) {
		if ((i % rowsize) == 0)
			printf("%s", prefix_str);
		printf("%02x", cbuf[i]);
		if ((i % rowsize) == (rowsize - 1))
			printf("\n");
		else
			printf(" ");
	}
}

void *
memchr_inv(const void *s, int c, size_t n)
{
	if (n != 0) {
		const unsigned char *p = s;

		do {
			if (*p++ != (unsigned char)c)
				return ((void *)(p - 1));
		} while (--n != 0);
	}
	return (NULL);
}

#undef RB_ROOT
#define RB_ROOT(head)	(head)->rbh_root

/*
 * This is a fairly minimal implementation of the Linux "idr" API.  It
 * probably isn't very efficient, and definitely isn't RCU safe.  The
 * pre-load buffer is global instead of per-cpu; we rely on the kernel
 * lock to make this work.  We do randomize our IDs in order to make
 * them harder to guess.
 */

int idr_cmp(struct idr_entry *, struct idr_entry *);
SPLAY_PROTOTYPE(idr_tree, idr_entry, entry, idr_cmp);

struct pool idr_pool;
struct idr_entry *idr_entry_cache;

void
idr_init(struct idr *idr)
{
	SPLAY_INIT(&idr->tree);
}

void
idr_destroy(struct idr *idr)
{
	struct idr_entry *id;

	while ((id = SPLAY_MIN(idr_tree, &idr->tree))) {
		SPLAY_REMOVE(idr_tree, &idr->tree, id);
		pool_put(&idr_pool, id);
	}
}

void
idr_preload(unsigned int gfp_mask)
{
	int flags = (gfp_mask & GFP_NOWAIT) ? PR_NOWAIT : PR_WAITOK;


	if (idr_entry_cache == NULL)
		idr_entry_cache = pool_get(&idr_pool, flags);
}

/* [start, end) */
int
idr_alloc(struct idr *idr, void *ptr, int start, int end, gfp_t gfp_mask)
{
	int flags = (gfp_mask & GFP_NOWAIT) ? PR_NOWAIT : PR_WAITOK;
	struct idr_entry *id;
	int begin;


	if (idr_entry_cache) {
		id = idr_entry_cache;
		idr_entry_cache = NULL;
	} else {
		id = pool_get(&idr_pool, flags);
		if (id == NULL)
			return -ENOMEM;
	}

	if (end <= 0)
		end = INT_MAX;

#ifdef notyet
	id->id = begin = start + arc4random_uniform(end - start);
#else
	id->id = begin = start;
#endif
	while (SPLAY_INSERT(idr_tree, &idr->tree, id)) {
		if (id->id == end)
			id->id = start;
		else
			id->id++;
		if (id->id == begin) {
			pool_put(&idr_pool, id);
			return -ENOSPC;
		}
	}
	id->ptr = ptr;
	return id->id;
}

void *
idr_replace(struct idr *idr, void *ptr, unsigned long id)
{
	struct idr_entry find, *res;
	void *old;

	find.id = id;
	res = SPLAY_FIND(idr_tree, &idr->tree, &find);
	if (res == NULL)
		return ERR_PTR(-ENOENT);
	old = res->ptr;
	res->ptr = ptr;
	return old;
}

void *
idr_remove(struct idr *idr, unsigned long id)
{
	struct idr_entry find, *res;
	void *ptr = NULL;

	find.id = id;
	res = SPLAY_FIND(idr_tree, &idr->tree, &find);
	if (res) {
		SPLAY_REMOVE(idr_tree, &idr->tree, res);
		ptr = res->ptr;
		pool_put(&idr_pool, res);
	}
	return ptr;
}

void *
idr_find(struct idr *idr, unsigned long id)
{
	struct idr_entry find, *res;

	find.id = id;
	res = SPLAY_FIND(idr_tree, &idr->tree, &find);
	if (res == NULL)
		return NULL;
	return res->ptr;
}

void *
idr_get_next(struct idr *idr, int *id)
{
	struct idr_entry *res;

	SPLAY_FOREACH(res, idr_tree, &idr->tree) {
		if (res->id >= *id) {
			*id = res->id;
			return res->ptr;
		}
	}

	return NULL;
}

int
idr_for_each(struct idr *idr, int (*func)(int, void *, void *), void *data)
{
	struct idr_entry *id;
	int ret;

	SPLAY_FOREACH(id, idr_tree, &idr->tree) {
		ret = func(id->id, id->ptr, data);
		if (ret)
			return ret;
	}

	return 0;
}

int
idr_cmp(struct idr_entry *a, struct idr_entry *b)
{
	return (a->id < b->id ? -1 : a->id > b->id);
}

SPLAY_GENERATE(idr_tree, idr_entry, entry, idr_cmp);

void
ida_init(struct ida *ida)
{
	idr_init(&ida->idr);
}

void
ida_destroy(struct ida *ida)
{
	idr_destroy(&ida->idr);
}

int
ida_simple_get(struct ida *ida, unsigned int start, unsigned int end,
    gfp_t gfp_mask)
{
	return idr_alloc(&ida->idr, NULL, start, end, gfp_mask);
}

void
ida_simple_remove(struct ida *ida, unsigned int id)
{
	idr_remove(&ida->idr, id);
}

/* [start, end] */
int
ida_alloc_range(struct ida *ida, unsigned int start, unsigned int end, gfp_t gfp)
{
	return idr_alloc(&ida->idr, NULL, start, end + 1, gfp);
}

int
ida_alloc_min(struct ida *ida, unsigned int min, gfp_t gfp)
{
	return idr_alloc(&ida->idr, NULL, min, INT_MAX, gfp);
}

int
ida_alloc_max(struct ida *ida, unsigned int max, gfp_t gfp)
{
	return idr_alloc(&ida->idr, NULL, 0, max - 1, gfp);
}

void
ida_free(struct ida *ida, unsigned int id)
{
	idr_remove(&ida->idr, id);
}

int
xarray_cmp(struct xarray_entry *a, struct xarray_entry *b)
{
	return (a->id < b->id ? -1 : a->id > b->id);
}

SPLAY_PROTOTYPE(xarray_tree, xarray_entry, entry, xarray_cmp);
struct pool xa_pool;
SPLAY_GENERATE(xarray_tree, xarray_entry, entry, xarray_cmp);

void
xa_init_flags(struct xarray *xa, gfp_t flags)
{
	SPLAY_INIT(&xa->xa_tree);
	if (flags & XA_FLAGS_LOCK_IRQ)
		mtx_init(&xa->xa_lock, IPL_TTY);
	else
		mtx_init(&xa->xa_lock, IPL_NONE);
	xa->xa_flags = flags;
}

void
xa_destroy(struct xarray *xa)
{
	struct xarray_entry *id;

	while ((id = SPLAY_MIN(xarray_tree, &xa->xa_tree))) {
		SPLAY_REMOVE(xarray_tree, &xa->xa_tree, id);
		pool_put(&xa_pool, id);
	}
}

/* Don't wrap ids. */
int
__xa_alloc(struct xarray *xa, u32 *id, void *entry, struct xarray_range xr,
    gfp_t gfp)
{
	struct xarray_entry *xid;
	uint32_t start = xr.start;
	uint32_t end = xr.end;

	if (start == 0 && (xa->xa_flags & XA_FLAGS_ALLOC1))
		start = 1;

	if (gfp & GFP_NOWAIT) {
		xid = pool_get(&xa_pool, PR_NOWAIT);
	} else {
		mtx_leave(&xa->xa_lock);
		xid = pool_get(&xa_pool, PR_WAITOK);
		mtx_enter(&xa->xa_lock);
	}

	if (xid == NULL)
		return -ENOMEM;

	xid->id = start;

	while (SPLAY_INSERT(xarray_tree, &xa->xa_tree, xid)) {
		if (xid->id == end)
			xid->id = start;
		else
			xid->id++;
		if (xid->id == start) {
			pool_put(&xa_pool, xid);
			return -EBUSY;
		}
	}
	xid->ptr = entry;
	*id = xid->id;
	return 0;
}

/*
 * [start, end] Wrap ids and store next id.
 * 9front: from *next on, as Linux's (7.3 allocates amdgpu's PASIDs and
 * GuC's TLB seqnos so: a freed id is not taken again at once); the
 * earlier one ignored next.  OpenBSD's of a9becb03c6c gives up when the
 * id at *next is taken and does not end when all are and *next < start:
 * here the search ends where it began.
 */
int
__xa_alloc_cyclic(struct xarray *xa, u32 *id, void *entry,
    struct xarray_range xr, u32 *next, gfp_t gfp)
{
	struct xarray_entry *xid;
	uint32_t start = xr.start;
	uint32_t end = xr.end;
	uint32_t begin;

	if (start == 0 && (xa->xa_flags & XA_FLAGS_ALLOC1))
		start = 1;

	if (gfp & GFP_NOWAIT) {
		xid = pool_get(&xa_pool, PR_NOWAIT);
	} else {
		mtx_leave(&xa->xa_lock);
		xid = pool_get(&xa_pool, PR_WAITOK);
		mtx_enter(&xa->xa_lock);
	}

	if (xid == NULL)
		return -ENOMEM;

	if (*next < start || *next > end)
		begin = start;
	else
		begin = *next;
	xid->id = begin;

	while (SPLAY_INSERT(xarray_tree, &xa->xa_tree, xid)) {
		if (xid->id == end)
			xid->id = start;
		else
			xid->id++;
		if (xid->id == begin) {
			pool_put(&xa_pool, xid);
			return -EBUSY;
		}
	}
	xid->ptr = entry;
	if (xid->id == end)
		*next = start;
	else
		*next = xid->id + 1;
	*id = xid->id;
	return 0;
}

void *
__xa_erase(struct xarray *xa, unsigned long index)
{
	struct xarray_entry find, *res;
	void *ptr = NULL;

	find.id = index;
	res = SPLAY_FIND(xarray_tree, &xa->xa_tree, &find);
	if (res) {
		SPLAY_REMOVE(xarray_tree, &xa->xa_tree, res);
		ptr = res->ptr;
		pool_put(&xa_pool, res);
	}
	return ptr;
}

void *
__xa_load(struct xarray *xa, unsigned long index)
{
	struct xarray_entry find, *res;

	find.id = index;
	res = SPLAY_FIND(xarray_tree, &xa->xa_tree, &find);
	if (res == NULL)
		return NULL;
	return res->ptr;
}

void *
__xa_store(struct xarray *xa, unsigned long index, void *entry, gfp_t gfp)
{
	struct xarray_entry find, *res;
	void *prev;

	if (entry == NULL)
		return __xa_erase(xa, index);

	find.id = index;
	res = SPLAY_FIND(xarray_tree, &xa->xa_tree, &find);
	if (res != NULL) {
		/* index exists */
		/* XXX Multislot entries updates not implemented yet */
		prev = res->ptr;
		res->ptr = entry;
		return prev;
	}

	/* index not found, add new */
	if (gfp & GFP_NOWAIT) {
		res = pool_get(&xa_pool, PR_NOWAIT);
	} else {
		mtx_leave(&xa->xa_lock);
		res = pool_get(&xa_pool, PR_WAITOK);
		mtx_enter(&xa->xa_lock);
	}
	if (res == NULL)
		return XA_ERROR(-ENOMEM);
	res->id = index;
	res->ptr = entry;
	if (SPLAY_INSERT(xarray_tree, &xa->xa_tree, res) != NULL)
		return XA_ERROR(-EINVAL);
	return NULL; /* no prev entry at index */
}

void *
xa_get_next(struct xarray *xa, unsigned long *index)
{
	struct xarray_entry *res;

	SPLAY_FOREACH(res, xarray_tree, &xa->xa_tree) {
		if (res->id >= *index) {
			*index = res->id;
			return res->ptr;
		}
	}

	return NULL;
}

/* 9front: what Linux 7.3's users take besides OpenBSD's */
/* xa_insert: entry goes to index if nothing is there (-EBUSY if something is) */
int
__xa_insert(struct xarray *xa, unsigned long index, void *entry, gfp_t gfp)
{
	void *cur;

	if (__xa_load(xa, index) != NULL)
		return -EBUSY;
	if (WARN_ON_ONCE(entry == NULL))	/* a reservation: not kept here */
		return -EINVAL;
	cur = __xa_store(xa, index, entry, gfp);
	return xa_is_err(cur) ? xa_err(cur) : 0;
}

int
xa_insert(struct xarray *xa, unsigned long index, void *entry, gfp_t gfp)
{
	int r;

	xa_lock(xa);
	r = __xa_insert(xa, index, entry, gfp);
	xa_unlock(xa);
	return r;
}

/*
 * xa_find: the first entry at *index or after, up to max; *index becomes
 * its index.  Only XA_PRESENT: the tree keeps no marks (nothing we build
 * sets one).
 */
void *
xa_find(struct xarray *xa, unsigned long *index, unsigned long max, xa_mark_t mark)
{
	struct xarray_entry *e;
	void *p;

	if (WARN_ON_ONCE(mark != XA_PRESENT))
		return NULL;
	p = NULL;
	xa_lock(xa);
	SPLAY_FOREACH(e, xarray_tree, &xa->xa_tree) {
		if (e->id > max)
			break;
		if (e->id >= *index) {
			*index = e->id;
			p = e->ptr;
			break;
		}
	}
	xa_unlock(xa);
	return p;
}

/* xa_find_after: as xa_find, from the index after *index */
void *
xa_find_after(struct xarray *xa, unsigned long *index, unsigned long max, xa_mark_t mark)
{
	unsigned long i;
	void *p;

	if (*index >= max)
		return NULL;
	i = *index + 1;
	p = xa_find(xa, &i, max, mark);
	if (p != NULL)
		*index = i;
	return p;
}


/* 9front: the fence itself is in dma_fence.c (from drm-kmod's, BSD) */

static atomic64_t drm_fence_context_count = ATOMIC64_INIT(1);

uint64_t
dma_fence_context_alloc(unsigned int num)
{
  return atomic64_add_return(num, &drm_fence_context_count) - num;
}

struct default_wait_cb {
	struct dma_fence_cb base;
	struct task_struct *proc;
};

static void
dma_fence_default_wait_cb(struct dma_fence *fence, struct dma_fence_cb *cb)
{
	struct default_wait_cb *wait =
	    container_of(cb, struct default_wait_cb, base);
	wake_up_process(wait->proc);
}

long
dma_fence_default_wait(struct dma_fence *fence, bool intr, signed long timeout)
{
	long ret = timeout ? timeout : 1;
	unsigned long end;
	int err;
	struct default_wait_cb cb;

	KASSERT(timeout <= INT_MAX);

	if (dma_fence_test_signaled_flag(fence))
		return ret;

	mtx_enter(dma_fence_spinlock(fence));

	if (!kpi_dma_fence_arm(fence))
		goto out;

	if (timeout == 0) {
		ret = 0;
		goto out;
	}

	cb.base.func = dma_fence_default_wait_cb;
	cb.proc = curproc;
	list_add(&cb.base.node, &fence->cb_list);

	/* 9front: the time left signed, 0 once past end (unsigned, end - jiffies wrapped to a huge value, negative as long) */
	end = jiffies + timeout;
	for (ret = timeout; ret > 0; ret = MAX(0L, (long)(end - jiffies))) {
		if (dma_fence_test_signaled_flag(fence))
			break;
		err = msleep_ticks(curproc, dma_fence_spinlock(fence), intr ? PCATCH : 0,
		    "dmafence", ret);
		if (err == EINTR || err == ERESTART) {
			ret = -ERESTARTSYS;
			break;
		}
	}

	if (!list_empty(&cb.base.node))
		list_del(&cb.base.node);
out:
	mtx_leave(dma_fence_spinlock(fence));

	return ret;
}

static bool
dma_fence_test_signaled_any(struct dma_fence **fences, uint32_t count,
    uint32_t *idx)
{
	int i;

	for (i = 0; i < count; ++i) {
		struct dma_fence *fence = fences[i];
		if (dma_fence_test_signaled_flag(fence)) {
			if (idx)
				*idx = i;
			return true;
		}
	}
	return false;
}

long
dma_fence_wait_any_timeout(struct dma_fence **fences, uint32_t count,
    bool intr, long timeout, uint32_t *idx)
{
	struct default_wait_cb *cb;
	long ret = timeout;
	unsigned long end;
	int i, err;

	KASSERT(timeout <= INT_MAX);

	if (timeout == 0) {
		for (i = 0; i < count; i++) {
			if (dma_fence_is_signaled(fences[i])) {
				if (idx)
					*idx = i;
				return 1;
			}
		}
		return 0;
	}

	cb = mallocarray(count, sizeof(*cb), M_DRM, M_WAITOK|M_CANFAIL|M_ZERO);
	if (cb == NULL)
		return -ENOMEM;

	for (i = 0; i < count; i++) {
		struct dma_fence *fence = fences[i];
		cb[i].proc = curproc;
		if (dma_fence_add_callback(fence, &cb[i].base,
		    dma_fence_default_wait_cb)) {
			if (idx)
				*idx = i;
			goto cb_cleanup;
		}
	}

	/* 9front: the time left signed, 0 once past end (unsigned, end - jiffies wrapped to a huge value, negative as long) */
	end = jiffies + timeout;
	for (ret = timeout; ret > 0; ret = MAX(0L, (long)(end - jiffies))) {
		if (dma_fence_test_signaled_any(fences, count, idx))
			break;
		err = tsleep(curproc, intr ? PCATCH : 0, "dfwat", ret);
		if (err == EINTR || err == ERESTART) {
			ret = -ERESTARTSYS;
			break;
		}
	}

cb_cleanup:
	while (i-- > 0)
		dma_fence_remove_callback(fences[i], &cb[i].base);
	kpi_free(cb, M_DRM, count * sizeof(*cb));
	return ret;
}

void
dma_fence_set_deadline(struct dma_fence *f, ktime_t t)
{
	const struct dma_fence_ops *ops;

	rcu_read_lock();
	ops = rcu_dereference(f->ops);
	if (ops && ops->set_deadline && !dma_fence_is_signaled(f)) {
		rcu_read_unlock();
		ops->set_deadline(f, t);
		return;
	}
	rcu_read_unlock();
}

static struct dma_fence dma_fence_stub;
static struct spinlock dma_fence_stub_mtx = MUTEX_INITIALIZER(IPL_TTY);

static const char *
dma_fence_stub_get_name(struct dma_fence *fence)
{
	return "stub";
}

static const struct dma_fence_ops dma_fence_stub_ops = {
	.get_driver_name = dma_fence_stub_get_name,
	.get_timeline_name = dma_fence_stub_get_name,
};

struct dma_fence *
dma_fence_get_stub(void)
{
	mtx_enter(&dma_fence_stub_mtx);
	/* (signalled, its ops are gone: initialized says it is made) */
	if (!dma_fence_was_initialized(&dma_fence_stub)) {
		dma_fence_init(&dma_fence_stub, &dma_fence_stub_ops,
		    &dma_fence_stub_mtx, 0, 0);
		set_bit(DMA_FENCE_FLAG_ENABLE_SIGNAL_BIT, &dma_fence_stub.flags);
		dma_fence_signal_locked(&dma_fence_stub);
	}
	mtx_leave(&dma_fence_stub_mtx);

	return dma_fence_get(&dma_fence_stub);
}

struct dma_fence *
dma_fence_allocate_private_stub(ktime_t ts)
{
	struct dma_fence *f = kpi_malloc(sizeof(*f), M_DRM,
	    M_ZERO | M_WAITOK | M_CANFAIL);
	if (f == NULL)
		return NULL;
	dma_fence_init(f, &dma_fence_stub_ops, NULL, 0, 0);
	set_bit(DMA_FENCE_FLAG_ENABLE_SIGNAL_BIT, &f->flags);
	dma_fence_signal_timestamp(f, ts);
	return f;
}

static const char *
dma_fence_array_get_driver_name(struct dma_fence *fence)
{
	return "dma_fence_array";
}

static const char *
dma_fence_array_get_timeline_name(struct dma_fence *fence)
{
	return "unbound";
}

static void
irq_dma_fence_array_work(void *arg)
{
	struct dma_fence_array *dfa = (struct dma_fence_array *)arg;
	dma_fence_signal(&dfa->base);
	dma_fence_put(&dfa->base);
}

static void
dma_fence_array_cb_func(struct dma_fence *f, struct dma_fence_cb *cb)
{
	struct dma_fence_array_cb *array_cb =
	    container_of(cb, struct dma_fence_array_cb, cb);
	struct dma_fence_array *dfa = array_cb->array;
	
	if (atomic_dec_and_test(&dfa->num_pending))
		timeout_add(&dfa->to, 1);
	else
		dma_fence_put(&dfa->base);
}

static bool
dma_fence_array_enable_signaling(struct dma_fence *fence)
{
	struct dma_fence_array *dfa = to_dma_fence_array(fence);
	struct dma_fence_array_cb *cb = (void *)(&dfa[1]);
	int i;

	for (i = 0; i < dfa->num_fences; ++i) {
		cb[i].array = dfa;
		dma_fence_get(&dfa->base);
		if (dma_fence_add_callback(dfa->fences[i], &cb[i].cb,
		    dma_fence_array_cb_func)) {
			dma_fence_put(&dfa->base);
			if (atomic_dec_and_test(&dfa->num_pending))
				return false;
		}
	}
	
	return true;
}

static bool
dma_fence_array_signaled(struct dma_fence *fence)
{
	struct dma_fence_array *dfa = to_dma_fence_array(fence);

	return atomic_read(&dfa->num_pending) <= 0;
}

static void
dma_fence_array_release(struct dma_fence *fence)
{
	struct dma_fence_array *dfa = to_dma_fence_array(fence);
	int i;

	for (i = 0; i < dfa->num_fences; ++i)
		dma_fence_put(dfa->fences[i]);

	kpi_free(dfa->fences, M_DRM, 0);
	dma_fence_free(fence);
}

/* 9front: Linux 6.12 split create into alloc and init; 6.16 dropped signal_on_any */
struct dma_fence_array *
dma_fence_array_alloc(int num_fences)
{
	return kpi_malloc(sizeof(struct dma_fence_array) +
	    (num_fences * sizeof(struct dma_fence_array_cb)),
	    M_DRM, M_WAITOK|M_CANFAIL|M_ZERO);
}

void
dma_fence_array_init(struct dma_fence_array *dfa, int num_fences,
    struct dma_fence **fences, u64 context, unsigned seqno)
{
	mtx_init(&dfa->lock, IPL_TTY);
	dma_fence_init(&dfa->base, &dma_fence_array_ops, &dfa->lock,
	    context, seqno);
	timeout_set(&dfa->to, irq_dma_fence_array_work, dfa);

	dfa->num_fences = num_fences;
	atomic_set(&dfa->num_pending, num_fences);
	dfa->fences = fences;
}

struct dma_fence_array *
dma_fence_array_create(int num_fences, struct dma_fence **fences, u64 context,
    unsigned seqno)
{
	struct dma_fence_array *dfa = dma_fence_array_alloc(num_fences);

	if (dfa == NULL)
		return NULL;
	dma_fence_array_init(dfa, num_fences, fences, context, seqno);
	return dfa;
}

struct dma_fence *
dma_fence_array_first(struct dma_fence *f)
{
	struct dma_fence_array *dfa;

	if (f == NULL)
		return NULL;

	if ((dfa = to_dma_fence_array(f)) == NULL)
		return f;

	if (dfa->num_fences > 0)
		return dfa->fences[0];

	return NULL;
}

struct dma_fence *
dma_fence_array_next(struct dma_fence *f, unsigned int i)
{
	struct dma_fence_array *dfa;

	if (f == NULL)
		return NULL;

	if ((dfa = to_dma_fence_array(f)) == NULL)
		return NULL;

	if (i < dfa->num_fences)
		return dfa->fences[i];

	return NULL;
}

const struct dma_fence_ops dma_fence_array_ops = {
	.get_driver_name = dma_fence_array_get_driver_name,
	.get_timeline_name = dma_fence_array_get_timeline_name,
	.enable_signaling = dma_fence_array_enable_signaling,
	.signaled = dma_fence_array_signaled,
	.release = dma_fence_array_release,
};

int
dma_fence_chain_find_seqno(struct dma_fence **df, uint64_t seqno)
{
	struct dma_fence_chain *chain;
	struct dma_fence *fence;

	if (seqno == 0)
		return 0;

	if ((chain = to_dma_fence_chain(*df)) == NULL)
		return -EINVAL;

	fence = &chain->base;
	if (fence->seqno < seqno)
		return -EINVAL;

	dma_fence_chain_for_each(*df, fence) {
		if ((*df)->context != fence->context)
			break;

		chain = to_dma_fence_chain(*df);
		if (chain->prev_seqno < seqno)
			break;
	}
	dma_fence_put(fence);

	return 0;
}

void
dma_fence_chain_init(struct dma_fence_chain *chain, struct dma_fence *prev,
    struct dma_fence *fence, uint64_t seqno)
{
	uint64_t context;

	chain->fence = fence;
	chain->prev = prev;
	mtx_init(&chain->lock, IPL_TTY);

	/* if prev is a chain */
	if (to_dma_fence_chain(prev) != NULL) {
		if (__dma_fence_is_later(prev, seqno, prev->seqno)) {
			chain->prev_seqno = prev->seqno;
			context = prev->context;
		} else {
			chain->prev_seqno = 0;
			context = dma_fence_context_alloc(1);
			seqno = prev->seqno;
		}
	} else {
		chain->prev_seqno = 0;
		context = dma_fence_context_alloc(1);
	}

	/* 9front: 64-bit sequence numbers (Linux 6.17: a flag, was ops.use_64bit_seqno) */
	dma_fence_init64(&chain->base, &dma_fence_chain_ops, &chain->lock,
	    context, seqno);
}

static const char *
dma_fence_chain_get_driver_name(struct dma_fence *fence)
{
	return "dma_fence_chain";
}

static const char *
dma_fence_chain_get_timeline_name(struct dma_fence *fence)
{
	return "unbound";
}

static bool dma_fence_chain_enable_signaling(struct dma_fence *);

static void
dma_fence_chain_timo(void *arg)
{
	struct dma_fence_chain *chain = (struct dma_fence_chain *)arg;

	if (dma_fence_chain_enable_signaling(&chain->base) == false)
		dma_fence_signal(&chain->base);
	dma_fence_put(&chain->base);
}

static void
dma_fence_chain_cb(struct dma_fence *f, struct dma_fence_cb *cb)
{
	struct dma_fence_chain *chain =
	    container_of(cb, struct dma_fence_chain, cb);
	timeout_set(&chain->to, dma_fence_chain_timo, chain);
	timeout_add(&chain->to, 1);
	dma_fence_put(f);
}

static bool
dma_fence_chain_enable_signaling(struct dma_fence *fence)
{
	struct dma_fence_chain *chain, *h;
	struct dma_fence *f;

	h = to_dma_fence_chain(fence);
	dma_fence_get(&h->base);
	dma_fence_chain_for_each(fence, &h->base) {
		chain = to_dma_fence_chain(fence);
		if (chain == NULL)
			f = fence;
		else
			f = chain->fence;

		dma_fence_get(f);
		if (!dma_fence_add_callback(f, &h->cb, dma_fence_chain_cb)) {
			dma_fence_put(fence);
			return true;
		}
		dma_fence_put(f);
	}
	dma_fence_put(&h->base);
	return false;
}

static bool
dma_fence_chain_signaled(struct dma_fence *fence)
{
	struct dma_fence_chain *chain;
	struct dma_fence *f;

	dma_fence_chain_for_each(fence, fence) {
		chain = to_dma_fence_chain(fence);
		if (chain == NULL)
			f = fence;
		else
			f = chain->fence;

		if (dma_fence_is_signaled(f) == false) {
			dma_fence_put(fence);
			return false;
		}
	}
	return true;
}

static void
dma_fence_chain_release(struct dma_fence *fence)
{
	struct dma_fence_chain *chain = to_dma_fence_chain(fence);
	struct dma_fence_chain *prev_chain;
	struct dma_fence *prev;

	for (prev = chain->prev; prev != NULL; prev = chain->prev) {
		if (kref_read(&prev->refcount) > 1)
			break;
		if ((prev_chain = to_dma_fence_chain(prev)) == NULL)
			break;
		chain->prev = prev_chain->prev;
		prev_chain->prev = NULL;
		dma_fence_put(prev);
	}
	dma_fence_put(prev);
	dma_fence_put(chain->fence);
	dma_fence_free(fence);
}

/*
 * 9front: a reference to the chain's predecessor as Linux takes it
 * (dma_fence_chain_get_prev): under RCU, only while the fence still has
 * one and is still the predecessor.  A plain dma_fence_get(chain->prev)
 * raced with another walker that swapped chain->prev and put the old one
 * to 0: its memory went back to a typesafe-by-RCU cache, our get revived
 * it, the later put freed it twice (st-dma-fence-chain, 8 processors;
 * OpenBSD of a9becb03c6c has the plain get).
 */
static struct dma_fence *
dma_fence_chain_get_prev(struct dma_fence_chain *chain)
{
	struct dma_fence *prev;

	rcu_read_lock();
	prev = dma_fence_get_rcu_safe(&chain->prev);
	rcu_read_unlock();
	return prev;
}

struct dma_fence *
dma_fence_chain_walk(struct dma_fence *fence)
{
	struct dma_fence_chain *chain = to_dma_fence_chain(fence), *prev_chain;
	struct dma_fence *prev, *new_prev, *tmp;

	if (chain == NULL) {
		dma_fence_put(fence);
		return NULL;
	}

	while ((prev = dma_fence_chain_get_prev(chain)) != NULL) {
		prev_chain = to_dma_fence_chain(prev);
		if (prev_chain != NULL) {
			if (!dma_fence_is_signaled(prev_chain->fence))
				break;
			new_prev = dma_fence_chain_get_prev(prev_chain);
		} else {
			if (!dma_fence_is_signaled(prev))
				break;
			new_prev = NULL;
		}
		tmp = atomic_cas_ptr(&chain->prev, prev, new_prev);
		dma_fence_put(tmp == prev ? prev : new_prev);
		dma_fence_put(prev);
	}

	dma_fence_put(fence);
	return prev;
}

const struct dma_fence_ops dma_fence_chain_ops = {
	.get_driver_name = dma_fence_chain_get_driver_name,
	.get_timeline_name = dma_fence_chain_get_timeline_name,
	.enable_signaling = dma_fence_chain_enable_signaling,
	.signaled = dma_fence_chain_signaled,
	.release = dma_fence_chain_release,
};





static wait_queue_head_t bit_waitq;
wait_queue_head_t var_waitq;
struct spinlock wait_bit_mtx = MUTEX_INITIALIZER(IPL_TTY);

int
wait_on_bit(unsigned long *word, int bit, unsigned mode)
{
	int err;

	if (!test_bit(bit, word))
		return 0;

	mtx_enter(&wait_bit_mtx);
	while (test_bit(bit, word)) {
		err = msleep_nsec(word, &wait_bit_mtx, PWAIT | mode, "wtb",
		    INFSLP);
		if (err) {
			mtx_leave(&wait_bit_mtx);
			return 1;
		}
	}
	mtx_leave(&wait_bit_mtx);
	return 0;
}

int
wait_on_bit_timeout(unsigned long *word, int bit, unsigned mode, int timo)
{
	int err;

	if (!test_bit(bit, word))
		return 0;

	mtx_enter(&wait_bit_mtx);
	while (test_bit(bit, word)) {
		err = msleep_ticks(word, &wait_bit_mtx, PWAIT | mode, "wtb", timo);
		if (err) {
			mtx_leave(&wait_bit_mtx);
			return 1;
		}
	}
	mtx_leave(&wait_bit_mtx);
	return 0;
}

void
wake_up_bit(void *word, int bit)
{
	mtx_enter(&wait_bit_mtx);
	wakeup(word);
	mtx_leave(&wait_bit_mtx);
}

void
clear_and_wake_up_bit(int bit, void *word)
{
	clear_bit(bit, word);
	wake_up_bit(word, bit);
}

wait_queue_head_t *
bit_waitqueue(void *word, int bit)
{
	/* XXX hash table of wait queues? */
	return &bit_waitq;
}

wait_queue_head_t *
__var_waitqueue(void *p)
{
	/* XXX hash table of wait queues? */
	return &bit_waitq;
}

struct workqueue_struct *system_wq;
struct workqueue_struct *system_highpri_wq;
struct workqueue_struct *system_unbound_wq;
struct workqueue_struct *system_long_wq;
struct taskq *taskletq;
struct taskq *kpi_irq_workq;

void
drm_linux_init(void)
{
	system_wq = (struct workqueue_struct *)
	    taskq_create("drmwq", 4, IPL_HIGH, 0);
	system_highpri_wq = (struct workqueue_struct *)
	    taskq_create("drmhpwq", 4, IPL_HIGH, 0);
	system_unbound_wq = (struct workqueue_struct *)
	    taskq_create("drmubwq", 4, IPL_HIGH, 0);
	system_long_wq = (struct workqueue_struct *)
	    taskq_create("drmlwq", 4, IPL_HIGH, 0);

	taskletq = taskq_create("drmtskl", 1, IPL_HIGH, 0);
	/* 9front: irq_work one at a time (linux/irq_work.h) */
	kpi_irq_workq = taskq_create("drmirqw", 1, IPL_HIGH, 0);

	init_waitqueue_head(&bit_waitq);
	init_waitqueue_head(&var_waitq);

	pool_init(&idr_pool, sizeof(struct idr_entry), 0, IPL_TTY, 0,
	    "idrpl", NULL);
	pool_init(&xa_pool, sizeof(struct xarray_entry), 0, IPL_NONE, 0,
	    "xapl", NULL);

	kmap_atomic_va =
	    (vaddr_t)km_alloc(PAGE_SIZE, &kv_any, &kp_none, &kd_waitok);

#if NACPI > 0
	if (acpi_softc) {
		memcpy(&acpi_gbl_FADT, acpi_softc->sc_fadt,
		    sizeof(acpi_gbl_FADT));
	}
#endif
}

void
drm_linux_exit(void)
{
	pool_destroy(&xa_pool);
	pool_destroy(&idr_pool);

	taskq_destroy(taskletq);

	taskq_destroy((struct taskq *)system_long_wq);
	taskq_destroy((struct taskq *)system_unbound_wq);
	taskq_destroy((struct taskq *)system_highpri_wq);
	taskq_destroy((struct taskq *)system_wq);
}

#define PCIE_ECAP_RESIZE_BAR	0x15
#define RBCAP0			0x04
#define RBCTRL0			0x08
#define RBCTRL_BARINDEX_MASK	0x07
#define RBCTRL_BARSIZE_MASK	0x1f00
#define RBCTRL_BARSIZE_SHIFT	8

TAILQ_HEAD(, shrinker) shrinkers = TAILQ_HEAD_INITIALIZER(shrinkers);

void *
bitmap_zalloc(u_int n, gfp_t flags)
{
	return kcalloc(BITS_TO_LONGS(n), sizeof(long), flags);
}

void
bitmap_free(void *p)
{
	kfree(p);
}

/* 9front: on Linux's atomic_t and struct mutex */
int
atomic_dec_and_mutex_lock(atomic_t *v, struct mutex *lock)
{
	if (atomic_add_unless(v, -1, 1))
		return 0;

	mutex_lock(lock);
	if (atomic_dec_return(v) == 0)
		return 1;
	mutex_unlock(lock);
	return 0;
}

#define START(node) ((node)->start)
#define LAST(node) ((node)->last)

struct interval_tree_node *
interval_tree_iter_first(struct rb_root_cached *root, unsigned long start,
    unsigned long last)
{
	struct interval_tree_node *node;
	struct rb_node *rb;

	for (rb = rb_first_cached(root); rb; rb = rb_next(rb)) {
		node = rb_entry(rb, typeof(*node), rb);
		if (LAST(node) >= start && START(node) <= last)
			return node;
	}
	return NULL;
}

void
interval_tree_remove(struct interval_tree_node *node,
    struct rb_root_cached *root) 
{
	rb_erase_cached(&node->rb, root);
}

void
interval_tree_insert(struct interval_tree_node *node,
    struct rb_root_cached *root)
{
	struct rb_node **iter = &root->rb_root.rb_node;
	struct rb_node *parent = NULL;
	struct interval_tree_node *iter_node;

	while (*iter) {
		parent = *iter;
		iter_node = rb_entry(*iter, struct interval_tree_node, rb);

		if (node->start < iter_node->start)
			iter = &(*iter)->rb_left;
		else
			iter = &(*iter)->rb_right;
	}

	rb_link_node(&node->rb, parent, iter);
	rb_insert_color_cached(&node->rb, root, false);
}


void
kfree_const(const void *addr)
{
        kfree(addr);
}

#include <linux/platform_device.h>

#ifdef BUS_DMA_FIXED

#include <linux/iommu.h>

#endif

#include <linux/component.h>

struct component {
	struct device *dev;
	struct device *adev;
	const struct component_ops *ops;
	SLIST_ENTRY(component) next;
};

SLIST_HEAD(,component) component_list = SLIST_HEAD_INITIALIZER(component_list);

struct component_match_entry {
	int (*compare)(struct device *, void *);
	void *data;
};

struct component_match {
	struct component_match_entry match[4];
	int nmatches;
};

#ifdef __HAVE_FDT

#include <linux/platform_device.h>
#include <dev/ofw/openfirm.h>
#include <dev/ofw/fdt.h>
#include <machine/fdt.h>

LIST_HEAD(, platform_device) pdev_list = LIST_HEAD_INITIALIZER(pdev_list);

#include <dev/ofw/ofw_clock.h>
#include <linux/clk.h>

#include <linux/gpio/consumer.h>
#include <dev/ofw/ofw_gpio.h>

struct gpio_desc {
	uint32_t gpios[4];
};

struct phy {
	int node;
	const char *name;
};

struct bus_type platform_bus_type;

#include <dev/ofw/ofw_misc.h>

#include <linux/of.h>
#include <linux/platform_device.h>

#endif

