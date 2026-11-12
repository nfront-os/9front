/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, sys/proc.h: the procs that
 * run the Linux code, and tsleep(9)/wakeup(9) (kpi/obsd/sleep.c).
 * OpenBSD's struct proc and Linux's struct task_struct are one struct
 * here (the Linux code sees current, OpenBSD's code curproc), one per
 * Plan 9 proc, made when the proc first asks for it.  Sleeping procs
 * hang off a hash of their wait channels; wakeup(chan) wakes all on it.
 */
#ifndef _OBSD_SYS_PROC_H
#define _OBSD_SYS_PROC_H

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/param.h>

struct spinlock;
struct rwlock;
struct mm_struct;
struct cpu_info;

struct process {
	pid_t	ps_pid;
	char	ps_comm[MAXCOMLEN + 1];
};

struct task_struct {
	/* OpenBSD's struct proc */
	struct process	*p_p;
	struct process	p_process;
	const volatile void	*p_wchan;	/* sleeping on it */
	const char	*p_wmesg;
	int		p_stat;
	int		p_flag;
	TAILQ_ENTRY(task_struct) p_runq;	/* on its channel's sleep queue */
	uint64_t	p_rendez[12];		/* a Plan 9 Rendez and its QLock */
	uint64_t	p_qlock[8];
	int		p_woken;
	int		p_timedout;
	void		*p_taskq;	/* the task queue it works for */
	struct cpu_info	*p_cpu;		/* its curcpu() */
	int		p_tqworker;	/* which of its workers */

	/* Linux's struct task_struct: what the Linux code uses of it */
	pid_t		pid;
	pid_t		tgid;		/* 9front: the program's (Linux): the group leader's pid */
	char		comm[MAXCOMLEN + 1];
	struct task_struct	*group_leader;	/* the first proc of the program: all share its memory */
	struct mm_struct	*mm;
	unsigned int	flags;
	int		exit_code;	/* never killed here: 0 */
	unsigned long	uwin, uwinlen;	/* the "user" memory a client's ioctl may reach (kpi_user_window), 0: all */
	int		unpriv;		/* runs a client's ioctl that is not the hostowner's (kpi_user_privileged): suser fails */
	int		__state;	/* set_current_state */
	void		*kthread;	/* kthread_run's, for kthread_should_stop */
	int		prio;
	int		rcu_nest;	/* rcu_read_lock depth (kpi/linux/rcu.c) */
	int		rcu_epoch;	/* the epoch it counts in */
};

/* p_stat */
#define SIDL	1
#define SRUN	2
#define SSLEEP	3
#define SSTOP	4
#define SDEAD	6
#define SONPROC	7
/* p_flag */
#define P_WSLEEP	0x00000020
#define P_SYSTEM	0x00000200
#define P_SINTR		0x00000080

#define INFSLP	UINT64_MAX	/* sleep without a time limit */

struct task_struct	*kpi_curproc(void);
#define curproc	(kpi_curproc())

/* 0, EWOULDBLOCK (timed out); PCATCH never gives EINTR: no signals */
int	tsleep(const volatile void *, int, const char *, int);
int	tsleep_nsec(const volatile void *, int, const char *, uint64_t);
/* OpenBSD's msleep, renamed: msleep is Linux's (linux/delay.h) */
int	msleep_ticks(const volatile void *, struct spinlock *, int, const char *, int);
int	msleep_nsec(const volatile void *, struct spinlock *, int, const char *, uint64_t);
int	rwsleep(const volatile void *, struct rwlock *, int, const char *, int);
int	rwsleep_nsec(const volatile void *, struct rwlock *, int, const char *, uint64_t);
void	wakeup(const volatile void *);
void	wakeup_n(const volatile void *, int);
#define wakeup_one(c)	wakeup_n((c), 1)
int	wakeup_proc(struct task_struct *, int);	/* 1 if it slept */

/* the two halves of a sleep: in between, the caller may check its condition */
void	sleep_setup(const volatile void *, int, const char *);
int	sleep_finish(uint64_t, int);	/* time limit in ns (INFSLP: none), do_sleep; 0 or EWOULDBLOCK */
void	unsleep(struct task_struct *);	/* off the sleep queue, running */

/* yield(9), Linux's yield too; named so: libthread has a yield of its own */
void	kpi_yield(void);
#define yield	kpi_yield
void	sched_pause(void (*)(void));
/* 9front: no preempt() macro: Linux code names things preempt (amdgpu's user queues) */

int	suser(struct task_struct *);

/* kthread(9): a proc of the KPI's (kpi_newproc) */
int	kthread_create(void (*)(void *), void *, struct task_struct **, const char *, ...);
__attribute__((__noreturn__)) void	kthread_exit(int);

#endif
