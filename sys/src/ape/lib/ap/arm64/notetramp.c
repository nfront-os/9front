#ifdef __GNUC_MINOR__	/* GCC port: gcc (APE is compiled -U__GNUC__); 9front's notetramp.c for kencc below */
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

void notecont(Ureg*, char*);	/* GCC port: was static, notify.S's _notecont calls it */
extern void _notecont(void);	/* GCC port: AAPCS64 trampoline to notecont, notify.S */

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
	u->pc = (unsigned long long) _notecont;	/* GCC port: was notecont, entered the 7c way */
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
	int set;		/* GCC port: the 32-bit words sigsetjmp stores */
	int blocked;
	unsigned long long jmpbuf[22];	/* GCC port: was [2], setjmp.S saves x19-x29, d8-d15 too */
} sigjmp_buf_arm64;

void
siglongjmp(sigjmp_buf j, int ret)
{
	struct Ureg *u;
	sigjmp_buf_arm64 *jb;

	jb = (sigjmp_buf_arm64*)j;

	if(jb->set)
		_psigblocked = jb->blocked;
	if(nstack == 0 || pcstack[nstack-1].u->sp > jb->jmpbuf[JMPBUFSP])
		longjmp((void*)jb->jmpbuf, ret);
	u = pcstack[nstack-1].u;
	nstack--;
	u->r0 = ret;
	if(ret == 0)
		u->r0 = 1;
	u->pc = jb->jmpbuf[JMPBUFPC];
	u->sp = jb->jmpbuf[JMPBUFSP];
	/* GCC port: and the callee-saved registers sigsetjmp saved (d8-d15 are not in the Ureg) */
	u->r19 = jb->jmpbuf[2];
	u->r20 = jb->jmpbuf[3];
	u->r21 = jb->jmpbuf[4];
	u->r22 = jb->jmpbuf[5];
	u->r23 = jb->jmpbuf[6];
	u->r24 = jb->jmpbuf[7];
	u->r25 = jb->jmpbuf[8];
	u->r26 = jb->jmpbuf[9];
	u->r27 = jb->jmpbuf[10];
	u->r28 = jb->jmpbuf[11];
	u->r29 = jb->jmpbuf[12];
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

void
siglongjmp(sigjmp_buf j, int ret)
{
	struct Ureg *u;

	if(j[0])
		_psigblocked = j[1];
	if(nstack == 0 || pcstack[nstack-1].u->sp > j[2+JMPBUFSP])
		longjmp(j+2, ret);
	u = pcstack[nstack-1].u;
	nstack--;
	u->r0 = ret;
	if(ret == 0)
		u->r0 = 1;
	u->pc = j[2+JMPBUFPC];
	u->sp = j[2+JMPBUFSP];
	_NOTED(3);	/* NRSTR */
}
#endif
