/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD's timeout(9), Linux's timer_list and
 * hrtimer on it (linux/timer.h, linux/hrtimer.h).
 *
 * One timeout proc runs the functions: the queue is in order of the
 * deadlines (nanoseconds of uptime, p9_nsec), the proc sleeps until the
 * first one or until a new first one comes.  A function runs without
 * the queue's lock; timeout_del_barrier waits until it has returned.
 */
#include "obsd.h"
#include <sys/timeout.h>
#include <linux/timer.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

static struct spinlock tolock = MUTEX_INITIALIZER(IPL_HIGH);
static TAILQ_HEAD(, timeout) toq = TAILQ_HEAD_INITIALIZER(toq);
static struct timeout *torunning;
static uint64_t torunsince;	/* when torunning's function started (status) */
static struct task_struct *toproc;
static int towaiters;

unsigned long
kpi_jiffies(void)
{
	return p9_nsec() / (1000000000LL / hz);
}

static void toprocfn(void *);

/* the timeout proc, made once, under no lock (proccreate switches threads) */
static void
tostart(void)
{
	static uint64_t once[4];	/* a QLock */
	struct task_struct *p;

	if (READ_ONCE(toproc) != NULL)
		return;
	p9_qlock(once);
	if (toproc == NULL) {
		p = kpi_newproc("timeout", toprocfn, NULL);
		if (p == NULL)
			p9_panic("kpi: no timeout proc");
		membar_producer();
		toproc = p;
	}
	p9_qunlock(once);
}

void
timeout_set(struct timeout *to, void (*fn)(void *), void *arg)
{
	timeout_set_flags(to, fn, arg, KCLOCK_NONE, 0);
}

void
timeout_set_flags(struct timeout *to, void (*fn)(void *), void *arg, int kclock, int flags)
{
	to->to_func = fn;
	to->to_arg = arg;
	to->to_time = 0;
	to->to_flags = flags | TIMEOUT_INITIALIZED;
	to->to_kclock = kclock;
}

void
timeout_set_proc(struct timeout *to, void (*fn)(void *), void *arg)
{
	timeout_set_flags(to, fn, arg, KCLOCK_NONE, TIMEOUT_PROC);
}

/* to the queue at nsec of uptime; 1 if it was not pending, as OpenBSD's */
static int
toadd(struct timeout *to, uint64_t when)
{
	struct timeout *t;
	int wasnew, first;

	tostart();
	mtx_enter(&tolock);
	wasnew = !(to->to_flags & TIMEOUT_ONQUEUE);
	if (!wasnew)
		TAILQ_REMOVE(&toq, to, to_list);
	to->to_time = when;
	to->to_flags |= TIMEOUT_ONQUEUE;
	to->to_flags &= ~TIMEOUT_TRIGGERED;
	TAILQ_FOREACH(t, &toq, to_list)
		if (t->to_time > when)
			break;
	if (t != NULL)
		TAILQ_INSERT_BEFORE(t, to, to_list);
	else
		TAILQ_INSERT_TAIL(&toq, to, to_list);
	first = TAILQ_FIRST(&toq) == to;
	mtx_leave(&tolock);
	if (first)
		kpi_procwake(toproc);
	return wasnew;
}

int
timeout_add_nsec(struct timeout *to, uint64_t nsecs)
{
	if (nsecs > INT64_MAX / 2)
		nsecs = INT64_MAX / 2;
	return toadd(to, p9_nsec() + nsecs);
}

int
timeout_add(struct timeout *to, int nticks)
{
	if (nticks < 1)
		nticks = 1;	/* the next tick, as OpenBSD's */
	return timeout_add_nsec(to, (uint64_t)nticks * (1000000000ULL / hz));
}

int
timeout_add_sec(struct timeout *to, int s)
{
	return timeout_add_nsec(to, SEC_TO_NSEC(s));
}

int
timeout_add_msec(struct timeout *to, uint64_t ms)
{
	return timeout_add_nsec(to, MSEC_TO_NSEC(ms));
}

int
timeout_add_usec(struct timeout *to, uint64_t us)
{
	return timeout_add_nsec(to, USEC_TO_NSEC(us));
}

int
timeout_add_tv(struct timeout *to, const struct timeval *tv)
{
	return timeout_add_nsec(to, TIMEVAL_TO_NSEC(tv));
}

int
timeout_abs_ts(struct timeout *to, const struct timespec *ts)
{
	return toadd(to, TIMESPEC_TO_NSEC(ts));
}

int
timeout_del(struct timeout *to)
{
	int was;

	mtx_enter(&tolock);
	was = (to->to_flags & TIMEOUT_ONQUEUE) != 0;
	if (was) {
		TAILQ_REMOVE(&toq, to, to_list);
		to->to_flags &= ~TIMEOUT_ONQUEUE;
	}
	mtx_leave(&tolock);
	return was;
}

void
timeout_barrier(struct timeout *to)
{
	mtx_enter(&tolock);
	while (torunning == to && curproc != toproc) {
		towaiters++;
		msleep_ticks(&torunning, &tolock, PWAIT, "tobar", 0);
	}
	mtx_leave(&tolock);
}

int
timeout_del_barrier(struct timeout *to)
{
	int was;

	was = timeout_del(to);
	timeout_barrier(to);
	return was;
}

static void
toprocfn(void *arg)
{
	struct timeout *to;
	uint64_t now;
	long ms;

	for (;;) {
		mtx_enter(&tolock);
		now = p9_nsec();
		while ((to = TAILQ_FIRST(&toq)) != NULL && to->to_time <= now) {
			TAILQ_REMOVE(&toq, to, to_list);
			to->to_flags &= ~TIMEOUT_ONQUEUE;
			to->to_flags |= TIMEOUT_TRIGGERED;
			torunning = to;
			torunsince = now;
			mtx_leave(&tolock);
			to->to_func(to->to_arg);
			mtx_enter(&tolock);
			torunning = NULL;
			if (towaiters) {
				towaiters = 0;
				wakeup(&torunning);
			}
			now = p9_nsec();
		}
		if (to == NULL)
			ms = -1;
		else
			ms = (to->to_time - now + 999999) / 1000000;
		mtx_leave(&tolock);
		kpi_procsleep(curproc, ms);
	}
}

/* Linux's timer_list (linux/timer.h): the timeout calls the timer's function */
void
kpi_timer_fire(void *arg)
{
	struct timer_list *t = arg;

	t->function(t);
}

/* Linux's hrtimer (linux/hrtimer.h) */
static void
hrtimer_fire(void *arg)
{
	struct hrtimer *h = arg;

	if (h->function(h) == HRTIMER_RESTART)
		toadd(&h->to, h->expires);
}

void
hrtimer_init(struct hrtimer *h, int clock, enum hrtimer_mode mode)
{
	memset(h, 0, sizeof(*h));
	h->clock = clock;
	timeout_set(&h->to, hrtimer_fire, h);
}

void
hrtimer_start_range_ns(struct hrtimer *h, int64_t t, uint64_t slack, enum hrtimer_mode mode)
{
	if (mode & HRTIMER_MODE_REL)
		t += ktime_get();
	h->expires = t;
	toadd(&h->to, t > 0 ? t : 0);
}

uint64_t
hrtimer_forward(struct hrtimer *h, int64_t now, int64_t interval)
{
	uint64_t n;

	if (interval <= 0 || now < h->expires)
		return 0;
	n = (now - h->expires) / interval + 1;
	h->expires += n * interval;
	return n;
}

int
hrtimer_try_to_cancel(struct hrtimer *h)
{
	int running;

	mtx_enter(&tolock);
	running = torunning == &h->to;
	mtx_leave(&tolock);
	if (running)
		return -1;
	return timeout_del(&h->to);
}

int
hrtimer_cancel(struct hrtimer *h)
{
	return timeout_del_barrier(&h->to);
}

/* the timeout proc's state, for a driver's status file (diagnosis) */
int
kpi_timeout_status(char *p, int n)
{
	struct timeout *to;
	uint64_t now;
	int len, q;

	mtx_enter(&tolock);
	now = p9_nsec();
	q = 0;
	TAILQ_FOREACH(to, &toq, to_list)
		q++;
	to = TAILQ_FIRST(&toq);
	len = snprintf(p, n, "timeouts %d queued, first %lld ms from now (%p)", q,
	    to != NULL ? ((long long)to->to_time - (long long)now) / 1000000 : 0LL,
	    to != NULL ? (void *)to->to_func : NULL);
	if (torunning != NULL && len < n)
		len += snprintf(p + len, n - len, ", running %p for %lld ms",
		    torunning->to_func == kpi_timer_fire ?
		    (void *)((struct timer_list *)torunning->to_arg)->function : (void *)torunning->to_func,
		    ((long long)now - (long long)torunsince) / 1000000);
	mtx_leave(&tolock);
	if (len < n)
		len += snprintf(p + len, n - len, "\n");
	return len < n ? len : n - 1;
}
