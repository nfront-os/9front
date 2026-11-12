/*
 * SPDX-License-Identifier: MIT
 *
 * LinuxKPI for 9front: the Plan 9 side (kpi/include/kpi/p9.h).
 * Sees only Plan 9's headers; the Linux side calls these.
 */
#include <u.h>
#include <libc.h>
#include <thread.h>
#include "../include/kpi/p9.h"

void p9_lock(void *l) { lock(l); }
void p9_unlock(void *l) { unlock(l); }
int p9_canlock(void *l) { return canlock(l); }
int p9_locked(void *l) { return ((Lock*)l)->val != 0; }
void p9_qlock(void *q) { qlock(q); }
void p9_qunlock(void *q) { qunlock(q); }
int p9_canqlock(void *q) { return canqlock(q); }
int p9_qlocked(void *q) { return ((QLock*)q)->locked != 0; }
void p9_rlock(void *rw) { rlock(rw); }
void p9_runlock(void *rw) { runlock(rw); }
void p9_wlock(void *rw) { wlock(rw); }
void p9_wunlock(void *rw) { wunlock(rw); }
int p9_canrlock(void *rw) { return canrlock(rw); }
int p9_canwlock(void *rw) { return canwlock(rw); }
int p9_rwlocked(void *rw) { RWLock *l = rw; return l->readers != 0 || l->writer != 0; }

void
p9_rendez_init(void *rendez, void *qlock)
{
	Rendez *r = rendez;

	memset(r, 0, sizeof *r);
	r->l = qlock;
}

void p9_rsleep(void *r) { rsleep(r); }
int p9_rwakeup(void *r) { return rwakeup(r); }
int p9_rwakeupall(void *r) { return rwakeupall(r); }

/*
 * rsleep with a time limit: the sleeper puts a deadline on a list that
 * one waker proc watches; at the deadline the waker takes the
 * sleeper's QLock and wakes the Rendez, unless the sleeper woke first.
 * Lock order: the QLock before wl; the waker claims entries under wl
 * and wakes them after letting go of it.  The waker waits on a timed
 * semaphore until the nearest deadline (a second at most); a sleeper
 * whose deadline comes before the waker means to look again releases
 * it (with a plain sleep the waker slept up to its 100 ms limit after
 * a quiet spell: msleep(1) took 45 ms on average, amdgpu's 2000 x
 * msleep(1) for the firmware's ready flag over 90 s).
 */
typedef struct Waiter Waiter;
struct Waiter {
	Rendez	*r;
	vlong	deadline;
	int	claimed;	/* the waker took it off the list (under wl) */
	int	done;		/* the sleeper is awake (under r->l) */
	int	fired;		/* the waker woke it (under r->l) */
	int	wokedone;	/* the waker is through with it (under r->l) */
	Waiter	*next;
};

static Lock wl;
static Waiter *wlist;
static int waker;
static Rendez wakerr;
static QLock wakerq;
static long wakersem;		/* released: a nearer deadline */
static vlong wakerat;		/* when the waker looks again (under wl) */

static void
wakerproc(void*)
{
	Waiter *w, **l, *exp;
	vlong now, next;

	for(;;){
		lock(&wl);
		now = nsec();
		exp = nil;
		next = now + 1000000000LL;	/* a second at most */
		for(l = &wlist; (w = *l) != nil;){
			if(w->deadline <= now){
				*l = w->next;
				w->claimed = 1;
				w->next = exp;
				exp = w;
			}else{
				if(w->deadline < next)
					next = w->deadline;
				l = &w->next;
			}
		}
		wakerat = next;
		unlock(&wl);
		while((w = exp) != nil){
			exp = w->next;
			qlock(w->r->l);
			if(!w->done){
				w->fired = 1;
				rwakeupall(w->r);
			}
			w->wokedone = 1;
			qunlock(w->r->l);
		}
		next = (next - nsec() + 999999) / 1000000;
		if(next < 1)
			next = 1;
		tsemacquire(&wakersem, next);
	}
}

int
p9_rsleep_ms(void *rendez, long ms)
{
	Rendez *r = rendez;
	Waiter w, **l;
	int kick;

	if(ms <= 0)
		return 0;
	memset(&w, 0, sizeof w);
	w.r = r;
	w.deadline = nsec() + (vlong)ms * 1000000;
	/* the waker, made once, under no lock (proccreate switches threads) */
	if(!waker){
		qlock(&wakerq);
		if(!waker){
			USED(wakerr);
			proccreate(wakerproc, nil, 16384);
			waker = 1;
		}
		qunlock(&wakerq);
	}
	lock(&wl);
	w.next = wlist;
	wlist = &w;
	kick = w.deadline < wakerat;
	unlock(&wl);
	if(kick)
		semrelease(&wakersem, 1);

	rsleep(r);	/* r->l held again */

	w.done = 1;
	lock(&wl);
	if(!w.claimed)
		for(l = &wlist; *l != nil; l = &(*l)->next)
			if(*l == &w){
				*l = w.next;
				break;
			}
	unlock(&wl);
	/* the waker may hold w still: wait until it is through */
	while(w.claimed && !w.wokedone){
		qunlock(r->l);
		sleep(0);
		qlock(r->l);
	}
	return !w.fired;
}

int p9_proccreate(void (*fn)(void*), void *arg, unsigned int stack) { return proccreate(fn, arg, stack); }
int p9_getpid(void) { return getpid(); }
void **p9_procdata(void) { return threaddata(); }
void p9_sleep(long ms) { sleep(ms); }
void p9_yield(void) { sleep(0); }
void p9_exits(const char *why) { threadexits((char*)why); }

/*
 * the nanoseconds since boot (the fourth number of /dev/bintime): Linux's
 * ktime_get, CLOCK_MONOTONIC, as APE's clock_gettime has it, so that the
 * absolute deadlines Mesa computes (amdgpu's waits) mean the same here;
 * nsec() is the epoch's, which moves when the time is set
 */
static int bintimefd = -1;

long long
p9_nsec(void)
{
	uchar b[32];
	uvlong v;
	int fd, i, old;

	if((fd = bintimefd) < 0){
		if((fd = open("/dev/bintime", OREAD|OCEXEC)) < 0)
			return nsec();
		old = -1;
		if(!__atomic_compare_exchange_n(&bintimefd, &old, fd, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)){
			close(fd);	/* another proc was first */
			fd = old;
		}
	}
	if(pread(fd, b, sizeof b, 0) != sizeof b)
		return nsec();
	v = 0;
	for(i = 24; i < 32; i++)
		v = v << 8 | b[i];
	return v;
}
long long p9_realnsec(void) { return nsec(); }

/* Linux's floating point environment: every exception masked (machine/fpu.h) */
void
p9_fpu_kernel(void)
{
	setfcr(getfcr() & ~(FPINEX|FPUNFL|FPOVFL|FPZDIV|FPINVAL));
}

/* the machine's processors: a line each in /dev/sysstat; 1 if it cannot be read */
int
p9_ncpus(void)
{
	char buf[8192];
	int fd, n, i, lines;

	fd = open("/dev/sysstat", OREAD);
	if(fd < 0)
		return 1;
	lines = 0;
	while((n = read(fd, buf, sizeof buf)) > 0)
		for(i = 0; i < n; i++)
			if(buf[i] == '\n')
				lines++;
	close(fd);
	return lines > 0 ? lines : 1;
}

void *p9_malloc(unsigned long n) { return malloc(n); }
void *p9_mallocz(unsigned long n) { return mallocz(n, 1); }
void *p9_mallocalign(unsigned long n, unsigned long align) { return mallocalign(n, align, 0, 0); }
void *p9_realloc(void *p, unsigned long n) { return realloc(p, n); }
void p9_free(void *p) { free(p); }
unsigned long p9_msize(void *p) { return msize(p); }

void
p9_print(int fd, const char *s, long n)
{
	write(fd, (char*)s, n);
}

void
p9_random(void *buf, long n)
{
	uchar *p = buf;
	ulong r;

	while(n > 0){
		r = truerand();
		memmove(p, &r, n < 4 ? n : 4);
		p += 4;
		n -= 4;
	}
}

/*
 * the machine down as 9front takes it down (fshalt: the file servers
 * synced and halted, then scram: power off; with reboot fshalt -r),
 * started as a process of its own; scram alone if fshalt cannot run
 */
void
p9_poweroff(int reboot)
{
	switch(rfork(RFPROC|RFNOWAIT|RFFDG|RFNOTEG|RFREND)){
	case -1:
		fprint(2, "%s: power off: fork: %r\n", argv0 ? argv0 : "linux");
		return;
	case 0:
		execl("/bin/rc", "rc", "-c", reboot ? "fshalt -r || reboot" : "fshalt || scram", nil);
		fprint(2, "%s: power off: exec rc: %r\n", argv0 ? argv0 : "linux");
		_exits("exec");
	}
}

void
p9_panic(const char *why)
{
	fprint(2, "%s: panic: %s\n", argv0 ? argv0 : "linux", why);
	abort();
}

const char*
p9_sysname(void)
{
	return sysname();
}

int
p9_readfile(const char *path, void **data, long *size)
{
	Dir *d;
	char *p;
	long n, m;
	int fd;

	if((fd = open((char*)path, OREAD)) < 0)
		return -1;
	if((d = dirfstat(fd)) == nil){
		close(fd);
		return -1;
	}
	n = d->length;
	free(d);
	if((p = malloc(n ? n : 1)) == nil){
		close(fd);
		return -1;
	}
	m = readn(fd, p, n);
	close(fd);
	if(m != n){
		free(p);
		return -1;
	}
	*data = p;
	*size = n;
	return 0;
}
