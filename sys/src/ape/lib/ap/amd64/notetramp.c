#ifdef __GNUC_MINOR__	/* GCC port: gcc (APE is compiled -U__GNUC__); 9front's notetramp.c for kencc below */
#include "../plan9/lib.h"
#include "../plan9/sys9.h"
#include <signal.h>
#include <setjmp.h>

/*
 * A stack to hold pcs when signals nest.  9front-ng: one per proc: the
 * threads of a program (pthreads, procs sharing memory: ap/gnu/pthread.c)
 * take notes at once (a page fault each in two of them) and broke one
 * shared stack.  A proc's stack is a slot it claims by its pid while a
 * handler of its runs, and gives back after the last one.
 */
#define MAXSIGSTACK 20
#define MAXSIGPROC 64
typedef long long vlong;
typedef unsigned int ulong;
typedef unsigned long long uvlong;
#include "../../../../../include/tos.h"

typedef struct Pcstack Pcstack;
struct Pcstack {
	int sig;
	void (*hdlr)(int, char*, Ureg*);
	unsigned long long restorepc;
	Ureg *u;
};
static struct Sigproc {
	int pid;
	int nstack;
	Pcstack pcstack[MAXSIGSTACK];
} sigprocs[MAXSIGPROC];

/* this proc's stack, claimed if create (nil if all are taken, or none and !create) */
static struct Sigproc*
sigproc(int create)
{
	struct Sigproc *sp;
	int pid, i;

	pid = _tos->pid;
	for(i = 0; i < MAXSIGPROC; i++)
		if(sigprocs[i].pid == pid)
			return &sigprocs[i];
	if(!create)
		return 0;
	for(i = 0; i < MAXSIGPROC; i++){
		sp = &sigprocs[i];
		if(sp->pid == 0 && __sync_bool_compare_and_swap(&sp->pid, 0, pid)){
			sp->nstack = 0;
			return sp;
		}
	}
	return 0;
}

/* the proc's last handler is through: the slot is free */
static void
sigdone(struct Sigproc *sp)
{
	if(sp->nstack == 0)
		__sync_lock_release(&sp->pid);
}

void notecont(Ureg*, char*);	/* GCC port: was static, amd64/notify.S's _notecont calls it */
extern void _notecont(void);	/* GCC port: System V trampoline to notecont, notify.S */

void
_notetramp(int sig, void (*hdlr)(int, char*, Ureg*), Ureg *u)
{
	struct Sigproc *sp;
	Pcstack *p;

	sp = sigproc(1);
	if(sp == 0 || sp->nstack >= MAXSIGSTACK)
		_NOTED(1);	/* nesting too deep (or too many procs in handlers); just do system default */
	p = &sp->pcstack[sp->nstack];
	p->restorepc = u->pc;
	p->sig = sig;
	p->hdlr = hdlr;
	p->u = u;
	sp->nstack++;
	u->pc = (unsigned long long) _notecont;	/* GCC port: was notecont, entered the 6c way */
	_NOTED(2);	/* NSAVE: clear note but hold state */
}

void	/* GCC port: was static */
notecont(Ureg *u, char *s)
{
	struct Sigproc *sp;
	Pcstack *p;
	void(*f)(int, char*, Ureg*);
	int sig;

	sp = sigproc(0);
	p = &sp->pcstack[sp->nstack-1];
	f = p->hdlr;
	sig = p->sig;
	u->pc = p->restorepc;
	sp->nstack--;
	sigdone(sp);
	(*f)(sig, s, u);
	_NOTED(3);	/* NRSTR */
}

#define JMPBUFPC 1
#define JMPBUFSP 0

extern sigset_t	_psigblocked;

typedef struct {
	sigset_t set;
	sigset_t blocked;
	unsigned long long jmpbuf[8];	/* GCC port: was [2], setjmp.s saves bp, bx, r12-r15 too */
} sigjmp_buf_amd64;

void
siglongjmp(sigjmp_buf j, int ret)
{
	struct Ureg *u;
	sigjmp_buf_amd64 *jb;
	struct Sigproc *sp;

	jb = (sigjmp_buf_amd64*)j;

	if(jb->set)
		_psigblocked = jb->blocked;
	sp = sigproc(0);
	if(sp == 0 || sp->nstack == 0 || sp->pcstack[sp->nstack-1].u->sp > jb->jmpbuf[JMPBUFSP])
		longjmp((void*)jb->jmpbuf, ret);
	u = sp->pcstack[sp->nstack-1].u;
	sp->nstack--;
	sigdone(sp);
	u->ax = ret;
	if(ret == 0)
		u->ax = 1;
	u->pc = jb->jmpbuf[JMPBUFPC];
	u->sp = jb->jmpbuf[JMPBUFSP] + 8;
	/* GCC port: and the System V callee-saved registers sigsetjmp saved */
	u->bp = jb->jmpbuf[2];
	u->bx = jb->jmpbuf[3];
	u->r12 = jb->jmpbuf[4];
	u->r13 = jb->jmpbuf[5];
	u->r14 = jb->jmpbuf[6];
	u->r15 = jb->jmpbuf[7];
	_NOTED(3);	/* NRSTR */
}
#else
#include "../plan9/lib.h"
#include "../plan9/sys9.h"
#include <signal.h>
#include <setjmp.h>

/* A stack to hold pcs when signals nest */
#define MAXSIGSTACK 20
typedef struct Pcstack Pcstack;
static struct Pcstack {
	int sig;
	void (*hdlr)(int, char*, Ureg*);
	unsigned long long restorepc;
	Ureg *u;
} pcstack[MAXSIGSTACK];
static int nstack = 0;

static void notecont(Ureg*, char*);

void
_notetramp(int sig, void (*hdlr)(int, char*, Ureg*), Ureg *u)
{
	Pcstack *p;

	if(nstack >= MAXSIGSTACK)
		_NOTED(1);	/* nesting too deep; just do system default */
	p = &pcstack[nstack];
	p->restorepc = u->pc;
	p->sig = sig;
	p->hdlr = hdlr;
	p->u = u;
	nstack++;
	u->pc = (unsigned long long) notecont;
	_NOTED(2);	/* NSAVE: clear note but hold state */
}

static void
notecont(Ureg *u, char *s)
{
	Pcstack *p;
	void(*f)(int, char*, Ureg*);

	p = &pcstack[nstack-1];
	f = p->hdlr;
	u->pc = p->restorepc;
	nstack--;
	(*f)(p->sig, s, u);
	_NOTED(3);	/* NRSTR */
}

#define JMPBUFPC 1
#define JMPBUFSP 0

extern sigset_t	_psigblocked;

typedef struct {
	sigset_t set;
	sigset_t blocked;
	unsigned long long jmpbuf[2];
} sigjmp_buf_amd64;

void
siglongjmp(sigjmp_buf j, int ret)
{
	struct Ureg *u;
	sigjmp_buf_amd64 *jb;

	jb = (sigjmp_buf_amd64*)j;

	if(jb->set)
		_psigblocked = jb->blocked;
	if(nstack == 0 || pcstack[nstack-1].u->sp > jb->jmpbuf[JMPBUFSP])
		longjmp((void*)jb->jmpbuf, ret);
	u = pcstack[nstack-1].u;
	nstack--;
	u->ax = ret;
	if(ret == 0)
		u->ax = 1;
	u->pc = jb->jmpbuf[JMPBUFPC];
	u->sp = jb->jmpbuf[JMPBUFSP] + 8;
	_NOTED(3);	/* NRSTR */
}
#endif
