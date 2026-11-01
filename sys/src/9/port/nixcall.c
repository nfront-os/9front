/*
 * NIX: system calls through a queue (nixcall(2)), after NIX's
 * port/nixcall.c (Lsub, Bell Labs; MIT license, see /lib/legal/nix).
 *
 * A process queues system calls in the Callq of its Tos; a kernel
 * process (nixkproc) polls the queue, makes the calls for it and
 * queues the replies. A process on an application core can so do
 * I/O without a round trip to a time-sharing core per call.
 *
 * Unlike NIX's prototype, which answered every call with -scall,
 * the kernel process makes the calls: with the user, name space,
 * file descriptors, environment, dot and segments of the process
 * (a kernel process is eve otherwise: the calls must not get eve's
 * rights). The calls that make sense from another process are
 * allowed (allowed()); they run one after another, so one that
 * blocks holds up the ones behind it, as NIX's TODO says. Before
 * each batch of calls the kernel process takes the process' segments
 * again (nixsyncseg), so it sees those attached or detached since.
 */
#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"
#include	"ureg.h"
#include	"tos.h"
#include	"../../libc/9syscall/sys.h"

struct Nixpctl
{
	Ref;
	int	cmd;
	int	kick;		/* nixsyscall() since the last look */
	Rendez	kwait;		/* the kernel process waits for calls */
	Rendez	uwait;		/* the process waits for the kernel process */
	Ureg	ureg;		/* up->dbgreg of the kernel process: zero */
	Proc	*p;
	ulong	pid;		/* p's: p may be gone and its Proc reused */
	Proc	*kproc;
	Tos	*tos;
};

enum
{
	NCnone = 0,
	NCinit,
	NCdie,
	NCdone = ~0,

	Npoll = 1000,		/* yields before sleeping */
};

#define	NEXT(i)	(((i)+1)%CALLQSZ)

static int
allowed(int scall)
{
	switch(scall){
	case OPEN:
	case CREATE:
	case CLOSE:
	case PREAD:
	case PWRITE:
	case SEEK:
	case STAT:
	case FSTAT:
	case WSTAT:
	case FWSTAT:
	case REMOVE:
	case DUP:
	case FD2PATH:
	case _NSEC:
		return 1;
	}
	return 0;
}

static void
putpctl(Nixpctl *pctl)
{
	if(decref(&pctl->Ref) == 0)
		free(pctl);
}

static int
cmddone(void *a)
{
	return ((Nixpctl*)a)->cmd == NCdone || ((Nixpctl*)a)->cmd == NCdie;
}

static int
kicked(void *a)
{
	Nixpctl *pctl;

	pctl = a;
	return pctl->kick || pctl->cmd == NCdie;
}

/* the queue is in user memory: no locks held when looking at it */
static int
work(Nixpctl *pctl)
{
	volatile Callq *q;

	q = &pctl->tos->callq;
	return pctl->cmd == NCdie || q->qr != q->qw;
}

/* make the call nc, reply in nr */
static void
nixsyscall(Nixcall *nc, Nixret *nr)
{
	uintptr ret;
	int scall;
	void *tag;

	scall = nc->scall;
	tag = nc->tag;
	if(!allowed(scall)){
		ret = -1;
		kstrcpy(up->syserrstr, "system call not allowed in a call queue", ERRMAX);
	} else
		dosyscall(scall, (Sargs*)nc->arg, &ret);
	nr->tag = tag;
	nr->sret = (vlong)(intptr)ret;
	if(ret == (uintptr)-1)
		kstrcpy(nr->err, up->syserrstr, sizeof nr->err);
	else
		nr->err[0] = 0;
}

/*
 * up (the kernel process) gets p's segments, those p attached or
 * detached since the last look too. p's seglock first: p (pexit,
 * segattach, segdetach) takes only its own.
 */
static void
nixsyncseg(Proc *p, ulong pid)
{
	Segment *s, *o;
	int i, changed;

	changed = 0;
	qlock(&p->seglock);
	if(p->pid != pid){
		qunlock(&p->seglock);
		return;
	}
	qlock(&up->seglock);
	for(i = 0; i < NSEG; i++){
		if((s = p->seg[i]) == up->seg[i])
			continue;
		if(s != nil)
			incref(&s->Ref);
		o = up->seg[i];
		up->seg[i] = s;
		if(o != nil)
			putseg(o);
		changed = 1;
	}
	qunlock(&up->seglock);
	qunlock(&p->seglock);
	if(changed)
		flushmmu();
}

/* take the process' resources: its rights, not eve's */
static void
nixshare(Proc *p)
{
	Chan *c;

	kstrdup(&up->user, p->user);
	closepgrp(up->pgrp);
	up->pgrp = p->pgrp;
	incref(&up->pgrp->Ref);
	up->fgrp = p->fgrp;
	incref(&up->fgrp->Ref);
	up->egrp = p->egrp;
	incref(&up->egrp->Ref);
	c = up->dot;
	up->dot = p->dot;
	incref(&up->dot->Ref);
	if(c != nil)
		cclose(c);
	nixsyncseg(p, p->pid);
}

static void
nixkproc(void *a)
{
	Nixpctl *pctl;
	volatile Callq *q;
	int i;

	pctl = a;
	/*
	 * a bad address in a call: validaddr prints the user's pc
	 * (userpc: up->dbgreg, nil in a kernel process)
	 */
	up->dbgreg = &pctl->ureg;
	nixshare(pctl->p);
	q = &pctl->tos->callq;
	pctl->kproc = up;
	pctl->cmd = NCdone;
	wakeup(&pctl->uwait);
	for(;;){
		for(i = 0; i < Npoll && !work(pctl); i++)
			yield();
		while(!work(pctl)){
			q->ksleep = 1;
			coherence();
			if(work(pctl))
				break;
			if(waserror())
				goto Done;	/* a note: the process is gone */
			sleep(&pctl->kwait, kicked, pctl);
			poperror();
			pctl->kick = 0;
		}
		q->ksleep = 0;
		coherence();
		if(pctl->cmd == NCdie || up->procctl == Proc_exitme)
			break;
		nixsyncseg(pctl->p, pctl->pid);
		while(q->qr != q->qw){
			coherence();
			nixsyscall((Nixcall*)&q->q[q->qr], (Nixret*)&q->r[q->rw]);
			coherence();
			/*
			 * validaddr posts "sys: bad address" to the caller,
			 * here us: the call failed, the next ones go on
			 */
			if(pctl->cmd != NCdie){
				up->nnote = 0;
				up->notepending = 0;
			}
			q->qr = NEXT(q->qr);
			q->rw = NEXT(q->rw);
			if(pctl->cmd == NCdie)
				break;
		}
	}
Done:
	putpctl(pctl);
	pexit("", 1);
}

/*
 * nixsyscall(): start the kernel process of up, or wake it.
 */
uintptr
sysnixsyscall(uchar*)
{
	Nixpctl *pctl;

	if((pctl = up->nixpctl) == nil){
		pctl = smalloc(sizeof(Nixpctl));
		memset(pctl, 0, sizeof(Nixpctl));
		pctl->Ref.ref = 2;	/* up and the kernel process */
		pctl->cmd = NCinit;
		pctl->p = up;
		pctl->pid = up->pid;
		pctl->tos = (Tos*)(USTKTOP-sizeof(Tos));
		up->nixpctl = pctl;
		kproc("nixcall", nixkproc, pctl);
		while(!cmddone(pctl))
			sleep(&pctl->uwait, cmddone, pctl);
		return 0;
	}
	pctl->kick = 1;
	wakeup(&pctl->kwait);
	return 0;
}

/*
 * From pexit and exec: no more kernel process for up.
 */
void
stopnixproc(void)
{
	Nixpctl *pctl;

	if((pctl = up->nixpctl) == nil)
		return;
	up->nixpctl = nil;
	pctl->cmd = NCdie;
	wakeup(&pctl->kwait);
	if(pctl->kproc != nil)
		postnote(pctl->kproc, 1, "nixcall: exit", NUser);	/* out of a blocking call */
	putpctl(pctl);
}
