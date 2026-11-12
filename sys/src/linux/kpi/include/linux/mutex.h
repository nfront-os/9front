/* Public domain. */

#ifndef _LINUX_MUTEX_H
#define _LINUX_MUTEX_H

#include <sys/stdint.h>
#include <sys/rwlock.h>
#include <linux/list.h>
#include <linux/spinlock_types.h>
#include <linux/lockdep.h>
#include <linux/atomic.h>
#include <linux/errno.h>

/*
 * 9front: Linux's struct mutex is a type of its own around an rwlock
 * (OpenBSD changes the Linux code to struct rwlock); the operations as
 * OpenBSD's, on its rwlock.
 */
struct mutex {
	struct rwlock rwl;
};

#define __MUTEX_INITIALIZER(x)	{ RWLOCK_INITIALIZER(#x) }
#define DEFINE_MUTEX(x)		struct mutex x = __MUTEX_INITIALIZER(x)

#define mutex_init(m)			rw_init(&((struct mutex *)(m))->rwl, #m)
#define mutex_lock_interruptible_nested(m, subc) \
					mutex_lock_interruptible(m)
#define mutex_lock(m)			rw_enter_write(&((struct mutex *)(m))->rwl)
#define mutex_lock_nest_lock(m, sub)	rw_enter_write(&((struct mutex *)(m))->rwl)
#define mutex_lock_nested(m, sub)	rw_enter_write(&((struct mutex *)(m))->rwl)
#define mutex_trylock(m)		(rw_enter(&((struct mutex *)(m))->rwl, RW_WRITE | RW_NOSLEEP) == 0)
#define mutex_unlock(m)			rw_exit_write(&((struct mutex *)(m))->rwl)
#define mutex_is_locked(m)		(rw_status(&((struct mutex *)(m))->rwl) != 0)
#define mutex_destroy(m)		((void)(m))
#define mutex_lock_killable(m)		mutex_lock_interruptible(m)
#define mutex_lock_killable_nested(m, s)	mutex_lock_interruptible(m)
#define mutex_lock_io(m)		mutex_lock(m)
#define devm_mutex_init(d, m)		(mutex_init(m), 0)

static inline int
mutex_lock_interruptible(struct mutex *m)
{
	if (rw_enter(&m->rwl, RW_WRITE | RW_INTR) != 0)
		return -EINTR;
	return 0;
}

enum mutex_trylock_recursive_result {
	MUTEX_TRYLOCK_FAILED,
	MUTEX_TRYLOCK_SUCCESS,
	MUTEX_TRYLOCK_RECURSIVE
};

static inline enum mutex_trylock_recursive_result
mutex_trylock_recursive(struct mutex *m)
{
	if (rw_status(&m->rwl) == RW_WRITE)
		return MUTEX_TRYLOCK_RECURSIVE;
	if (mutex_trylock(m))
		return MUTEX_TRYLOCK_SUCCESS;
	return MUTEX_TRYLOCK_FAILED;
}

int atomic_dec_and_mutex_lock(atomic_t *, struct mutex *);

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/types.h>
void	kpi_mutex_init(struct mutex *m);
#define __mutex_init(m, n, k)		rw_init(&((struct mutex *)(m))->rwl, (n))

/* 9front: scope guards (linux/cleanup.h) */
#include <linux/cleanup.h>
DEFINE_GUARD(mutex, struct mutex *, mutex_lock(_T), mutex_unlock(_T))

#endif
