/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD's task_add(9): task queues with worker
 * procs, which OpenBSD's linux/workqueue.h builds Linux's workqueues
 * on.  Each task added gets a number (t_seq); taskq_barrier waits until
 * every task added before it has run (none queued or running with a
 * number up to the queue's last), as OpenBSD's waits for all its threads.
 */
#include "obsd.h"
#include <sys/task.h>

#define MAXWORKERS	8

struct taskq {
	const char	*tq_name;
	unsigned int	tq_nthreads;
	unsigned int	tq_started;
	struct spinlock	tq_mtx;
	struct task_list	tq_worklist;
	uint64_t	tq_seq;		/* the last task's number */
	uint64_t	tq_running[MAXWORKERS];	/* each worker's task's number, 0: idle */
	struct task	*tq_runtask[MAXWORKERS];
	unsigned int	tq_waiters;
};

struct tqworker {
	struct taskq	*tq;
	int		n;
};

static struct taskq taskq_sys = {
	"systq", 1, 0, MUTEX_INITIALIZER(IPL_HIGH),
	TAILQ_HEAD_INITIALIZER(taskq_sys.tq_worklist),
};
static struct taskq taskq_sys_mp = {
	"systqmp", 4, 0, MUTEX_INITIALIZER(IPL_HIGH),
	TAILQ_HEAD_INITIALIZER(taskq_sys_mp.tq_worklist),
};
struct taskq *const systq = &taskq_sys;
struct taskq *const systqmp = &taskq_sys_mp;

static void taskworker(void *);

/* the workers, made once, under no lock (proccreate switches threads) */
static void
taskq_start(struct taskq *tq)
{
	struct tqworker *w;
	char name[32];
	unsigned int i;

	if (READ_ONCE(tq->tq_started))
		return;
	mtx_enter(&tq->tq_mtx);
	if (tq->tq_started) {
		mtx_leave(&tq->tq_mtx);
		return;
	}
	tq->tq_started = 1;
	mtx_leave(&tq->tq_mtx);
	for (i = 0; i < tq->tq_nthreads; i++) {
		w = p9_mallocz(sizeof(*w));
		if (w == NULL)
			p9_panic("taskq: no memory");
		w->tq = tq;
		w->n = i;
		snprintf(name, sizeof(name), "%s%d", tq->tq_name, i);
		if (kpi_newproc(name, taskworker, w) == NULL)
			p9_panic("taskq: no proc");
	}
}

struct taskq *
taskq_create(const char *name, unsigned int nthreads, int ipl, unsigned int flags)
{
	struct taskq *tq;

	tq = p9_mallocz(sizeof(*tq));
	if (tq == NULL)
		return NULL;
	if (nthreads < 1)
		nthreads = 1;
	if (nthreads > MAXWORKERS)
		nthreads = MAXWORKERS;
	tq->tq_name = name;
	tq->tq_nthreads = nthreads;
	mtx_init(&tq->tq_mtx, ipl);
	TAILQ_INIT(&tq->tq_worklist);
	return tq;
}

void
taskq_destroy(struct taskq *tq)
{
	/* the workers stay: the file server ends with its procs */
	taskq_barrier(tq);
}

void
task_set(struct task *t, void (*fn)(void *), void *arg)
{
	t->t_func = fn;
	t->t_arg = arg;
	t->t_flags = 0;
}

int
task_add(struct taskq *tq, struct task *t)
{
	int added = 0;

	taskq_start(tq);
	mtx_enter(&tq->tq_mtx);
	if (!(t->t_flags & TASK_ONQUEUE)) {
		t->t_flags |= TASK_ONQUEUE;
		TAILQ_INSERT_TAIL(&tq->tq_worklist, t, t_entry);
		t->t_seq = ++tq->tq_seq;
		added = 1;
	}
	mtx_leave(&tq->tq_mtx);
	if (added)
		wakeup_one(tq);
	return added;
}

int
task_del(struct taskq *tq, struct task *t)
{
	int removed = 0;

	mtx_enter(&tq->tq_mtx);
	if (t->t_flags & TASK_ONQUEUE) {
		TAILQ_REMOVE(&tq->tq_worklist, t, t_entry);
		t->t_flags &= ~TASK_ONQUEUE;
		removed = 1;
	}
	if (removed && tq->tq_waiters)
		wakeup(&tq->tq_waiters);
	mtx_leave(&tq->tq_mtx);
	return removed;
}

/* under tq_mtx: is any task numbered up to seq still queued or running? */
static int
pending(struct taskq *tq, uint64_t seq, struct task *only)
{
	struct task *t;
	unsigned int i;

	for (i = 0; i < tq->tq_nthreads; i++) {
		/* a worker waiting does not wait for its own task */
		if (curproc->p_taskq == (void *)tq && curproc->p_tqworker == (int)i)
			continue;
		if (only != NULL) {
			if (tq->tq_runtask[i] == only)
				return 1;
		} else if (tq->tq_running[i] != 0 && tq->tq_running[i] <= seq)
			return 1;
	}
	if (only != NULL)
		return 0;
	t = TAILQ_FIRST(&tq->tq_worklist);	/* the oldest */
	return t != NULL && t->t_seq <= seq;
}

void
taskq_barrier(struct taskq *tq)
{
	uint64_t seq;

	mtx_enter(&tq->tq_mtx);
	seq = tq->tq_seq;
	while (pending(tq, seq, NULL)) {
		tq->tq_waiters++;
		msleep_ticks(&tq->tq_waiters, &tq->tq_mtx, PWAIT, "tqbar", 0);
		tq->tq_waiters--;
	}
	mtx_leave(&tq->tq_mtx);
}

void
taskq_del_barrier(struct taskq *tq, struct task *t)
{
	task_del(tq, t);
	mtx_enter(&tq->tq_mtx);
	while (pending(tq, 0, t)) {
		tq->tq_waiters++;
		msleep_ticks(&tq->tq_waiters, &tq->tq_mtx, PWAIT, "tqdbar", 0);
		tq->tq_waiters--;
	}
	mtx_leave(&tq->tq_mtx);
}

static void
taskworker(void *arg)
{
	struct tqworker w = *(struct tqworker *)arg;
	struct taskq *tq = w.tq;
	struct task *t;

	p9_free(arg);
	curproc->p_taskq = tq;
	curproc->p_tqworker = w.n;
	mtx_enter(&tq->tq_mtx);
	for (;;) {
		while ((t = TAILQ_FIRST(&tq->tq_worklist)) == NULL)
			msleep_ticks(tq, &tq->tq_mtx, PWAIT, "bored", 0);
		TAILQ_REMOVE(&tq->tq_worklist, t, t_entry);
		t->t_flags &= ~TASK_ONQUEUE;
		tq->tq_running[w.n] = t->t_seq;
		tq->tq_runtask[w.n] = t;
		mtx_leave(&tq->tq_mtx);

		t->t_func(t->t_arg);

		mtx_enter(&tq->tq_mtx);
		tq->tq_running[w.n] = 0;
		tq->tq_runtask[w.n] = NULL;
		if (tq->tq_waiters)
			wakeup(&tq->tq_waiters);
	}
}
