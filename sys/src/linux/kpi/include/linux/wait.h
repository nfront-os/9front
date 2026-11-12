/*	$OpenBSD: wait.h,v 1.15 2025/12/01 09:25:03 jsg Exp $	*/
/*
 * Copyright (c) 2013, 2014, 2015 Mark Kettenis
 * Copyright (c) 2017 Martin Pieuchot
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

#ifndef _LINUX_WAIT_H
#define _LINUX_WAIT_H

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mutex.h>
#include <sys/proc.h>

#include <linux/list.h>
#include <linux/errno.h>
#include <linux/spinlock.h>

struct wait_queue_entry {
	unsigned int flags;
	void *private;
	int (*func)(struct wait_queue_entry *, unsigned, int, void *);
	struct list_head entry;
};

#define WQ_FLAG_WOKEN	(1 << 1)

typedef struct wait_queue_entry wait_queue_entry_t;

struct wait_queue_head {
	struct spinlock lock;
	struct list_head head;
};
typedef struct wait_queue_head wait_queue_head_t;

void	prepare_to_wait(wait_queue_head_t *, wait_queue_entry_t *, int);
void	finish_wait(wait_queue_head_t *, wait_queue_entry_t *);

static inline void
init_waitqueue_head(wait_queue_head_t *wqh)
{
	mtx_init(&wqh->lock, IPL_TTY);
	INIT_LIST_HEAD(&wqh->head);
}

#define __init_waitqueue_head(wqh, name, key)	init_waitqueue_head(wqh)

int autoremove_wake_function(struct wait_queue_entry *, unsigned int, int, void *);
int woken_wake_function(struct wait_queue_entry *, unsigned int, int, void *);

static inline void
init_wait_entry(wait_queue_entry_t *wqe, int flags)
{
	wqe->flags = flags;
	wqe->private = curproc;
	wqe->func = autoremove_wake_function;
	INIT_LIST_HEAD(&wqe->entry);
}

static inline void
__add_wait_queue(wait_queue_head_t *wqh, wait_queue_entry_t *wqe)
{
	list_add(&wqe->entry, &wqh->head);
}

static inline void
__add_wait_queue_entry_tail(wait_queue_head_t *wqh, wait_queue_entry_t *wqe)
{
	list_add_tail(&wqe->entry, &wqh->head);
}

static inline void
add_wait_queue(wait_queue_head_t *head, wait_queue_entry_t *new)
{
	mtx_enter(&head->lock);
	__add_wait_queue(head, new);
	mtx_leave(&head->lock);
}

static inline void
__remove_wait_queue(wait_queue_head_t *wqh, wait_queue_entry_t *wqe)
{
	list_del(&wqe->entry);
}

static inline void
remove_wait_queue(wait_queue_head_t *head, wait_queue_entry_t *old)
{
	mtx_enter(&head->lock);
	__remove_wait_queue(head, old);
	mtx_leave(&head->lock);
}

#define __wait_event_intr(wqh, condition, prio)				\
({									\
	long __ret = 0;							\
	struct wait_queue_entry __wq_entry;				\
									\
	init_wait_entry(&__wq_entry, 0);				\
	do {								\
		int __error, __wait;					\
									\
		KASSERT(!cold);						\
									\
		prepare_to_wait(&wqh, &__wq_entry, prio);		\
									\
		__wait = !(condition);					\
									\
		__error = sleep_finish(INFSLP, __wait);			\
									\
		if (__error == ERESTART || __error == EINTR) {		\
			__ret = -ERESTARTSYS;				\
			break;						\
		}							\
	} while (!(condition));						\
	finish_wait(&wqh, &__wq_entry);					\
	__ret;								\
})

#define __wait_event_intr_timeout(wqh, condition, timo, prio)		\
({									\
	long __ret = timo;						\
	struct wait_queue_entry __wq_entry;				\
									\
	init_wait_entry(&__wq_entry, 0);				\
	do {								\
		int __error, __wait;					\
		unsigned long deadline;					\
		uint64_t nsecs = jiffies_to_nsecs(__ret);		\
									\
		KASSERT(!cold);						\
									\
		prepare_to_wait(&wqh, &__wq_entry, prio);		\
		deadline = jiffies + __ret;				\
									\
		__wait = !(condition);					\
									\
		__error = sleep_finish(nsecs, __wait);			\
		if ((timo) > 0)						\
			__ret = deadline - jiffies;			\
									\
		if (__error == ERESTART || __error == EINTR) {		\
			__ret = -ERESTARTSYS;				\
			break;						\
		}							\
		if ((timo) > 0 && (__ret <= 0 || __error == EWOULDBLOCK)) { \
			__ret = ((condition)) ? 1 : 0;			\
			break;						\
 		}							\
	} while (__ret > 0 && !(condition));				\
	finish_wait(&wqh, &__wq_entry);					\
	__ret;								\
})

/*
 * Sleep until `condition' gets true.
 */
#define wait_event(wqh, condition) 		\
do {						\
	if (!(condition))			\
		__wait_event_intr(wqh, condition, 0); \
} while (0)

#define wait_event_killable(wqh, condition) 		\
({						\
	int __ret = 0;				\
	if (!(condition))			\
		__ret = __wait_event_intr(wqh, condition, PCATCH); \
	__ret;					\
})

#define wait_event_interruptible(wqh, condition) 		\
({						\
	int __ret = 0;				\
	if (!(condition))			\
		__ret = __wait_event_intr(wqh, condition, PCATCH); \
	__ret;					\
})

#define __wait_event_intr_locked(wqh, condition)			\
({									\
	struct wait_queue_entry __wq_entry;				\
	int __error;							\
									\
	init_wait_entry(&__wq_entry, 0);				\
	do {								\
		KASSERT(!cold);						\
									\
		if (list_empty(&__wq_entry.entry))			\
			__add_wait_queue_entry_tail(&wqh, &__wq_entry);	\
		set_current_state(TASK_INTERRUPTIBLE);			\
									\
		mtx_leave(&(wqh).lock);					\
		__error = sleep_finish(INFSLP, 1);			\
		mtx_enter(&(wqh).lock);					\
		if (__error == ERESTART || __error == EINTR) {		\
			__error = -ERESTARTSYS;				\
			break;						\
		}							\
	} while (!(condition));						\
	__remove_wait_queue(&(wqh), &__wq_entry);			\
	__set_current_state(TASK_RUNNING);				\
	__error;							\
})

#define wait_event_interruptible_locked(wqh, condition) 		\
({						\
	int __ret = 0;				\
	if (!(condition))			\
		__ret = __wait_event_intr_locked(wqh, condition); 	\
	__ret;					\
})

/*
 * Sleep until `condition' gets true or `timo' expires.
 *
 * Returns 0 if `condition' is still false when `timo' expires or
 * the remaining (>=1) jiffies otherwise.
 */
#define wait_event_timeout(wqh, condition, timo)	\
({						\
	long __ret = timo;			\
	if (!(condition))			\
		__ret = __wait_event_intr_timeout(wqh, condition, timo, 0); \
	__ret;					\
})

/*
 * Sleep until `condition' gets true, `timo' expires or the process
 * receives a signal.
 *
 * Returns -ERESTARTSYS if interrupted by a signal.
 * Returns 0 if `condition' is still false when `timo' expires or
 * the remaining (>=1) jiffies otherwise.
 */
#define wait_event_interruptible_timeout(wqh, condition, timo) \
({						\
	long __ret = timo;			\
	if (!(condition))			\
		__ret = __wait_event_intr_timeout(wqh, condition, timo, PCATCH);\
	__ret;					\
})

#define __wait_event_lock_irq(wqh, condition, mtx)			\
({									\
	struct wait_queue_entry __wq_entry;				\
									\
	init_wait_entry(&__wq_entry, 0);				\
	do {								\
		int __wait;						\
									\
		KASSERT(!cold);						\
									\
		prepare_to_wait(&wqh, &__wq_entry, 0);			\
									\
		__wait = !(condition);					\
									\
		mtx_leave(&(mtx));					\
		sleep_finish(INFSLP, __wait);				\
		mtx_enter(&(mtx));					\
	} while (!(condition));						\
	finish_wait(&wqh, &__wq_entry);					\
})

/*
 * Sleep until `condition' gets true.
 * called locked, condition checked under lock
 */
#define wait_event_lock_irq(wqh, condition, mtx) 		\
do {								\
	if (!(condition))					\
		__wait_event_lock_irq(wqh, condition, mtx); 	\
} while (0)

static inline void
wake_up(wait_queue_head_t *wqh)
{
	wait_queue_entry_t *wqe;
	wait_queue_entry_t *tmp;
	mtx_enter(&wqh->lock);
	
	list_for_each_entry_safe(wqe, tmp, &wqh->head, entry) {
		KASSERT(wqe->func != NULL);
		if (wqe->func != NULL)
			wqe->func(wqe, 0, wqe->flags, NULL);
	}
	mtx_leave(&wqh->lock);
}

#define wake_up_all(wqh)			wake_up(wqh)

static inline void
wake_up_all_locked(wait_queue_head_t *wqh)
{
	wait_queue_entry_t *wqe;
	wait_queue_entry_t *tmp;

	list_for_each_entry_safe(wqe, tmp, &wqh->head, entry) {
		KASSERT(wqe->func != NULL);
		if (wqe->func != NULL)
			wqe->func(wqe, 0, wqe->flags, NULL);
	}
}

#define wake_up_interruptible(wqh)		wake_up(wqh)
#define wake_up_interruptible_poll(wqh, flags)	wake_up(wqh)

#define	DEFINE_WAIT(name)				\
	struct wait_queue_entry name = {		\
		.private = curproc,			\
		.func = autoremove_wake_function,	\
		.entry = LIST_HEAD_INIT((name).entry),	\
	}

#define	DEFINE_WAIT_FUNC(name, fn)			\
	struct wait_queue_entry name = {		\
		.private = curproc,			\
		.func = fn,				\
		.entry = LIST_HEAD_INIT((name).entry),	\
	}
/* 9front: what the Linux code uses besides (OpenBSD changes that code) */
typedef int (*wait_queue_func_t)(struct wait_queue_entry *, unsigned, int, void *);

#define WQ_FLAG_EXCLUSIVE	0x01
#define WQ_FLAG_WOKEN		0x02

#define __WAIT_QUEUE_HEAD_INITIALIZER(n) \
	{ .lock = MUTEX_INITIALIZER(IPL_TTY), .head = LIST_HEAD_INIT((n).head) }
#define DECLARE_WAIT_QUEUE_HEAD(n) \
	wait_queue_head_t n = __WAIT_QUEUE_HEAD_INITIALIZER(n)

int	default_wake_function(wait_queue_entry_t *, unsigned, int, void *);
int	woken_wake_function(wait_queue_entry_t *, unsigned, int, void *);
long	wait_woken(wait_queue_entry_t *, unsigned, long);
void	prepare_to_wait_exclusive(wait_queue_head_t *, wait_queue_entry_t *, int);

#define DECLARE_WAITQUEUE(n, t)					\
	wait_queue_entry_t n = {				\
		.private = (t),					\
		.func = default_wake_function,			\
		.entry = LIST_HEAD_INIT((n).entry),		\
	}

static inline void
init_waitqueue_entry(wait_queue_entry_t *wqe, struct task_struct *p)
{
	wqe->flags = 0;
	wqe->private = p;
	wqe->func = default_wake_function;
}

static inline void
init_waitqueue_func_entry(wait_queue_entry_t *wqe, wait_queue_func_t fn)
{
	wqe->flags = 0;
	wqe->private = NULL;
	wqe->func = fn;
}

static inline void
add_wait_queue_exclusive(wait_queue_head_t *head, wait_queue_entry_t *new)
{
	new->flags |= WQ_FLAG_EXCLUSIVE;
	mtx_enter(&head->lock);
	list_add_tail(&new->entry, &head->head);
	mtx_leave(&head->lock);
}

static inline int
waitqueue_active(wait_queue_head_t *wqh)
{
	return !list_empty(&wqh->head);
}

static inline bool
wq_has_sleeper(wait_queue_head_t *wqh)
{
	membar_sync();
	return waitqueue_active(wqh);
}

/* wakes the entries, of exclusive ones at most nr (0: all), with key */
static inline void
__wake_up(wait_queue_head_t *wqh, unsigned int mode, int nr, void *key)
{
	wait_queue_entry_t *wqe, *tmp;

	mtx_enter(&wqh->lock);
	list_for_each_entry_safe(wqe, tmp, &wqh->head, entry) {
		if (wqe->func(wqe, mode, 0, key) &&
		    (wqe->flags & WQ_FLAG_EXCLUSIVE) && --nr == 0)
			break;
	}
	mtx_leave(&wqh->lock);
}

#define wake_up_interruptible_all(wqh)	__wake_up(wqh, TASK_INTERRUPTIBLE, 0, NULL)
#define wake_up_poll(wqh, m)		__wake_up(wqh, TASK_NORMAL, 1, (void *)(uintptr_t)(m))
#define wake_up_locked(wqh)		wake_up_all_locked(wqh)
#define wait_event_killable_timeout(wqh, c, t)	wait_event_interruptible_timeout(wqh, c, t)

#include <linux/wait_bit.h>	/* 9front: as Linux's headers bring it */

#endif
