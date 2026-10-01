/*
 * Print functions for system call tracing.
 */
#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"

#include "../../libc/9syscall/sys.h"

/* gcc port: syscall number aliases (underscore-prefixed versions) */
#ifndef SYSR1
#define SYSR1	1
#endif
#ifndef _FSESSION
#define _FSESSION	48
#endif
#ifndef _WAIT
#define _WAIT	AWAIT
#endif
#ifndef _READ
#define _READ	PREAD
#endif
#ifndef _WRITE
#define _WRITE	PWRITE
#endif
#ifndef _ERRSTR
#define _ERRSTR	ERRSTR
#endif
#ifndef _FSTAT
#define _FSTAT	FSTAT
#endif
#ifndef _FWSTAT
#define _FWSTAT	FWSTAT
#endif
#ifndef _MOUNT
#define _MOUNT	MOUNT
#endif
#ifndef _STAT
#define _STAT	STAT
#endif
#ifndef _WSTAT
#define _WSTAT	WSTAT
#endif
#ifndef _NSEC
#define _NSEC	NSEC
#endif
#ifndef OSEEK
#define OSEEK	SEEK
#endif

// WE ARE OVERRUNNING SOMEHOW
static void
fmtrwdata(Fmt* f, char* a, int n, char* suffix)
{
	int i;
	char *t;

	if(a == nil){
		fmtprint(f, "0x0%s", suffix);
		return;
	}
	validaddr((uintptr)a, n, 0);
	t = smalloc(n+1);
	t[n] = 0;
	for(i = 0; i < n; i++)
		if(a[i] > 0x20 && a[i] < 0x7f)	/* printable ascii? */
			t[i] = a[i];
		else
			t[i] = '.';

	fmtprint(f, " %#p/\"%s\"%s", a, t, suffix);
	free(t);
}

static void
fmtuserstring(Fmt* f, char* a, char* suffix)
{
	char *t, *e;
	int n;

	if(a == nil){
		fmtprint(f, "0/\"\"%s", suffix);
		return;
	}
	validaddr((uintptr)a, 1, 0);
	n = 1<<16;
	e = vmemchr(a, 0, n);
	if(e != nil)
		n = e - a;
	t = smalloc(n+1);
	memmove(t, a, n);
	t[n] = 0;
	fmtprint(f, "%#p/\"%s\"%s", a, t, suffix);
	free(t);
}

void
syscallfmt(ulong syscallno, uintptr pc, uchar *list)
{
	long l;
	Fmt fmt;
	void *v;
	vlong vl;
	uintptr p;
	int i[2], len;
	char *a, **argv;

	fmtstrinit(&fmt);
	fmtprint(&fmt, "%uld %s ", up->pid, up->text);

	if(syscallno >= nsyscall)
		fmtprint(&fmt, " %uld ", syscallno);
	else
		fmtprint(&fmt, "%s ", sysctab[syscallno]?
			sysctab[syscallno]: "huh?");

	fmtprint(&fmt, "%p ", pc);
	switch(syscallno){
	case SYSR1:
		p = ARG(list, uintptr);
		fmtprint(&fmt, "%#p", p);
		break;
	case CHDIR:
	case EXITS:
	case REMOVE:
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, "");
		break;
	case BIND:
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, " ");
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, " ");
		i[0] = ARG(list, int);
		fmtprint(&fmt, "%#ux",  i[0]);
		break;
	case CLOSE:
	case NOTED:
		i[0] = ARG(list, int);
		fmtprint(&fmt, "%d", i[0]);
		break;
	case DUP:
		i[0] = ARG(list, int);
		i[1] = ARG(list, int);
		fmtprint(&fmt, "%d %d", i[0], i[1]);
		break;
	case ALARM:
		l = ARG(list, unsigned long);
		fmtprint(&fmt, "%#lud ", l);
		break;
	case EXEC:
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, "");
		argv = ARG(list, char**);
		evenaddr((uintptr)argv);
		for(;;){
			validaddr((uintptr)argv, sizeof(char**), 0);
			a = *(char **)argv;
			if(a == nil)
				break;
			fmtprint(&fmt, " ");
			fmtuserstring(&fmt, a, "");
			argv++;
		}
		break;
		i[0] = ARG(list, int);
		a = ARG(list, char*);
		fmtprint(&fmt, "%d %#p", i[0], a);
		break;
	case FAUTH:
		i[0] = ARG(list, int);
		a = ARG(list, char*);
		fmtprint(&fmt, "%d", i[0]);
		fmtuserstring(&fmt, a, "");
		break;
	case SEGBRK:
	case RENDEZVOUS:
		v = ARG(list, void*);
		fmtprint(&fmt, "%#p ", v);
		v = ARG(list, void*);
		fmtprint(&fmt, "%#p", v);
		break;
		i[0] = ARG(list, int);
		fmtprint(&fmt, "%d ", i[0]);
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, " ");
		i[0] = ARG(list, int);
		fmtprint(&fmt, "%#ux ", i[0]);
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, "");
		break;
	case OPEN:
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, " ");
		i[0] = ARG(list, int);
		fmtprint(&fmt, "%#ux", i[0]);
		break;
		i[0] = ARG(list, int);
		l = ARG(list, long);
		i[1] = ARG(list, int);
		fmtprint(&fmt, "%d %ld %d", i[0], l, i[1]);
		break;
	case SLEEP:
		l = ARG(list, long);
		fmtprint(&fmt, "%ld", l);
		break;
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, " ");
		a = ARG(list, char*);
		fmtprint(&fmt, "%#p", a);
		break;
	case RFORK:
		i[0] = ARG(list, int);
		fmtprint(&fmt, "%#ux", i[0]);
		break;
	case PIPE:
	case BRK_:
		v = ARG(list, int*);
		fmtprint(&fmt, "%#p", v);
		break;
	case CREATE:
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, " ");
		i[0] = ARG(list, int);
		i[1] = ARG(list, int);
		fmtprint(&fmt, "%#ux %#ux", i[0], i[1]);
		break;
	case FD2PATH:
	case FSTAT:
	case FWSTAT:
		i[0] = ARG(list, int);
		a = ARG(list, char*);
		l = ARG(list, unsigned long);
		fmtprint(&fmt, "%d %#p %lud", i[0], a, l);
		break;
	case NOTIFY:
	case SEGDETACH:
		v = ARG(list, void*);
		fmtprint(&fmt, "%#p", v);
		break;
	case SEGATTACH:
		i[0] = ARG(list, int);
		fmtprint(&fmt, "%d ", i[0]);
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, " ");
		/*FALLTHROUGH*/
	case SEGFREE:
	case SEGFLUSH:
		v = ARG(list, void*);
		l = ARG(list, unsigned long);
		fmtprint(&fmt, "%#p %lud", v, l);
		break;
	case UNMOUNT:
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, " ");
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, "");
		break;
	case SEMACQUIRE:
	case SEMRELEASE:
		v = ARG(list, int*);
		i[0] = ARG(list, int);
		fmtprint(&fmt, "%#p %d", v, i[0]);
		break;
	case TSEMACQUIRE:
		v = ARG(list, int*);
		l = ARG(list, ulong);
		fmtprint(&fmt, "%#p %ld", v, l);
		break;
	case SEEK:
		v = ARG(list, vlong*);
		i[0] = ARG(list, int);
		vl = ARG(list, vlong);
		i[1] = ARG(list, int);
		fmtprint(&fmt, "%#p %d %#llux %d", v, i[0], vl, i[1]);
		break;
	case FVERSION:
		i[0] = ARG(list, int);
		i[1] = ARG(list, int);
		fmtprint(&fmt, "%d %d ", i[0], i[1]);
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, " ");
		l = ARG(list, unsigned long);
		fmtprint(&fmt, "%lud", l);
		break;
	case WSTAT:
	case STAT:
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, " ");
		/*FALLTHROUGH*/
	case ERRSTR:
	case AWAIT:
		a = ARG(list, char*);
		l = ARG(list, unsigned long);
		fmtprint(&fmt, "%#p %lud", a, l);
		break;
	case MOUNT:
		i[0] = ARG(list, int);
		i[1] = ARG(list, int);
		fmtprint(&fmt, "%d %d ", i[0], i[1]);
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, " ");
		i[0] = ARG(list, int);
		fmtprint(&fmt, "%#ux ", i[0]);
		a = ARG(list, char*);
		fmtuserstring(&fmt, a, "");
		break;
	case PREAD:
		i[0] = ARG(list, int);
		v = ARG(list, void*);
		l = ARG(list, long);
		fmtprint(&fmt, "%d %#p %ld", i[0], v, l);
		if(syscallno == PREAD){
			vl = ARG(list, vlong);
			fmtprint(&fmt, " %lld", vl);
		}
		break;
	case PWRITE:
		i[0] = ARG(list, int);
		v = ARG(list, void*);
		l = ARG(list, long);
		fmtprint(&fmt, "%d ", i[0]);
		len = MIN(l, 64);
		fmtrwdata(&fmt, v, len, " ");
		fmtprint(&fmt, "%ld", l);
		if(syscallno == PWRITE){
			vl = ARG(list, vlong);
			fmtprint(&fmt, " %lld", vl);
		}
		break;
		if(sizeof(uintptr) == sizeof(vlong))
			break;
		v = ARG(list, vlong*);
		fmtprint(&fmt, "%#p", v);
		break;
	}

	a = fmtstrflush(&fmt);
	qlock(&up->debug);
	free(up->syscalltrace);
	up->syscalltrace = a;
	qunlock(&up->debug);
}

void
sysretfmt(ulong syscallno, uchar *list, uintptr ret, uvlong start, uvlong stop)
{
	long l;
	void* v;
	Fmt fmt;
	vlong vl;
	int i, len;
	char *a, *errstr;

	fmtstrinit(&fmt);

	errstr = "\"\"";
	switch(syscallno){
	case EXEC:
	case SEGBRK:
	case SEGATTACH:
	case RENDEZVOUS:
		if((void *)ret == (void*)-1)
			errstr = up->syserrstr;
		fmtprint(&fmt, " = %#p", (void *)ret);
		break;
	case AWAIT:
		a = ARG(list, char*);
		l = ARG(list, unsigned long);
		if(ret > 0){
			fmtuserstring(&fmt, a, " ");
			fmtprint(&fmt, "%lud = %ld", l, (long)ret);
		}
		else{
			fmtprint(&fmt, "%#p/\"\" %lud = %ld", a, l, (long)ret);
			errstr = up->syserrstr;
		}
		break;
	case ERRSTR:
		a = ARG(list, char*);
		if(syscallno == _ERRSTR)
			l = 64;
		else
			l = ARG(list, unsigned long);
		if(ret > 0){
			fmtuserstring(&fmt, a, " ");
			fmtprint(&fmt, "%lud = %ld", l, (long)ret);
		}
		else{
			fmtprint(&fmt, "\"\" %lud = %ld", l, (long)ret);
			errstr = up->syserrstr;
		}
		break;
	case FD2PATH:
		i = ARG(list, int);
		USED(i);
		a = ARG(list, char*);
		l = ARG(list, unsigned long);
		if(ret > 0){
			fmtuserstring(&fmt, a, " ");
			fmtprint(&fmt, "%lud = %ld", l, (long)ret);
		}
		else{
			fmtprint(&fmt, "\"\" %lud = %ld", l, (long)ret);
			errstr = up->syserrstr;
		}
		break;
	case PREAD:
		i = ARG(list, int);
		USED(i);
		v = ARG(list, void*);
		l = ARG(list, long);
		if(ret > 0){
			len = MIN(ret, 64);
			fmtrwdata(&fmt, v, len, "");
		}
		else{
			fmtprint(&fmt, "/\"\"");
			errstr = up->syserrstr;
		}
		fmtprint(&fmt, " %ld", l);
		if(syscallno == PREAD){
			vl = ARG(list, vlong);
			fmtprint(&fmt, " %lld", vl);
		}
		fmtprint(&fmt, " = %ld", (long)ret);
		break;
		if(sizeof(uintptr) == sizeof(vlong)){
			fmtprint(&fmt, " = %lld", (vlong)ret);
			break;
		}
		/* wet floor */
	case ALARM:
	case PWRITE:
	default:
		if((long)ret == -1)
			errstr = up->syserrstr;
		fmtprint(&fmt, " = %ld", (long)ret);
		break;
	}
	fmtprint(&fmt, " %s %#llud %#llud\n", errstr, start, stop);

	a = fmtstrflush(&fmt);
	qlock(&up->debug);
	free(up->syscalltrace);
	up->syscalltrace = a;
	qunlock(&up->debug);
}
