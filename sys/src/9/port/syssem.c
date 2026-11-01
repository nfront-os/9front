/*
 * NIX optimistic user semaphores and the kernel's side of tubes,
 * after the NIX kernel (Lsub, Bell Labs, Sandia, Vita Nuova;
 * port/syssem.c; MIT license, see /lib/legal/nix).
 *
 * A semaphore is an int in user memory, shared by the processes
 * using it (libc upsem(2)). n > 0: there are n tickets; n == 0:
 * none; n < 0: |n| processes wait for one or are about to.
 * upsem and downsem change it with an atomic increment and
 * decrement and enter the kernel only to wake a process
 * (semwakeup) or to wait (semsleep, semalt).
 *
 * The kernel keeps a Usem (NIX's Sem; sd53c8xx.c has an enum Sem)
 * for each semaphore it has seen, in the
 * semaphore's segment (segmksem, port/segment.c), with the
 * processes waiting on it. A process waits in sleep() on its own
 * Rendez (Proc.semr) until a waker hands it a ticket by setting
 * Proc.waitsem. A note interrupts the wait; the process then gives
 * its down back and the system call fails (error Eintr).
 *
 * Differences to NIX: no scheduler state Semdown (sleep() makes
 * notes and kill work, /proc shows "Semdown" as psstate); the Usem is
 * a QLock (the kernel reads the user's int while holding it, which
 * may fault); a process that leaves a semaphore it waited on, after
 * semalt or a note, always gives its down back (NIX lost one when
 * it was still queued); a non-blocking downsem that lost the race
 * returns -1 (NIX returned 0); semalt queues on all semaphores (NIX
 * overwrote the count with the first decrement); a process that
 * queues itself hands out the tickets the queue has (semwake), an up
 * may have come before it was queued (NIX lost that ticket).
 *
 * Kernel processes may share semaphores and tubes with user
 * processes through the Usem (upsem, downsem, altsems) and KTube
 * (u2ktube, tsend, trecv, ...) entry points; see devtube.c.
 */
#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"

#include	"../port/tube.h"

#ifndef semcas
#define semcas(p, o, n)	cmpswap((long*)(p), (o), (n))
#endif

static int semtrytimes = 100;

static int
semainc(int *p)
{
	int v;

	do
		v = *p;
	while(!semcas(p, v, v+1));
	return v+1;
}

static int
semadec(int *p)
{
	int v;

	do
		v = *p;
	while(!semcas(p, v, v-1));
	return v-1;
}

/*
 * With s locked: if more processes wait than |*s->np|, there are
 * tickets they have not seen; hand them out, first come first.
 */
static void
semwake(Usem *s)
{
	Proc *p;

	while(s->nq > 0 && s->nq > -*s->np){
		p = s->q[0];
		s->nq--;
		memmove(s->q, s->q+1, s->nq * sizeof s->q[0]);
		p->waitsem = s;
		wakeup(&p->semr);
	}
}

/*
 * With s locked: wait on s. The caller then calls semwake: an up may
 * have come between our decrement and now, while fewer processes than
 * waited were queued (it saw nq <= -*np and woke none); NIX lost that
 * ticket.
 */
static void
semqueue(Usem *s)
{
	s->q = realloc(s->q, (s->nq+1) * sizeof s->q[0]);
	if(s->q == nil)
		panic("semqueue: no memory");
	s->q[s->nq++] = up;
}

/*
 * Leave the semaphores ss[0..n-1] but keep, our down on each one
 * is given back: we are still queued (no ticket yet) or a waker
 * took us out with one we no longer want.
 */
static void
semcancel(Usem **ss, int n, Usem *keep)
{
	int i, j;
	Usem *s;

	for(i = 0; i < n; i++){
		s = ss[i];
		if(s == keep)
			continue;
		qlock(s);
		for(j = 0; j < s->nq; j++)
			if(s->q[j] == up){
				s->nq--;
				memmove(s->q+j, s->q+j+1, (s->nq-j) * sizeof s->q[0]);
				break;
			}
		semainc(s->np);
		semwake(s);
		qunlock(s);
	}
}

static int
semgot(void *a)
{
	return ((Proc*)a)->waitsem != nil;
}

/*
 * We are queued on ss[0..n-1]: wait until one of them gives us a
 * ticket, leave the others and return it.
 */
static Usem*
semwait(Usem **ss, int n)
{
	char *ps;
	Usem *w;

	ps = up->psstate;
	if(waserror()){
		up->psstate = ps;
		semcancel(ss, n, nil);
		nexterror();
	}
	up->psstate = "Semdown";
	while(!semgot(up))
		sleep(&up->semr, semgot, up);
	up->psstate = ps;
	poperror();
	w = up->waitsem;
	semcancel(ss, n, w);
	return w;
}

/*
 * The caller made *s->np negative (a down without a ticket in
 * sight). Wait for a ticket and return 0, or, dontblock, give the
 * down back and return -1.
 */
static int
semsleep(Usem *s, int dontblock)
{
	if(dontblock){
		/*
		 * Someone took the ticket between our look at the
		 * semaphore and our decrement: undo the decrement and
		 * wake whoever waits for the ticket we kept.
		 */
		semainc(s->np);
		qlock(s);
		semwake(s);
		qunlock(s);
		return -1;
	}
	qlock(s);
	if(*s->np >= 0){
		/* a ticket came while we were entering the kernel */
		qunlock(s);
		return 0;
	}
	up->waitsem = nil;
	semqueue(s);
	semwake(s);
	qunlock(s);
	semwait(&s, 1);
	return 0;
}

/*
 * Down one of ss[0..n-1]; returns its index.
 */
static int
semalt(Usem **ss, int n)
{
	int i;
	Usem *s, *w;

	up->waitsem = nil;
	for(i = 0; i < n; i++){
		s = ss[i];
		if(semadec(s->np) >= 0){
			semcancel(ss, i, nil);
			return i;
		}
		qlock(s);
		if(*s->np >= 0){
			qunlock(s);
			semcancel(ss, i, nil);
			return i;
		}
		semqueue(s);
		semwake(s);
		qunlock(s);
		if(up->waitsem != nil){
			/* a ticket already: no more downs */
			n = i+1;
			break;
		}
	}
	w = semwait(ss, n);
	for(i = 0; i < n; i++)
		if(ss[i] == w)
			return i;
	panic("semalt");
	return -1;
}

/* the Usem of the user's semaphore at np */
static Usem*
usem(int *np)
{
	Segment *sg;

	evenaddr((uintptr)np);
	sg = seg(up, (uintptr)np, 0);
	if(sg == nil || (sg->type&SG_RONLY) != 0 || (uintptr)np+sizeof(int) > sg->top){
		validaddr((uintptr)np, sizeof(int), 1);
		error(Ebadarg);
	}
	return segmksem(sg, np);
}

/* int semsleep(int *s, int dontblock) */
uintptr
syssemsleep(uchar *list)
{
	int *np, dontblock;

	np = ARG(list, int*);
	dontblock = ARG(list, int);
	return (uintptr)semsleep(usem(np), dontblock);
}

/* void semwakeup(int *s) */
uintptr
syssemwakeup(uchar *list)
{
	Usem *s;

	s = usem(ARG(list, int*));
	qlock(s);
	semwake(s);
	qunlock(s);
	return 0;
}

/* int semalt(int *ss[], int n) */
uintptr
syssemalt(uchar *list)
{
	int **sl, *nl[16];
	int i, ns;
	Usem *ksl[nelem(nl)];

	sl = ARG(list, int**);
	ns = ARG(list, int);
	if(ns <= 0 || ns > nelem(nl))
		error(Ebadarg);
	evenaddr((uintptr)sl);
	validaddr((uintptr)sl, ns * sizeof(int*), 0);
	memmove(nl, sl, ns * sizeof(int*));
	for(i = 0; i < ns; i++)
		ksl[i] = usem(nl[i]);
	return (uintptr)semalt(ksl, ns);
}

/*
 * The C library's entry points, for kernel processes that share
 * semaphores with user processes. Within the kernel a semaphore is
 * used through its Usem, not its int*: no lookup each time.
 * They must be run in a process context; downsem and altsems may
 * raise an error (Eintr).
 */

void
upsem(Usem *s)
{
	coherence();	/* what the holder wrote before the unit it gives */
	if(semainc(s->np) <= 0){
		qlock(s);
		semwake(s);
		qunlock(s);
	}
}

int
downsem(Usem *s, int dontblock)
{
	int i;

	for(i = 0; *s->np <= 0 && i < semtrytimes; i++)
		yield();
	if(*s->np <= 0 && dontblock)
		return -1;
	if(semadec(s->np) < 0)
		return semsleep(s, dontblock);
	return 0;
}

int
altsems(Usem *ss[], int n)
{
	int i, w;

	/* busy wait */
	for(w = 0; w < semtrytimes; w++){
		for(i = 0; i < n; i++)
			if(*ss[i]->np > 0)
				break;
		if(i < n)
			break;
		yield();
	}
	for(i = 0; i < n; i++)
		if(downsem(ss[i], 1) != -1)
			return i;
	return semalt(ss, n);
}

/*
 * Tube entry points as seen by the kernel.
 * They use Usem*, and not int*.
 */

enum{Doblock, Dontblock, Already};	/* xsend() nb argument */

/*
 * Make a KTube for the user's tube at t, to use it with the
 * functions below. t must be valid in up's address space; the
 * caller frees the KTube with freektube.
 */
KTube*
u2ktube(Tube *t)
{
	KTube *kt;
	Segment *sg;

	evenaddr((uintptr)t);
	validaddr((uintptr)t, sizeof *t, 1);
	sg = seg(up, (uintptr)t, 0);
	if(sg == nil || (uintptr)&t[1] > sg->top)
		error(Ebadarg);
	if(t->msz <= 0 || t->tsz <= 0)
		error(Ebadarg);
	kt = smalloc(sizeof *kt);
	kt->t = t;
	kt->msz = t->msz;
	kt->hole = segmksem(sg, &t->nhole);
	kt->msg = segmksem(sg, &t->nmsg);
	return kt;
}

void
freektube(KTube *kt)
{
	free(kt);
}

/*
 * The message slot n of kt's tube, after a check that the tube
 * (which the user may change at any time) still has the message
 * size of kt and points into user memory.
 */
static uchar*
tslot(KTube *kt, int n)
{
	int tsz;
	uintptr c;
	Tube *t;

	t = kt->t;
	tsz = t->tsz;
	if(t->msz != kt->msz || tsz <= 0)
		error(Ebadarg);
	c = (uintptr)&t[1] + (uintptr)(1 + kt->msz) * ((uint)n % (uint)tsz);
	validaddr(c, 1 + kt->msz, 1);
	return (uchar*)c;
}

static int
xsend(KTube *kt, void *p, int nb)
{
	uchar *c;
	Tube *t;

	t = kt->t;
	assert(t != nil && p != nil);
	if(nb != Already && downsem(kt->hole, nb) < 0)
		return -1;
	c = tslot(kt, semainc(&t->tl) - 1);
	memmove(c+1, p, kt->msz);
	coherence();
	*c = 1;
	upsem(kt->msg);
	return 0;
}

static int
xrecv(KTube *kt, void *p, int nb)
{
	uchar *c;
	Tube *t;

	t = kt->t;
	assert(t != nil && p != nil);
	if(nb != Already && downsem(kt->msg, nb) < 0)
		return -1;
	c = tslot(kt, semainc(&t->hd) - 1);
	while(*c == 0)
		yield();
	coherence();	/* the message after its flag */
	memmove(p, c+1, kt->msz);
	coherence();
	*c = 0;
	upsem(kt->hole);
	return 0;
}

/* p holds kt->msz bytes */
void
tsend(KTube *t, void *p)
{
	xsend(t, p, Doblock);
}

void
trecv(KTube *t, void *p)
{
	xrecv(t, p, Doblock);
}

int
nbtsend(KTube *t, void *p)
{
	return xsend(t, p, Dontblock);
}

int
nbtrecv(KTube *t, void *p)
{
	return xrecv(t, p, Dontblock);
}

int
talt(KTalt a[], int na)
{
	int i, n, *ai;
	Usem **ss;

	assert(a != nil && na > 0);
	ss = smalloc(sizeof(Usem*) * na);
	ai = smalloc(sizeof(int) * na);
	if(waserror()){
		free(ss);
		free(ai);
		nexterror();
	}
	n = 0;
	for(i = 0; i < na; i++)
		switch(a[i].op){
		case TSND:
			ai[n] = i;
			ss[n++] = a[i].kt->hole;
			break;
		case TRCV:
			ai[n] = i;
			ss[n++] = a[i].kt->msg;
			break;
		case TNBSND:
			if(nbtsend(a[i].kt, a[i].m) != -1)
				goto Done;
			break;
		case TNBRCV:
			if(nbtrecv(a[i].kt, a[i].m) != -1)
				goto Done;
			break;
		}
	i = -1;
	if(n == 0)
		goto Done;
	i = ai[altsems(ss, n)];
	switch(a[i].op){
	case TSND:
		xsend(a[i].kt, a[i].m, Already);
		break;
	case TRCV:
		xrecv(a[i].kt, a[i].m, Already);
		break;
	default:
		panic("talt");
	}
Done:
	poperror();
	free(ss);
	free(ai);
	return i;
}
