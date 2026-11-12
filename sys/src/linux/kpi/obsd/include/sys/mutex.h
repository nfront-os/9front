/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, mutex(9): the spinning
 * mutex, on Plan 9's Lock (kpi/obsd/lock.c).  Named struct spinlock, not
 * OpenBSD's struct mutex: the Linux code is unchanged here and has its
 * own struct mutex (a sleeping lock, linux/mutex.h).  The IPL is kept
 * but means nothing: a file server has no interrupts, its IRQ proc is
 * a proc like the others.
 */
#ifndef _OBSD_SYS_MUTEX_H
#define _OBSD_SYS_MUTEX_H

#include <sys/types.h>
#include <machine/intr.h>

struct spinlock {
	uint64_t	mtx_lock[2];	/* a Plan 9 Lock */
	void		*mtx_owner;	/* the struct proc holding it */
	int		mtx_wantipl;
};

#define MUTEX_INITIALIZER(ipl)	{ { 0, 0 }, NULL, (ipl) }
#define MUTEX_INITIALIZER_FLAGS(ipl, name, flags)	MUTEX_INITIALIZER(ipl)

void	mtx_init(struct spinlock *, int);
#define mtx_init_flags(m, ipl, name, flags)	mtx_init(m, ipl)
void	mtx_enter(struct spinlock *);
int	mtx_enter_try(struct spinlock *);
void	mtx_leave(struct spinlock *);
int	mtx_owned(struct spinlock *);

#define MUTEX_ASSERT_LOCKED(m)		((void)0)
#define MUTEX_ASSERT_UNLOCKED(m)	((void)0)
#define mtx_owner(m)			((m)->mtx_owner)

#endif
