/* Public domain. */

#ifndef LINUX_RCUPDATE_H
#define LINUX_RCUPDATE_H

#include <linux/cpumask.h>

/*
 * 9front: RCU for real (kpi/linux/rcu.c): the procs here run in
 * parallel with the readers (the irq proc, the workers), so a callback
 * must wait for a grace period; OpenBSD's run at once
 */
struct rcu_head {
	struct rcu_head *next;
	void (*func)(struct rcu_head *);
};
typedef void (*rcu_callback_t)(struct rcu_head *head);

#define __rcu
#define rcu_dereference(p)	READ_ONCE(p)
#define rcu_dereference_raw(p)	READ_ONCE(p)
#define rcu_dereference_protected(p, c)	(p)
#define rcu_dereference_check(p, c)	READ_ONCE(p)
#define rcu_access_pointer(p)	READ_ONCE(p)
#define RCU_INIT_POINTER(p, v)		do { (p) = (v); } while(0)
#define rcu_assign_pointer(p, v)	do { membar_producer(); WRITE_ONCE((p), (v)); } while(0)
void	kpi_rcu_read_lock(void);
void	kpi_rcu_read_unlock(void);
#define rcu_read_lock()		kpi_rcu_read_lock()
#define rcu_read_unlock()	kpi_rcu_read_unlock()
#define rcu_pointer_handoff(p)	(p)
#define init_rcu_head(h)
#define destroy_rcu_head(h)
#define rcu_replace_pointer(rp, p, c)		\
({						\
	__typeof(rp) __r = rp;			\
	rcu_assign_pointer(rp, p);		\
	__r;					\
})
void	call_rcu(struct rcu_head *, rcu_callback_t);
void	synchronize_rcu(void);
void	rcu_barrier(void);
void	kpi_kvfree_call_rcu(struct rcu_head *, void *);
/* the head's offset in the object for the func: rcu.c frees the object */
#define kfree_rcu(objp, name)	\
	kpi_kvfree_call_rcu(&(objp)->name, (void *)(objp))
#define kvfree_rcu(objp, name)	kfree_rcu(objp, name)
#define synchronize_rcu_expedited()	synchronize_rcu()
#define cond_synchronize_rcu(x)		synchronize_rcu()
#define get_state_synchronize_rcu()	0

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/atomic.h>
#include <linux/rculist.h>
#define rcu_read_lock_held()		1
#define rcu_read_lock_bh()		rcu_read_lock()
#define rcu_read_unlock_bh()		rcu_read_unlock()
#define rcu_read_lock_sched()		rcu_read_lock()
#define rcu_read_unlock_sched()		rcu_read_unlock()
#define rcu_dereference_bh(p)		READ_ONCE(p)
#define RCU_INITIALIZER(v)		(v)
#define unrcu_pointer(p)		(p)
#define synchronize_rcu_tasks()		synchronize_rcu()
#define rcu_head_init(h)		do { } while (0)
#define init_rcu_head_on_stack(h)	do { } while (0)
#define destroy_rcu_head_on_stack(h)	do { } while (0)
#define rcu_is_watching()		1
#define __rcu_read_lock()		barrier()
#define __rcu_read_unlock()		barrier()
#define guard_rcu()

/* 9front: guard(rcu)() (linux/cleanup.h) */
#include <linux/cleanup.h>
DEFINE_LOCK_GUARD_0(rcu, rcu_read_lock(), rcu_read_unlock())

#endif
