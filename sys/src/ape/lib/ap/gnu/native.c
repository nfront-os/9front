/*
 * GCC port: APE in a native Plan 9 program, whose C library is libc's
 * renamed __p9_* (/$objtype/lib.gcc/libp9.a: Rust's std is on the native
 * libc, the C in a Rust program on APE). APE's _main does not run there:
 * this does what main9.S, _callmain and _envsetup do, from a constructor
 * before every other one (C++'s, the C's), except taking the notes: the
 * program's handler stays. Per proc, as _callmain's locals are: errno and
 * _privates are slots of libc's privalloc (on the proc's stack segment).
 *
 * The program refers to __ape_attach (linking this object) and calls
 * __ape_exit when it ends through libc's exits: APE's atexit functions
 * (stdio's flush with them).
 */
#include <stddef.h>

/* libp9.a's, the native libc's */
extern void *__p9__tos;
extern void **__p9_privalloc(void);

/* APE's: main9.S, callmain.c, _envsetup.c, stdio/exit.c */
extern void *_tos;
extern void **_privates;
extern int _nprivates;
extern int *_errnoloc;
extern char *_plan9err;
extern int _envnotify;
extern void _envsetup(void);
extern void _doatexits(void);

int __ape_attached;

__attribute__((constructor(101))) void
__ape_attach(void)
{
	static char err[128];	/* ERRMAX */

	if(__ape_attached)
		return;
	__ape_attached = 1;
	_tos = __p9__tos;
	_privates = __p9_privalloc();
	_nprivates = 1;
	_errnoloc = (int *)__p9_privalloc();
	_plan9err = err;
	_envnotify = 0;
	_envsetup();
}

void
__ape_exit(void)
{
	if(__ape_attached)
		_doatexits();
}
