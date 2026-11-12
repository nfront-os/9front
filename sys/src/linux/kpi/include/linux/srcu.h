/* Public domain. */

#ifndef _LINUX_SRCU_H
#define _LINUX_SRCU_H

/* 9front: sleepable RCU on RCU (the Linux code is unchanged and declares them) */
struct srcu_struct {
	int unused;
};
#define DEFINE_STATIC_SRCU(n)		static struct srcu_struct n
#define DEFINE_SRCU(n)			struct srcu_struct n
#define synchronize_srcu(x)		synchronize_rcu()

#define init_srcu_struct(x)
#define cleanup_srcu_struct(x)

#define srcu_read_lock(x)			0
#define srcu_read_unlock(x, y)

#define synchronize_srcu_expedited(x)

#endif
