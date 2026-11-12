/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD's mutex(9) on Plan 9's Lock, rwlock(9)
 * on Plan 9's RWLock.  The lock's storage is in the struct (sized
 * there); the owner is kept for mtx_owned and rw_status.  A downgrade
 * lets go of the write lock and takes a read lock: not atomic, a writer
 * may come in between.
 */
#include "obsd.h"

void
mtx_init(struct spinlock *m, int ipl)
{
	memset(m->mtx_lock, 0, sizeof(m->mtx_lock));
	m->mtx_owner = NULL;
	m->mtx_wantipl = ipl;
}

void
mtx_enter(struct spinlock *m)
{
	p9_lock(m->mtx_lock);
	m->mtx_owner = curproc;
}

int
mtx_enter_try(struct spinlock *m)
{
	if (!p9_canlock(m->mtx_lock))
		return 0;
	m->mtx_owner = curproc;
	return 1;
}

void
mtx_leave(struct spinlock *m)
{
	m->mtx_owner = NULL;
	p9_unlock(m->mtx_lock);
}

int
mtx_owned(struct spinlock *m)
{
	return m->mtx_owner == curproc;
}

void
rw_init(struct rwlock *rwl, const char *name)
{
	memset(rwl->rwl_lock, 0, sizeof(rwl->rwl_lock));
	rwl->rwl_owner = NULL;
	rwl->rwl_name = name;
}

void
rw_enter_read(struct rwlock *rwl)
{
	p9_rlock(rwl->rwl_lock);
}

void
rw_exit_read(struct rwlock *rwl)
{
	p9_runlock(rwl->rwl_lock);
}

void
rw_enter_write(struct rwlock *rwl)
{
	p9_wlock(rwl->rwl_lock);
	rwl->rwl_owner = curproc;
}

void
rw_exit_write(struct rwlock *rwl)
{
	rwl->rwl_owner = NULL;
	p9_wunlock(rwl->rwl_lock);
}

void
rw_exit(struct rwlock *rwl)
{
	if (rwl->rwl_owner == curproc)
		rw_exit_write(rwl);
	else
		rw_exit_read(rwl);
}

int
rw_enter(struct rwlock *rwl, int flags)
{
	switch (flags & RW_OPMASK) {
	case RW_WRITE:
		if (flags & RW_NOSLEEP) {
			if (!p9_canwlock(rwl->rwl_lock))
				return EBUSY;
			rwl->rwl_owner = curproc;
			return 0;
		}
		rw_enter_write(rwl);
		return 0;
	case RW_READ:
		if (flags & RW_NOSLEEP) {
			if (!p9_canrlock(rwl->rwl_lock))
				return EBUSY;
			return 0;
		}
		rw_enter_read(rwl);
		return 0;
	case RW_DOWNGRADE:
		rw_exit_write(rwl);
		rw_enter_read(rwl);
		return 0;
	}
	panic("rw_enter: flags %#x", flags);
}

int
rw_status(struct rwlock *rwl)
{
	void *owner = READ_ONCE(rwl->rwl_owner);

	if (owner == curproc)
		return RW_WRITE;
	if (owner != NULL)
		return RW_WRITE_OTHER;
	if (p9_rwlocked(rwl->rwl_lock))
		return RW_READ;
	return 0;
}
