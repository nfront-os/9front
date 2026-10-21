#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/error.h"

#include <tos.h>
#include "ureg.h"
#include "riscv.h"

/* sstatus bits the user can't change (setregisters, noted) */
#define USTATUSMASK	(Spp|Sie|Spie|Sum|Mxr|Fsst)

static void faultriscv(Ureg*);

void
trapinit(void)
{
	extern void strap(void);

	setsie(0);
	/* user mode may read the time and cycle counters (libc's cycles) */
	setscounteren(7);
	setstvec((uintptr)strap);
	setsie(Seie|Stie|Ssie);
}

static char *excnames[16] = {
	[Instaddralign]		"misaligned instruction address",
	[Instaccess]		"instruction access fault",
	[Illinst]		"illegal instruction",
	[Breakpt]		"breakpoint",
	[Loadaddralign]		"misaligned load address",
	[Loadaccess]		"load access fault",
	[Storeaddralign]	"misaligned store address",
	[Storeaccess]		"store access fault",
	[Envcalluser]		"system call",
	[Envcallsup]		"supervisor ecall",
	[Instpage]		"instruction page fault",
	[Loadpage]		"load page fault",
	[Storepage]		"store page fault",
};

static char*
excname(uvlong cause)
{
	if(cause < nelem(excnames) && excnames[cause] != nil)
		return excnames[cause];
	return "unknown exception";
}

void
trap(Ureg *ureg)
{
	uvlong cause;
	int user, isclock, math;
	char buf[ERRMAX];

	cause = ureg->cause;
	if(cause == Envcalluser){
		syscall(ureg);
		return;
	}
	user = kenter(ureg);
	math = 0;
	if(cause & Rv64intr){
		fpukenter(ureg);
		isclock = 0;
		switch(cause & ~Rv64intr){
		case Suptmrintr:
			isclock = 1;
			m->intr++;
			clockintr(ureg);
			break;
		case Supextintr:
			irq(ureg);
			break;
		case Supswintr:
			m->intr++;
			setsip(getsip() & ~Ssie);
			break;
		default:
			iprint("cpu%d: unknown interrupt %#llux\n", m->machno, cause);
			break;
		}
		preempted(isclock);
	} else switch(cause){
	case Instpage:
	case Loadpage:
	case Storepage:
	case Instaccess:
	case Loadaccess:
	case Storeaccess:
		fpukenter(ureg);
		faultriscv(ureg);
		break;
	case Illinst:
		/* the fpu is off (fpoff): first use, or after a switch */
		if((ureg->status & Fsst) == Fsoff){
			math = 1;
			mathtrap(ureg);
			break;
		}
		/* wet floor */
	default:
		fpukenter(ureg);
		if(!user){
			dumpregs(ureg);
			panic("kernel %s at pc %#p", excname(cause), ureg->pc);
		}
		if(cause == Breakpt)
			snprint(buf, sizeof buf, "sys: breakpoint");
		else
			snprint(buf, sizeof buf, "sys: trap: %s", excname(cause));
		postnote(up, 1, buf, NDebug);
		break;
	}

	splhi();
	if(user){
		if(up->procctl || up->nnote)
			donotify(ureg);
		kexit(ureg);
	}
	if(!math)
		fpukexit(ureg);
}

void
syscall(Ureg *ureg)
{
	ulong scallnr;

	if(!kenter(ureg))
		panic("syscall from kernel");
	fpukenter(ureg);

	ureg->pc += 4;		/* past the ecall */
	scallnr = ureg->ret;
	if(dosyscall(scallnr, (Sargs*)(ureg->sp+BY2WD), &ureg->ret))
		return;		/* noted changed the ureg */

	if(up->procctl || up->nnote)
		donotify(ureg);

	if(up->delaysched)
		sched();

	kexit(ureg);
	fpukexit(ureg);
}

/*
 * the note handler is called as notify(ureg, msg) with jc's
 * conventions: ureg in R8 and at 8(sp), msg at 16(sp)
 */
Ureg*
notify(Ureg *ureg, char *msg)
{
	Ureg *nureg;
	uintptr sp;

	sp = ureg->sp;
	sp -= 256;	/* debugging: preserve context causing problem */
	sp -= sizeof(Ureg);
	sp = STACKALIGN(sp);

	if(!okaddr(sp-ERRMAX-4*BY2WD, sizeof(Ureg)+ERRMAX+4*BY2WD, 1)
	|| ((uintptr) up->notify & 1) != 0
	|| (sp & 7) != 0)
		return nil;

	nureg = (Ureg*)sp;
	memmove(nureg, ureg, sizeof(Ureg));
	sp -= BY2WD+ERRMAX;
	memmove((char*)sp, msg, ERRMAX);
	sp -= 3*BY2WD;
	*(uintptr*)(sp+2*BY2WD) = sp+3*BY2WD;
	*(uintptr*)(sp+1*BY2WD) = (uintptr)nureg;
	ureg->ret = (uintptr)nureg;
	ureg->sp = sp;
	ureg->pc = (uintptr)up->notify;
	ureg->link = 0;

	return nureg;
}

int
noted(Ureg *ureg, Ureg *nureg, int arg0)
{
	uintptr oureg, sp;

	oureg = (uintptr) nureg;
	if((oureg & 7) != 0)
		return -1;

	setregisters(ureg, (char*)ureg, (char*)nureg, sizeof(Ureg));

	switch(arg0){
	case NCONT: case NRSTR:
		if(!okaddr(ureg->pc, BY2WD, 0)
		|| !okaddr(ureg->sp, BY2WD, 0)
		|| (ureg->pc & 1) != 0 || (ureg->sp & 7) != 0)
			return -1;
		break;
	case NSAVE:
		sp = oureg - 4 * BY2WD - ERRMAX;
		if(!okaddr(ureg->pc, BY2WD, 0)
		|| !okaddr(sp, 4 * BY2WD, 1)
		|| (nureg->pc & 1) != 0 || (sp & 7) != 0)
			return -1;
		ureg->sp = sp;
		ureg->ret = (uintptr) oureg;
		((uintptr *) sp)[1] = oureg;
		((uintptr *) sp)[0] = 0;
		break;
	}
	return 0;
}

static void
faultriscv(Ureg *ureg)
{
	int user, read;
	uintptr addr;

	user = userureg(ureg);
	if(!user){
		if(ureg->pc >= (uintptr)peek && ureg->pc < (uintptr)_peekend){
			ureg->pc = ureg->link;
			return;
		}
		/* before the first process nothing can catch it (waserror needs up) */
		if(up == nil)
			panic("kernel fault: cause %llud addr %#p pc %#p ra %#p",
				ureg->cause, ureg->tval, ureg->pc, ureg->link);
		if(waserror()){
			if(up->nerrlab == 0){
				pprint("suicide: sys: %s\n", up->errstr);
				pexit(up->errstr, 1);
			}
			/* skipping bottom of trap(), so do it outselfs */
			splhi();
			fpukexit(ureg);
			spllo();
			nexterror();
		}
	}

	addr = ureg->tval;
	read = ureg->cause != Storepage && ureg->cause != Storeaccess;

	switch(ureg->cause){
	case Instpage:
	case Loadpage:
	case Storepage:
		if(up != nil && fault(addr, ureg->pc, read) == 0)
			break;
		/* wet floor */
	default:
		if(!user){
			dumpregs(ureg);
			panic("kernel fault: %s addr=%#p", read? "read": "write", addr);
		}
		faultnote("fault", read? "read": "write", addr);
	}

	if(!user)
		poperror();
}

int
userureg(Ureg* ureg)
{
	return (ureg->status & Spp) == 0;
}

uintptr
userpc(void)
{
	return up->dbgreg->pc;
}

uintptr
dbgpc(Proc *)
{
	Ureg *ur = up->dbgreg;
	if(ur == nil)
		return 0;
	return ur->pc;
}

void
procfork(Proc *p)
{
	fpuprocfork(p);
}

void
procsetup(Proc *p)
{
	fpuprocsetup(p);
}

void
procsave(Proc *p)
{
	fpuprocsave(p);
	putasid(p);	// release asid
}

void
procrestore(Proc *p)
{
	fpuprocrestore(p);
}

void
kprocchild(Proc *p, void (*entry)(void))
{
	p->sched.pc = (uintptr) entry;
	p->sched.sp = STACKALIGN((uintptr)p) - 16;
	*(void**)p->sched.sp = kprocchild;	/* fake */
}

void
forkchild(Proc *p, Ureg *ureg)
{
	Ureg *cureg;

	p->sched.pc = (uintptr) forkret;
	p->sched.sp = STACKALIGN((uintptr)p) - TRAPFRAME;

	cureg = (Ureg*) (p->sched.sp + UREG);
	memmove(cureg, ureg, sizeof(Ureg));
	cureg->ret = 0;
}

uintptr
execregs(uintptr entry, int argc, char *argv[], Tos *tos)
{
	uintptr *sp = (void*)argv;
	Ureg *ureg;

	*--sp = argc;

	ureg = up->dbgreg;
	ureg->sp = (uintptr)sp;
	ureg->pc = entry;
	ureg->link = 0;

	return (uintptr)tos;
}

void
evenaddr(uintptr addr)
{
	if(addr & 3){
		postnote(up, 1, "sys: odd address", NDebug);
		error(Ebadarg);
	}
}

void
callwithureg(void (*f) (Ureg *))
{
	Ureg u;

	u.pc = getcallerpc(&f);
	u.sp = (uintptr) &f;
	u.link = 0;
	f(&u);
}

void
setkernur(Ureg *ureg, Proc *p)
{
	ureg->pc = p->sched.pc;
	ureg->sp = p->sched.sp;
	ureg->link = (uintptr)sched;
}

void
setupwatchpts(Proc*, Watchpt*, int)
{
}

void
setregisters(Ureg* ureg, char* pureg, char* uva, int n)
{
	uvlong v;

	v = ureg->status;
	memmove(pureg, uva, n);
	ureg->status = (ureg->status & ~USTATUSMASK) | (v & USTATUSMASK);
}

static void
dumpstackwithureg(Ureg *ureg)
{
	uintptr v, estack, sp;
	char *s;
	int i;

	if((s = getconf("*nodumpstack")) != nil && strcmp(s, "0") != 0){
		iprint("dumpstack disabled\n");
		return;
	}
	iprint("ktrace /kernel/path %#p %#p %#p # pc, sp, link\n",
		ureg->pc, ureg->sp, ureg->link);
	delay(2000);

	sp = ureg->sp;
	if(sp < KZERO || (sp & 7) != 0)
		sp = (uintptr)&ureg;

	estack = (uintptr)m+MACHSIZE;
	if(up != nil && sp <= (uintptr)up)
		estack = (uintptr)up;

	if(sp > estack){
		if(up != nil)
			iprint("&up %#p sp %#p\n", up, sp);
		else
			iprint("&m %#p sp %#p\n", m, sp);
		return;
	}

	i = 0;
	for(; sp < estack; sp += sizeof(uintptr)){
		v = *(uintptr*)sp;
		if(KTZERO < v && v < (uintptr)etext && (v & 1) == 0){
			iprint("%#8.8lux=%#8.8lux ", (ulong)sp, (ulong)v);
			i++;
		}
		if(i == 4){
			i = 0;
			iprint("\n");
		}
	}
	if(i)
		iprint("\n");
}

void
dumpstack(void)
{
	callwithureg(dumpstackwithureg);
}

void
dumpregs(Ureg *ureg)
{
	uintptr *r;
	int i, x;

	x = splhi();
	if(up != nil)
		iprint("cpu%d: dumpregs ureg %#p process %lud: %s\n", m->machno, ureg,
			up->pid, up->text);
	else
		iprint("cpu%d: dumpregs ureg %#p\n", m->machno, ureg);
	r = ureg->regs;
	for(i = 1; i < 32; i += 3)
		if(i+2 < 32)
			iprint("R%d %.16llux  R%d %.16llux  R%d %.16llux\n",
				i, r[i], i+1, r[i+1], i+2, r[i+2]);
		else
			iprint("R%d %.16llux\n", i, r[i]);
	iprint("PC %#p  STATUS %#llux  CAUSE %#llux  TVAL %#p\n",
		ureg->pc, (uvlong)ureg->status, (uvlong)ureg->cause, ureg->tval);
	splx(x);
}
