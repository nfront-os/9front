/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, rwlock(9): the sleeping
 * reader/writer lock, on Plan 9's RWLock (kpi/obsd/lock.c).  The owner
 * is recorded for writers (rw_status, mutex_is_locked).
 */
#ifndef _OBSD_SYS_RWLOCK_H
#define _OBSD_SYS_RWLOCK_H

#include <sys/types.h>

struct rwlock {
	uint64_t	rwl_lock[8];	/* a Plan 9 RWLock */
	void		*rwl_owner;	/* the struct proc writing */
	const char	*rwl_name;
};

#define RWLOCK_INITIALIZER(name)	{ { 0 }, NULL, (name) }

#define RW_WRITE	0x0001UL
#define RW_READ		0x0002UL
#define RW_DOWNGRADE	0x0004UL
#define RW_OPMASK	0x0007UL
#define RW_INTR		0x0010UL
#define RW_SLEEPFAIL	0x0020UL
#define RW_NOSLEEP	0x0040UL
#define RW_RECURSEFAIL	0x0080UL
#define RW_WRITE_OTHER	0x0100UL

void	rw_init(struct rwlock *, const char *);
#define rw_init_flags(rwl, name, flags)	rw_init(rwl, name)
void	rw_enter_read(struct rwlock *);
void	rw_enter_write(struct rwlock *);
void	rw_exit_read(struct rwlock *);
void	rw_exit_write(struct rwlock *);
void	rw_exit(struct rwlock *);
int	rw_enter(struct rwlock *, int);	/* 0, or EBUSY with RW_NOSLEEP */
int	rw_status(struct rwlock *);	/* RW_WRITE (mine), RW_WRITE_OTHER, RW_READ or 0 */
#define rw_assert_wrlock(rwl)	((void)0)
#define rw_assert_rdlock(rwl)	((void)0)
#define rw_assert_anylock(rwl)	((void)0)
#define rw_assert_unlocked(rwl)	((void)0)

#endif
