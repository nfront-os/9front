/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD's procs and tsleep(9)/wakeup(9).
 *
 * Each Plan 9 proc that runs KPI code has a struct task_struct (curproc,
 * Linux's current), made when it first asks; procs the KPI starts
 * (kpi_newproc) get theirs before they run.  A proc sleeps on its own
 * Rendez (with its QLock), with or without a time limit.
 *
 * Sleeping is OpenBSD's two steps: sleep_setup puts the proc on the
 * sleep queue of a wait channel (a hash of them, under one lock), the
 * caller lets go of its locks, sleep_finish sleeps unless a wakeup came
 * in between.  wakeup(chan) takes every proc on chan off the queue and
 * wakes it; wakeup_proc takes one proc off whatever it sleeps on.
 */
#include "obsd.h"
#include <linux/sched.h>

#define NSLEEPQ	64
#define SLEEPQ(c)	(&sleepq[((uintptr_t)(c) >> 4) % NSLEEPQ])

TAILQ_HEAD(sleepq, task_struct);
static struct sleepq sleepq[NSLEEPQ];
static struct spinlock sleepqlock = MUTEX_INITIALIZER(IPL_HIGH);
static int sleepqinit;
static pid_t nextpid = 1;

int nowake;
const int hz = 1000;
const int tick = 1000000 / 1000;

static void
initsleepq(void)
{
	int i;

	for (i = 0; i < NSLEEPQ; i++)
		TAILQ_INIT(&sleepq[i]);
	sleepqinit = 1;
}

/* the first proc that ran KPI code: the leader of all (one thread group) */
static struct task_struct *leader;

static struct task_struct *
newtask(const char *name)
{
	struct task_struct *p, *l;

	p = p9_mallocz(sizeof(*p));
	if (p == NULL)
		p9_panic("kpi: no memory for a proc");
	p9_rendez_init(p->p_rendez, p->p_qlock);
	p->p_p = &p->p_process;
	p->p_stat = SONPROC;
	p->pid = p->p_process.ps_pid = p9_getpid();
	strlcpy(p->comm, name, sizeof(p->comm));
	strlcpy(p->p_process.ps_comm, name, sizeof(p->p_process.ps_comm));
	l = NULL;
	if (!__atomic_compare_exchange_n(&leader, &l, p, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE))
		p->group_leader = l;
	else
		p->group_leader = p;
	p->tgid = p->group_leader->pid;
	return p;
}

struct task_struct *
kpi_curproc(void)
{
	void **d = p9_procdata();

	if (*d == NULL)
		*d = newtask("linux");
	return *d;
}

struct newproc {
	struct task_struct *p;
	void (*fn)(void *);
	void *arg;
};

static void
procstart(void *a)
{
	struct newproc np = *(struct newproc *)a;

	p9_free(a);
	*p9_procdata() = np.p;
	np.p->pid = np.p->p_process.ps_pid = p9_getpid();
	np.fn(np.arg);
	p9_exits(NULL);
}

struct task_struct *
kpi_newproc(const char *name, void (*fn)(void *), void *arg)
{
	struct newproc *np;
	struct task_struct *p;

	np = p9_mallocz(sizeof(*np));
	if (np == NULL)
		return NULL;
	p = newtask(name);
	p->p_flag |= P_SYSTEM;
	np->p = p;
	np->fn = fn;
	np->arg = arg;
	/* the new proc frees np: from here on only p */
	if (p9_proccreate(procstart, np, 256 * 1024) < 0) {
		p9_free(p);
		p9_free(np);
		return NULL;
	}
	return p;
}

/* the proc's own sleep, until kpi_procwake or ms (< 0: no limit); 1 if woken */
int
kpi_procsleep(struct task_struct *p, long ms)
{
	int woken;

	p9_qlock(p->p_qlock);
	if (!p->p_woken) {
		if (ms < 0)
			p9_rsleep(p->p_rendez);
		else if (ms > 0)
			p9_rsleep_ms(p->p_rendez, ms);
	}
	woken = p->p_woken;
	p->p_woken = 0;
	p9_qunlock(p->p_qlock);
	return woken;
}

void
kpi_procwake(struct task_struct *p)
{
	p9_qlock(p->p_qlock);
	p->p_woken = 1;
	p9_rwakeup(p->p_rendez);
	p9_qunlock(p->p_qlock);
}

void
sleep_setup(const volatile void *ident, int prio, const char *wmesg)
{
	struct task_struct *p = curproc;

	mtx_enter(&sleepqlock);
	if (!sleepqinit)
		initsleepq();
	if (p->p_wchan != NULL)
		TAILQ_REMOVE(SLEEPQ(p->p_wchan), p, p_runq);
	p->p_wchan = ident;
	p->p_wmesg = wmesg;
	p->p_stat = SSLEEP;
	p->p_flag |= P_WSLEEP;
	if (prio & PCATCH)
		p->p_flag |= P_SINTR;
	TAILQ_INSERT_TAIL(SLEEPQ(ident), p, p_runq);
	mtx_leave(&sleepqlock);
}

/* off the sleep queue; under sleepqlock */
static int
unqueue(struct task_struct *p)
{
	if (p->p_wchan == NULL)
		return 0;
	TAILQ_REMOVE(SLEEPQ(p->p_wchan), p, p_runq);
	p->p_wchan = NULL;
	p->p_stat = SONPROC;
	p->p_flag &= ~(P_WSLEEP | P_SINTR);
	return 1;
}

void
unsleep(struct task_struct *p)
{
	mtx_enter(&sleepqlock);
	unqueue(p);
	mtx_leave(&sleepqlock);
}

/*
 * nsecs: the time limit (INFSLP: none), as OpenBSD's sleep_finish since
 * 7.x, whose linux/wait.h we have (an int of ticks took 5 s as 8 days)
 */
int
sleep_finish(uint64_t nsecs, int do_sleep)
{
	struct task_struct *p = curproc;
	int woken;
	long ms;

	if (do_sleep) {
		/* a wakeup since sleep_setup took us off the queue and set p_woken */
		if (nsecs == INFSLP)
			ms = -1;
		else if (nsecs >= (uint64_t)LONG_MAX / 2)
			ms = LONG_MAX / 2;
		else
			ms = (nsecs + 999999) / 1000000;
		if (ms == 0)
			ms = 1;
		woken = kpi_procsleep(p, ms);
	} else
		woken = 0;
	mtx_enter(&sleepqlock);
	if (unqueue(p))
		woken = 0;	/* nobody woke us: timed out, or did not sleep */
	else
		woken = 1;
	mtx_leave(&sleepqlock);
	/* a wakeup that came after the time was up must not wake the next sleep */
	p9_qlock(p->p_qlock);
	p->p_woken = 0;
	p9_qunlock(p->p_qlock);
	return (woken || !do_sleep || nsecs == INFSLP) ? 0 : EWOULDBLOCK;
}

int
wakeup_proc(struct task_struct *p, int flags)
{
	int r;

	mtx_enter(&sleepqlock);
	r = unqueue(p);
	mtx_leave(&sleepqlock);
	if (r)
		kpi_procwake(p);
	return r;
}

void
wakeup_n(const volatile void *ident, int n)
{
	struct task_struct *p, *np;
	struct task_struct *woke[32];
	int nw, i, more;

	do {
		nw = 0;
		more = 0;
		mtx_enter(&sleepqlock);
		if (!sleepqinit)
			initsleepq();
		TAILQ_FOREACH_SAFE(p, SLEEPQ(ident), p_runq, np) {
			if (p->p_wchan != ident)
				continue;
			if (nw == nitems(woke)) {
				more = 1;
				break;
			}
			unqueue(p);
			woke[nw++] = p;
			if (n > 0 && --n == 0)
				break;
		}
		mtx_leave(&sleepqlock);
		for (i = 0; i < nw; i++)
			kpi_procwake(woke[i]);
	} while (more);
}

void
wakeup(const volatile void *ident)
{
	wakeup_n(ident, -1);
}

static int
nsectoticks(uint64_t nsecs)
{
	uint64_t t;

	if (nsecs == INFSLP)
		return 0;
	t = (nsecs + 1000000000ULL / hz - 1) / (1000000000ULL / hz);
	if (t == 0)
		t = 1;
	if (t > INT_MAX)
		t = INT_MAX;
	return t;
}

/* ticks (0: no limit) as sleep_finish's time limit */
static uint64_t
tickstonsec(int timo)
{
	if (timo <= 0)
		return INFSLP;
	return (uint64_t)timo * (1000000000ULL / hz);
}

int
tsleep(const volatile void *ident, int prio, const char *wmesg, int timo)
{
	sleep_setup(ident, prio, wmesg);
	return sleep_finish(tickstonsec(timo), 1);
}

int
tsleep_nsec(const volatile void *ident, int prio, const char *wmesg, uint64_t nsecs)
{
	return tsleep(ident, prio, wmesg, nsectoticks(nsecs));
}

int
msleep_ticks(const volatile void *ident, struct spinlock *mtx, int prio, const char *wmesg, int timo)
{
	int r;

	sleep_setup(ident, prio, wmesg);
	mtx_leave(mtx);
	r = sleep_finish(tickstonsec(timo), 1);
	mtx_enter(mtx);
	return r;
}

int
msleep_nsec(const volatile void *ident, struct spinlock *mtx, int prio, const char *wmesg, uint64_t nsecs)
{
	return msleep_ticks(ident, mtx, prio, wmesg, nsectoticks(nsecs));
}

int
rwsleep(const volatile void *ident, struct rwlock *rwl, int prio, const char *wmesg, int timo)
{
	int r, write;

	write = rw_status(rwl) == RW_WRITE;
	sleep_setup(ident, prio, wmesg);
	rw_exit(rwl);
	r = sleep_finish(tickstonsec(timo), 1);
	if (write)
		rw_enter_write(rwl);
	else
		rw_enter_read(rwl);
	return r;
}

int
rwsleep_nsec(const volatile void *ident, struct rwlock *rwl, int prio, const char *wmesg, uint64_t nsecs)
{
	return rwsleep(ident, rwl, prio, wmesg, nsectoticks(nsecs));
}

/* OpenBSD's yield; Linux's yield is the same (libthread has a yield of its own) */
void
kpi_yield(void)
{
	p9_yield();
}

void
sched_pause(void (*fn)(void))
{
	fn();
}

/*
 * the file server's procs may do what the file server may, but for a
 * client's ioctl the client's right counts: only the hostowner's are
 * a superuser's (capable(CAP_SYS_ADMIN): DRM_ROOT_ONLY and amdgpu's own
 * checks), as on Linux only root's
 */
int
suser(struct task_struct *p)
{
	return p->unpriv ? EPERM : 0;
}

/* kthread(9) */
int
kthread_create(void (*fn)(void *), void *arg, struct task_struct **pp, const char *fmt, ...)
{
	struct task_struct *p;
	char name[MAXCOMLEN + 1];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(name, sizeof(name), fmt, ap);
	va_end(ap);
	p = kpi_newproc(name, fn, arg);
	if (p == NULL)
		return ENOMEM;
	if (pp != NULL)
		*pp = p;
	return 0;
}

void
kthread_exit(int ecode)
{
	p9_exits(ecode ? "kthread" : NULL);
	for (;;)
		;
}
