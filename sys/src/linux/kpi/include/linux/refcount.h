/* Public domain. */

#ifndef _LINUX_REFCOUNT_H
#define _LINUX_REFCOUNT_H

#include <sys/types.h>
#include <linux/atomic.h>

/* 9front: Linux's refcount_t, a struct around an atomic_t (the Linux code is unchanged) */
typedef struct refcount_struct {
	atomic_t refs;
} refcount_t;

#define REFCOUNT_INIT(n)	{ .refs = ATOMIC_INIT(n) }

static inline bool
refcount_dec_and_test(refcount_t *p)
{
	return atomic_dec_and_test(&p->refs);
}

static inline bool
refcount_inc_not_zero(refcount_t *p)
{
	return atomic_inc_not_zero(&p->refs);
}

static inline void
refcount_set(refcount_t *p, int v)
{
	atomic_set(&p->refs, v);
}

static inline void
refcount_inc(refcount_t *p)
{
	atomic_inc(&p->refs);
}

static inline void
refcount_add(int n, refcount_t *p)
{
	atomic_add(n, &p->refs);
}

static inline void
refcount_dec(refcount_t *p)
{
	atomic_dec(&p->refs);
}

static inline bool
refcount_dec_not_one(refcount_t *p)
{
	return atomic_add_unless(&p->refs, -1, 1);
}

static inline bool
refcount_dec_and_lock_irqsave(refcount_t *p, struct spinlock *lock,
    unsigned long *flags)
{
	if (atomic_add_unless(&p->refs, -1, 1))
		return false;
	mtx_enter(lock);
	if (atomic_dec_return(&p->refs) == 0)
		return true;
	mtx_leave(lock);
	return false;
}

static inline uint32_t
refcount_read(const refcount_t *p)
{
	return atomic_read(&p->refs);
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
static inline bool refcount_add_not_zero(int i, refcount_t *r) { return atomic_add_unless(&r->refs, i, 0); }
static inline bool refcount_sub_and_test(int i, refcount_t *r) { return atomic_sub_and_test(i, &r->refs); }
int	kpi_atomic_dec_and_lock(atomic_t *a, struct spinlock *l);
#define refcount_dec_and_lock(r, l)	kpi_atomic_dec_and_lock(&(r)->refs, l)

#endif
