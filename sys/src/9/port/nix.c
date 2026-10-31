/*
 * NIX application cores (ACs), after the NIX kernel
 * (Lsub, Bell Labs, Sandia, Vita Nuova; k8/acore.c, k8/tcore.c;
 * MIT license, see /lib/legal/nix).
 *
 * The boot option *nixac=n makes the last n processors ACs, *nixkc=k
 * the k before them kernel cores (KCs: they run only kprocs and the
 * TC halves of AC processes). /dev/nixctl changes roles at run time.
 * An AC never runs the scheduler, takes no clock and no device
 * interrupts, and runs one user process at a time: the one that
 * execac(2) or the ctl message "core n" (proc(3)) moved there.
 *
 * The process keeps its kernel half on a time-sharing core (TC):
 * runacore() hands the user registers to the AC through the AC's
 * mailbox (Icc) and sleeps. When the process makes a system call,
 * faults or gets an IPI, the AC saves the registers back into the
 * process (acexit), wakes it and waits for the next call. runacore()
 * then serves the call or the trap on the TC with the usual code
 * (acservice) and sends the process back, until it leaves the AC.
 *
 * The process' pages are faulted in before it goes to the AC
 * (nixprepage, as NIX does), so it rarely comes back for a fault.
 *
 * A note for the process interrupts runacore()'s sleep; it then
 * sends the AC an IPI, which brings the process back to the TC to
 * take the note. procflushmmu() does the same when the process'
 * page tables change.
 *
 * The architecture provides (fns.h): acarchinit, acidle, acipi,
 * acmmuswitch, acfpuprep, acfpusave, acfpurestore, actouser and
 * acservice.
 */
#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"
#include	"ureg.h"

int	nixnac;			/* number of ACs */
static	uchar	nixrole[MAXMACH];
static	uchar	nixkcboot[MAXMACH];
static	Lock	nixlock;
static	QLock	nixctllock;

/*
 * Called by the bootstrap processor before it starts the others,
 * ncpu of them in all: machno ncpu-nac ... ncpu-1 become ACs.
 * One TC at least.
 */
int
nixboot(int ncpu)
{
	char *s;
	int i, n, k;

	if((s = getconf("*nixac")) == nil)
		return 0;
	n = strtol(s, nil, 0);
	if(n > ncpu-1)
		n = ncpu-1;
	if(n < 0)
		n = 0;
	for(i = ncpu-n; i < ncpu; i++)
		nixrole[i] = NIXAC;
	nixnac = n;
	if((s = getconf("*nixkc")) != nil){
		k = strtol(s, nil, 0);
		for(i = ncpu-n-1; i >= 1 && k > 0; i--, k--)
			nixkcboot[i] = 1;
	}
	return n;
}

/*
 * After the processors started: the kernel cores of *nixkc.
 */
void
nixbootkc(void)
{
	int i;

	for(i = 1; i < conf.nmach; i++)
		if(nixkcboot[i] && MACHP(i) != nil && !isac(i)){
			MACHP(i)->PMach.nixtype = NIXKC;
			print("cpu%d: kernel core\n", i);
		}
}

int
isac(int machno)
{
	return machno >= 0 && machno < MAXMACH && nixrole[machno] == NIXAC;
}

/*
 * The AC's loop, at the bottom of its stack: run what the TC sends.
 * fn does not return when it goes to user mode; the next entry from
 * user mode (acexit) comes back here on a fresh stack.
 */
_Noreturn void
acsched(void)
{
	Icc *icc;
	void (*fn)(void);

	icc = m->PMach.icc;
	for(;;){
		splhi();
		if((fn = icc->fn) == nil){
			spllo();
			acidle();
			continue;
		}
		icc->fn = nil;
		coherence();
		fn();
	}
}

/*
 * The application processor machno becomes an AC
 * instead of entering the scheduler.
 */
static Icc*
iccalloc(void)
{
	Icc *icc;

	icc = mallocalign(sizeof(Icc), 64, 0, 0);
	if(icc == nil)
		panic("iccalloc: no memory");
	memset(icc, 0, sizeof(Icc));
	return icc;
}

_Noreturn void
acinit(void)
{
	if(m->PMach.icc == nil)
		m->PMach.icc = iccalloc();
	m->PMach.nixtype = NIXAC;
	acarchinit();
	iprint("cpu%d: application core\n", m->machno);
	acsched();
}

/*
 * On the AC: run the process in icc->proc.
 */
static void
acrun(void)
{
	Icc *icc;
	Proc *p;

	icc = m->PMach.icc;
	p = icc->proc;
	icc->busy = 1;
	acmmuswitch(p);
	acfpurestore(p);
	icc->running = p;
	coherence();
	actouser(p->acureg);
}

/*
 * On the AC: the process left user mode (system call, trap or IPI).
 * Give its registers back and wake its TC half.
 */
_Noreturn void
acexit(Ureg *ureg, int rc, uintptr addr)
{
	Icc *icc;
	Proc *p;
	int tc;

	icc = m->PMach.icc;
	p = icc->running;
	if(p == nil){
		iprint("cpu%d: acexit: no process (rc %d)\n", m->machno, rc);
		acsched();
	}
	icc->running = nil;
	icc->nuser++;
	memmove(p->acureg, ureg, sizeof(Ureg));
	acfpusave(p);
	acmmuswitch(nil);
	p->acrc = rc;
	p->acaddr = addr;
	p->nicc++;
	if(rc == ICCSYSCALL)
		p->nacsyscall++;
	else
		p->nactrap++;
	tc = p->actc;
	coherence();
	p->acdone = 1;
	wakeup(&p->acr);
	coherence();
	icc->busy = 0;			/* p is the TC's again */
	acipi(MACHP(tc));		/* it may sleep in halt */
	acsched();
}

/*
 * Fault in all pages of s from from up (prepaging). Text and
 * read-only segments by reading, the others by writing, so a
 * copy-on-write page is copied now, not on the AC.
 */
static void
prepageseg(Segment *s, uintptr from)
{
	uintptr addr;
	int read;

	read = (s->type&SG_TYPE) == SG_TEXT || (s->type&SG_RONLY) != 0;
	for(addr = from; addr < s->top; addr += BY2PG)
		if(fault(addr, 0, read) < 0)
			break;
}

/*
 * NIX: prepage up's segments, as NIX does before a process goes to
 * an AC. NIX's 2M pages made that cheap for the whole stack; here
 * the stack is prepaged only from the user's sp up, what it uses.
 */
void
nixprepage(uintptr sp)
{
	Segment *s;
	uintptr from;
	int i;

	for(i = 0; i < NSEG; i++){
		if((s = up->seg[i]) == nil)
			continue;
		from = s->base;
		if((s->type&SG_TYPE) == SG_STACK){
			from = sp & ~(BY2PG-1);
			if(from < s->base || from >= s->top)
				from = s->top - BY2PG;
		}
		prepageseg(s, from);
	}
}

/* NIX: prepage the segment at va, just attached */
void
nixprepageva(uintptr va)
{
	Segment *s;

	if((s = seg(up, va, 0)) != nil)
		prepageseg(s, s->base);
}

static int
acdonef(void *a)
{
	return ((Proc*)a)->acdone;
}

/*
 * On the TC, from donotify(): run up on its AC until it comes back
 * to the TC (execac with core 0, ctl "core 0") or exits.
 * Returns 1 when ureg has to be restored in full.
 */
int
runacore(Ureg *ureg)
{
	Mach *ac;
	Icc *icc;

	up->procctl = 0;
	if(up->acrunning)
		return 0;	/* execac on the AC: the loop below goes on */
	up->acrunning = 1;
	spllo();
	if(up->prepagemem)
		nixprepage(ureg->sp);
	/*
	 * the Tos (pid, cycles) as at the end of a system call: the call
	 * that brought us here ends only when we come back to the TC.
	 * acservice does it for each round trip after this one.
	 */
	kexit(ureg);
	while((ac = up->ac) != nil){
		if(up->acback){
			up->acback = 0;
			putac(up);
			break;
		}
		icc = ac->PMach.icc;
		acfpuprep(up);
		up->acureg = ureg;
		up->acdone = 0;
		up->actc = m->machno;
		icc->proc = up;
		coherence();
		icc->fn = acrun;
		while(!up->acdone){
			if(waserror()){
				/* a note: get the process off the AC */
				acipi(ac);
				continue;
			}
			sleep(&up->acr, acdonef, up);
			poperror();
		}
		while(icc->busy)
			coherence();
		splhi();
		acservice(ureg, up->acrc);
		spllo();
	}
	up->acrunning = 0;
	/* back to user mode, but not on a kernel core */
	while(m->PMach.nixtype == NIXKC)
		sched();
	splhi();
	return 1;
}

/*
 * Reserve an AC for p: the one numbered core, or any free one
 * for core < 0.
 */
Mach*
getac(Proc *p, int core)
{
	Mach *mp;
	int i;

	if(nixnac == 0)
		error("no application cores");
	lock(&nixlock);
	if(core > 0){
		if(core >= conf.nmach || !isac(core)){
			unlock(&nixlock);
			error("not an application core");
		}
		mp = MACHP(core);
		if(mp->PMach.icc == nil || mp->PMach.acowner != nil && mp->PMach.acowner != p){
			unlock(&nixlock);
			error("application core busy");
		}
	} else {
		mp = nil;
		for(i = 0; i < conf.nmach; i++){
			if(!isac(i))
				continue;
			mp = MACHP(i);
			if(mp->PMach.icc != nil && (mp->PMach.acowner == nil || mp->PMach.acowner == p))
				break;
			mp = nil;
		}
		if(mp == nil){
			unlock(&nixlock);
			error("no free application core");
		}
	}
	mp->PMach.acowner = p;
	unlock(&nixlock);
	return mp;
}

/*
 * Release the ACs reserved for p, but keep keep (nil: all of them).
 */
void
putacs(Proc *p, Mach *keep)
{
	Mach *mp;
	int i;

	if(nixnac == 0)
		return;
	lock(&nixlock);
	for(i = 0; i < conf.nmach; i++){
		mp = MACHP(i);
		if(mp != nil && mp != keep && mp->PMach.acowner == p)
			mp->PMach.acowner = nil;
	}
	unlock(&nixlock);
}

void
putac(Proc *p)
{
	putacs(p, nil);
	p->ac = nil;
}

/*
 * devproc's ctl message "core n" for p (qlock'ed p->debug).
 */
void
procctlcore(Proc *p, int core)
{
	if(p->kp)
		error(Eperm);
	if(core == 0){
		if(p->ac != nil){
			p->acback = 1;
			acipi(p->ac);
		}
		return;
	}
	if(p->ac != nil)
		error("already on an application core");
	if(p->procctl != 0)
		error(Einuse);
	p->ac = getac(p, core);
	p->prepagemem = 1;	/* in runacore, as p */
	p->procctl = Proc_toac;
}

/*
 * From procflushmmu(): p's page tables change. Bring p off
 * its AC if it runs there; returns the AC to wait for (acflushed).
 */
Mach*
acflush(Proc *p, ulong *nuser)
{
	Mach *mp;
	Icc *icc;

	if(nixnac == 0 || (mp = p->ac) == nil)
		return nil;
	icc = mp->PMach.icc;
	if(icc->running != p)
		return nil;
	*nuser = icc->nuser;
	acipi(mp);
	return mp;
}

int
acflushed(Mach *mp, Proc *p, ulong nuser)
{
	Icc *icc;

	icc = mp->PMach.icc;
	return icc->running != p || icc->nuser != nuser;
}

/*
 * Roles at run time (/dev/nixctl). Processor 0 keeps its role:
 * it takes the clock duties (checkalarms) and, on arm64, all
 * device interrupts.
 */
/*
 * Processes wired to processor n (aux/timesync wires itself) go to
 * processor 0, which keeps its role.
 */
static void
nixunwire(int n)
{
	Proc *p;
	int i;

	for(i = 0; (p = proctab(i)) != nil; i++)
		if(p != up && p->state > New && p->wired && p->affinity == n){
			p->affinity = 0;
			print("cpu%d: process %lud %s now wired to cpu0\n", n, p->pid, p->text);
		}
}

/* on the AC: become a time-sharing core */
static void
actotc(void)
{
	nixrole[m->machno] = NIXTC;
	nixnac--;
	acarchexit();
	m->PMach.nixtype = NIXTC;
	m->PMach.ticks = MACHP(0)->PMach.ticks;
	active.machs[m->machno] = 1;
	timersinit();
	iprint("cpu%d: time-sharing core\n", m->machno);
	schedinit();
}

static void
settc(int n)
{
	Mach *mp;
	Icc *icc;

	mp = MACHP(n);
	if(!isac(n)){
		mp->PMach.nixtype = NIXTC;
		return;
	}
	lock(&nixlock);
	if(mp->PMach.acowner != nil){
		unlock(&nixlock);
		error("application core busy");
	}
	mp->PMach.acowner = up;	/* nobody gets it meanwhile */
	unlock(&nixlock);
	icc = mp->PMach.icc;
	coherence();
	icc->fn = actotc;
	while(mp->PMach.nixtype == NIXAC)
		tsleep(&up->sleep, return0, nil, 10);
	lock(&nixlock);
	mp->PMach.acowner = nil;
	unlock(&nixlock);
}

static void
setac(int n)
{
	Mach *mp;
	Icc *icc;
	int owired, oaff, s;

	if(isac(n))
		return;
	mp = MACHP(n);
	nixunwire(n);
	if(mp->PMach.icc == nil)
		mp->PMach.icc = iccalloc();
	icc = mp->PMach.icc;
	memset(icc, 0, sizeof(Icc));
	mp->PMach.nixtype = NIXTC;	/* a KC runs no user process: us */

	/* go there; it makes itself an AC */
	owired = up->wired;
	oaff = up->affinity;
	up->wired = 1;
	up->affinity = n;
	while(m->machno != n)
		sched();
	s = splhi();
	nixrole[n] = NIXAC;
	nixnac++;
	m->PMach.nixtype = NIXAC;
	m->PMach.readied = nil;
	active.machs[n] = 0;
	acarchinit();
	timersmove();
	intrmove(n);
	up->wired = owired;
	up->affinity = owired? oaff: -1;
	iprint("cpu%d: application core\n", n);
	sched();	/* schedinit on n goes to acsched */
	splx(s);
}

long
nixctlread(void *a, long n, vlong off)
{
	char *buf, *p, *e, *role;
	Mach *mp;
	Proc *o;
	int i;

	buf = smalloc(READSTR);
	p = buf;
	e = buf + READSTR;
	for(i = 0; i < conf.nmach; i++){
		if((mp = MACHP(i)) == nil)
			continue;
		if(isac(i))
			role = "AC";
		else if(mp->PMach.nixtype == NIXKC)
			role = "KC";
		else
			role = "TC";
		o = mp->PMach.acowner;
		p = seprint(p, e, "%d %s %lud\n", i, role, o != nil? o->pid: 0);
	}
	n = readstr(off, a, n, buf);
	free(buf);
	return n;
}

/*
 * "ac n", "tc n", "kc n": processor n becomes an application,
 * time-sharing or kernel core.
 */
void
nixctlwrite(char *a, long n)
{
	Cmdbuf *cb;
	int c;

	cb = parsecmd(a, n);
	if(waserror()){
		free(cb);
		nexterror();
	}
	if(cb->nf != 2)
		error(Ebadarg);
	c = atoi(cb->f[1]);
	if(c < 1 || c >= conf.nmach || MACHP(c) == nil)
		error("bad processor (not 0, which keeps its role)");
	eqlock(&nixctllock);
	if(waserror()){
		qunlock(&nixctllock);
		nexterror();
	}
	if(strcmp(cb->f[0], "ac") == 0)
		setac(c);
	else if(strcmp(cb->f[0], "tc") == 0)
		settc(c);
	else if(strcmp(cb->f[0], "kc") == 0){
		settc(c);
		nixunwire(c);
		MACHP(c)->PMach.nixtype = NIXKC;
	} else
		error(Ebadctl);
	qunlock(&nixctllock);
	poperror();
	free(cb);
	poperror();
}
