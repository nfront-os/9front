/* Public domain. */

#ifndef _LINUX_WAIT_BIT_H
#define _LINUX_WAIT_BIT_H

struct wait_queue_head;
#include <linux/wait.h>

int wait_on_bit(unsigned long *, int, unsigned);
int wait_on_bit_timeout(unsigned long *, int, unsigned, int);
void wake_up_bit(void *, int);
void clear_and_wake_up_bit(int, void *);

struct wait_queue_head *bit_waitqueue(void *, int);
struct wait_queue_head *__var_waitqueue(void *);

extern struct wait_queue_head var_waitq;	/* 9front: struct names, wait.h may be on its way here */

/* 9front: a macro, so wake_up need not be declared yet (wait.h includes us) */
#define wake_up_var(var)	wake_up(&var_waitq)	/* the queue wait_var_event waits on, as OpenBSD's */

#define wait_var_event(var, condition)				\
	wait_event(var_waitq, (condition))

#define wait_var_event_interruptible(var, condition)		\
	wait_event_interruptible(var_waitq, condition)

#define wait_var_event_timeout(var, condition, timo)		\
	wait_event_timeout(var_waitq, (condition), (timo))

#define wait_var_event_killable(var, condition)			\
	wait_event_killable(var_waitq, (condition))

#define ___wait_var_event(var, condition, state, ex, ret, fn)	\
({								\
	long __ret = ret ;					\
	if (state & TASK_INTERRUPTIBLE)				\
		__ret = wait_var_event_interruptible((var), (condition)); \
	else							\
		wait_var_event((var), (condition));		\
	__ret;							\
})


#endif
