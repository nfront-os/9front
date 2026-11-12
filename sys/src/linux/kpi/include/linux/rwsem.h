/* Public domain. */

#ifndef _LINUX_RWSEM_H
#define _LINUX_RWSEM_H

#include <sys/rwlock.h>

/* 9front: Linux's struct rw_semaphore around an rwlock, as struct mutex (linux/mutex.h) */
struct rw_semaphore {
	struct rwlock rwl;
};

#define init_rwsem(s)			rw_init(&(s)->rwl, #s)
#define down_read(s)			rw_enter_read(&(s)->rwl)
#define down_read_trylock(s)		(rw_enter(&(s)->rwl, RW_READ | RW_NOSLEEP) == 0)
#define down_write_trylock(s)		(rw_enter(&(s)->rwl, RW_WRITE | RW_NOSLEEP) == 0)
#define up_read(s)			rw_exit_read(&(s)->rwl)
#define down_write(s)			rw_enter_write(&(s)->rwl)
#define down_write_nest_lock(s, x)	rw_enter_write(&(s)->rwl)
#define down_read_killable(s)		(rw_enter_read(&(s)->rwl), 0)
#define down_write_killable(s)		(rw_enter_write(&(s)->rwl), 0)
#define up_write(s)			rw_exit_write(&(s)->rwl)
#define downgrade_write(s)		rw_enter(&(s)->rwl, RW_DOWNGRADE)
#define rwsem_is_locked(s)		(rw_status(&(s)->rwl) != 0)

#define DECLARE_RWSEM(s) \
	struct rw_semaphore s = { RWLOCK_INITIALIZER(#s) }
/* no interface to check if another caller wants the lock */
#define rwsem_is_contended(s)		0

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/types.h>
#include <linux/lockdep.h>
#define __RWSEM_INITIALIZER(n)		{ { 0 } }
void	kpi_init_rwsem(struct rw_semaphore *s);
#define down_read_interruptible(s)	({ down_read(s); 0; })
#define down_read_nested(s, n)		down_read(s)
#define down_write_nested(s, n)		down_write(s)

#endif
