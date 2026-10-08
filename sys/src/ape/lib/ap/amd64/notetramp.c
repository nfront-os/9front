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

void notecont(Ureg*, char*);	/* GCC port: was static, amd64/notify.S's _notecont calls it */
extern void _notecont(void);	/* GCC port: System V trampoline to notecont, notify.S */

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
	u->pc = (unsigned long long) _notecont;	/* GCC port: was notecont, entered the 6c way */
	_NOTED(2);	/* NSAVE: clear note but hold state */
}

void	/* GCC port: was static */
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
	unsigned long long jmpbuf[8];	/* GCC port: was [2], setjmp.s saves bp, bx, r12-r15 too */
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
	/* GCC port: and the System V callee-saved registers sigsetjmp saved */
	u->bp = jb->jmpbuf[2];
	u->bx = jb->jmpbuf[3];
	u->r12 = jb->jmpbuf[4];
	u->r13 = jb->jmpbuf[5];
	u->r14 = jb->jmpbuf[6];
	u->r15 = jb->jmpbuf[7];
	_NOTED(3);	/* NRSTR */
}
