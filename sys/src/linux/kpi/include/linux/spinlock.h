/* Public domain. */

#ifndef _LINUX_SPINLOCK_H
#define _LINUX_SPINLOCK_H

#include <linux/spinlock_types.h>
#include <linux/preempt.h>
#include <linux/bottom_half.h>
#include <linux/atomic.h>
#include <linux/lockdep.h>

#define spin_lock_irqsave(_mtxp, _flags) do {			\
		_flags = 0;					\
		mtx_enter(_mtxp);				\
	} while (0)

#define spin_lock_irqsave_nested(_mtxp, _flags, _subclass) do {	\
		(void)(_subclass);				\
		_flags = 0;					\
		mtx_enter(_mtxp);				\
	} while (0)

#define spin_unlock_irqrestore(_mtxp, _flags) do {		\
		(void)(_flags);					\
		mtx_leave(_mtxp);				\
	} while (0)

#define spin_trylock(_mtxp)					\
({								\
	mtx_enter_try(_mtxp) ? 1 : 0;				\
})

#define spin_trylock_irqsave(_mtxp, _flags)			\
({								\
	(void)(_flags);						\
	mtx_enter_try(_mtxp) ? 1 : 0;				\
})

/* 9front: on an atomic_t, without the race of testing before the decrement */
static inline int
atomic_dec_and_lock(atomic_t *v, struct spinlock *mtxp)
{
	if (atomic_add_unless(v, -1, 1))
		return 0;
	mtx_enter(mtxp);
	if (atomic_dec_and_test(v))
		return 1;
	mtx_leave(mtxp);
	return 0;
}

#define atomic_dec_and_lock_irqsave(_a, _mtxp, _flags)		\
	atomic_dec_and_lock(_a, _mtxp)

#define spin_lock(mtxp)			mtx_enter(mtxp)
#define spin_lock_nested(mtxp, l)	mtx_enter(mtxp)
#define spin_unlock(mtxp)		mtx_leave(mtxp)
#define spin_lock_irq(mtxp)		mtx_enter(mtxp)
#define spin_unlock_irq(mtxp)		mtx_leave(mtxp)
#define assert_spin_locked(mtxp)	MUTEX_ASSERT_LOCKED(mtxp)
#define spin_trylock_irq(mtxp)		mtx_enter_try(mtxp)

#define read_lock(mtxp)			mtx_enter(mtxp)
#define read_unlock(mtxp)		mtx_leave(mtxp)
#define write_lock(mtxp)		mtx_enter(mtxp)
#define write_unlock(mtxp)		mtx_leave(mtxp)

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/irqflags.h>
void	kpi_spin_lock(spinlock_t *l);
void	kpi_spin_unlock(spinlock_t *l);
int	kpi_spin_trylock(spinlock_t *l);
int	kpi_spin_is_locked(spinlock_t *l);
void	kpi_read_lock(rwlock_t *l);
void	kpi_read_unlock(rwlock_t *l);
void	kpi_write_lock(rwlock_t *l);
void	kpi_write_unlock(rwlock_t *l);
#define raw_spin_lock_init(l)		spin_lock_init(l)
#define spin_is_locked(l)		kpi_spin_is_locked(l)
#define spin_lock_bh(l)			spin_lock(l)
#define spin_unlock_bh(l)		spin_unlock(l)
#define raw_spin_lock(l)		spin_lock(l)
#define raw_spin_unlock(l)		spin_unlock(l)
#define raw_spin_lock_irq(l)		spin_lock(l)
#define raw_spin_unlock_irq(l)		spin_unlock(l)
#define raw_spin_lock_irqsave(l, f)	spin_lock_irqsave(l, f)
#define raw_spin_unlock_irqrestore(l, f)	spin_unlock_irqrestore(l, f)
#define read_lock_irq(l)		read_lock(l)
#define read_unlock_irq(l)		read_unlock(l)
#define write_lock_irq(l)		write_lock(l)
#define write_unlock_irq(l)		write_unlock(l)
#define read_lock_irqsave(l, f)		do { (f) = 0; read_lock(l); } while (0)
#define read_unlock_irqrestore(l, f)	do { (void)(f); read_unlock(l); } while (0)
#define write_lock_irqsave(l, f)	do { (f) = 0; write_lock(l); } while (0)
#define write_unlock_irqrestore(l, f)	do { (void)(f); write_unlock(l); } while (0)
#define arch_spin_lock(l)		spin_lock(l)
#define arch_spin_unlock(l)		spin_unlock(l)

/* 9front: scope guards (linux/cleanup.h) */
#include <linux/cleanup.h>
DEFINE_LOCK_GUARD_1(spinlock, spinlock_t, spin_lock(_T->lock), spin_unlock(_T->lock))
DEFINE_LOCK_GUARD_1(spinlock_irq, spinlock_t, spin_lock_irq(_T->lock), spin_unlock_irq(_T->lock))
DEFINE_LOCK_GUARD_1(spinlock_irqsave, spinlock_t, spin_lock_irqsave(_T->lock, _T->flags),
	spin_unlock_irqrestore(_T->lock, _T->flags), unsigned long flags)


#endif
