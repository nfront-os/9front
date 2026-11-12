/* Public domain. */
/*
 * 9front: counting semaphores (OpenBSD's header is empty, OpenBSD
 * changes the Linux code that needs them: drm/task_barrier.h), on
 * mutex(9) and msleep(9).  No signals here: the interruptible and
 * killable downs always get the semaphore.
 */

#ifndef _LINUX_SEMAPHORE_H
#define _LINUX_SEMAPHORE_H

#include <sys/types.h>
#include <sys/mutex.h>
#include <sys/proc.h>
#include <linux/errno.h>
#include <linux/jiffies.h>

struct semaphore {
	struct spinlock	lock;
	unsigned int	count;
};

#define __SEMAPHORE_INITIALIZER(name, n) \
	{ .lock = MUTEX_INITIALIZER(IPL_NONE), .count = (n) }
#define DEFINE_SEMAPHORE(name, n) \
	struct semaphore name = __SEMAPHORE_INITIALIZER(name, n)

static inline void
sema_init(struct semaphore *s, int n)
{
	mtx_init(&s->lock, IPL_NONE);
	s->count = n;
}

/* wait at most nsecs for the count to be > 0, then take one */
static inline int
__down_nsec(struct semaphore *s, uint64_t nsecs)
{
	int r = 0;

	mtx_enter(&s->lock);
	while (s->count == 0) {
		if (msleep_nsec(s, &s->lock, 0, "sema", nsecs) == EWOULDBLOCK &&
		    s->count == 0) {
			r = -ETIME;
			break;
		}
	}
	if (r == 0)
		s->count--;
	mtx_leave(&s->lock);
	return r;
}

static inline void
down(struct semaphore *s)
{
	__down_nsec(s, INFSLP);
}

#define down_interruptible(s)	(down(s), 0)
#define down_killable(s)	(down(s), 0)

static inline int
down_timeout(struct semaphore *s, long jiffies)
{
	return __down_nsec(s, jiffies_to_nsecs(jiffies));
}

/* 0 when it took one, 1 when the count was 0 */
static inline int
down_trylock(struct semaphore *s)
{
	int r = 1;

	mtx_enter(&s->lock);
	if (s->count > 0) {
		s->count--;
		r = 0;
	}
	mtx_leave(&s->lock);
	return r;
}

static inline void
up(struct semaphore *s)
{
	mtx_enter(&s->lock);
	s->count++;
	wakeup_one(s);
	mtx_leave(&s->lock);
}

#endif
