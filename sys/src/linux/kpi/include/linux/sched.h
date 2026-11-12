/*	$OpenBSD: sched.h,v 1.6 2026/08/06 10:25:10 jsg Exp $	*/
/*
 * Copyright (c) 2013, 2014, 2015 Mark Kettenis
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _LINUX_SCHED_H
#define _LINUX_SCHED_H

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/stdint.h>
#include <sys/mutex.h>
#include <linux/mutex.h>	/* 9front: as Linux's sched.h */
#include <sys/signalvar.h>	/* 9front: SIGKILL, as Linux's sched.h brings it */
#include <linux/hrtimer.h>
#include <linux/sem.h>
#include <asm/current.h>	/* 9front: current, as Linux's sched.h has it */
#include <linux/uidgid.h>
/* 9front: from OpenBSD 6.18 */
#include <linux/preempt.h>

#define TASK_NORMAL		1
#define TASK_UNINTERRUPTIBLE	0
#define TASK_INTERRUPTIBLE	PCATCH
#define TASK_RUNNING		-1

#define MAX_SCHEDULE_TIMEOUT	(INT32_MAX)

#define TASK_COMM_LEN		(MAXCOMLEN + 1)

#define cond_resched()		sched_pause(yield)
#define drm_need_resched() \
    (curcpu()->ci_schedstate.spc_schedflags & SPCF_SHOULDYIELD)

static inline int
cond_resched_lock(struct spinlock *mtxp)
{
	if (drm_need_resched() == 0)
		return 0;

	mtx_leave(mtxp);
	cond_resched();
	mtx_enter(mtxp);

	return 1;
}

void set_current_state(int);
void __set_current_state(int);
void schedule(void);
long schedule_timeout(long);
long schedule_timeout_uninterruptible(long);

#define io_schedule_timeout(x)	schedule_timeout(x)

struct task_struct;
int wake_up_process(struct task_struct *p);

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/ktime.h>
#include <linux/capability.h>
#include <linux/seqlock.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/kthread.h>
#define TASK_KILLABLE		TASK_UNINTERRUPTIBLE
#define TASK_NOLOAD		0x0400
#define TASK_IDLE		(TASK_UNINTERRUPTIBLE | TASK_NOLOAD)
#define PF_KTHREAD		0x00200000
#define PF_MEMALLOC		0x00000800
#define PF_EXITING		0x00000004
struct task_struct	*kpi_current(void);
long	schedule_timeout_interruptible(long timeout);
long	schedule_timeout_killable(long timeout);
void	kpi_yield(void);
#define yield_to(p, preempt)	({ (void)(p); (void)(preempt); kpi_yield(); 1; })
void	kpi_set_current_state(int state);
#define fatal_signal_pending(t)	0
#define task_pid_nr(t)		((t)->pid)
#define task_tgid_nr(t)		((t)->pid)
#define get_task_comm(buf, t)	strcpy(buf, (t)->comm)
#define need_resched()		0
#define sched_set_fifo(t)	do { } while (0)
#define sched_set_fifo_low(t)	do { } while (0)
#define sched_set_normal(t, n)	do { } while (0)
#define io_schedule()		schedule()
#define get_task_struct(t)	(t)
#define put_task_struct(t)	do { } while (0)
struct pid;
static inline pid_t pid_nr(struct pid *p) { return (pid_t)(uintptr_t)p; }
#define task_pid(t)		((struct pid *)(uintptr_t)(t)->pid)
#define get_pid(p)		(p)
#define pid_task(p, t)		((struct task_struct *)NULL)
#define get_pid_task(p, t)	((struct task_struct *)NULL)
#define task_tgid(t)			task_pid(t)
#define task_pid_vnr(t)		((t)->pid)
#define get_task_pid(t, x)	task_pid(t)
#define find_vpid(n)		((struct pid *)(uintptr_t)(n))
#define rcu_access_pointer_pid(p)	(p)
enum pid_type { PIDTYPE_PID, PIDTYPE_TGID, PIDTYPE_PGID, PIDTYPE_SID, PIDTYPE_MAX };

#define signal_pending(t)	0
#define fatal_signal_pending(t)	0
#define signal_pending_state(s, t)	0
#define put_pid(p)		do { } while (0)

#include <linux/pid.h>	/* 9front: as Linux's sched.h brings it */

#endif
