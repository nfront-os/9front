/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, task_add(9): task queues,
 * each with worker procs (kpi/obsd/task.c).  systq and systqmp are the
 * system's queues.
 */
#ifndef _OBSD_SYS_TASK_H
#define _OBSD_SYS_TASK_H

#include <sys/types.h>
#include <sys/queue.h>

struct taskq;

struct task {
	TAILQ_ENTRY(task) t_entry;
	void		(*t_func)(void *);
	void		*t_arg;
	unsigned int	t_flags;
	uint64_t	t_seq;		/* its number on the queue (kpi/obsd/task.c) */
};

#define TASK_ONQUEUE	1
#define TASK_BARRIER	2

TAILQ_HEAD(task_list, task);

#define TASKQ_MPSAFE		(1 << 0)
#define TASK_INITIALIZER(f, a)	{ { NULL, NULL }, (f), (a), 0, 0 }

extern struct taskq *const systq;
extern struct taskq *const systqmp;

struct taskq	*taskq_create(const char *, unsigned int, int, unsigned int);
void	taskq_destroy(struct taskq *);
void	taskq_barrier(struct taskq *);
void	taskq_del_barrier(struct taskq *, struct task *);

void	task_set(struct task *, void (*)(void *), void *);
int	task_add(struct taskq *, struct task *);
int	task_del(struct taskq *, struct task *);

#define task_pending(t)	((t)->t_flags & TASK_ONQUEUE)

#endif
