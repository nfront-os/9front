#include "lib.h"
#include "sys9.h"
#include <stdlib.h>
#include <string.h>	/* GCC port: memset, kencc declared it implicitly */

extern	void _envsetup(void);
extern	char **environ;
extern	int *_errnoloc;

void **_privates;
int _nprivates;
char *_plan9err;

enum{
	NPRIVATES=16,
};

#pragma profile off

#ifdef __GNUC_MINOR__
/*
 * GCC port: ../gnu/pthread.c, when linked: runs main on a stack of shared
 * memory, as the other threads' stacks (each proc has its own copy of the
 * stack segment: a thread would see a stale copy of main's locals). libgcc's
 * __wrap_main calls it too, for the gcc driver's programs (--wrap=main); the
 * tree's own programs link without it, so here as well (it runs main once).
 */
extern int __plan9_mainstack(int (*)(int, char **, char **), int, char **, char **)
	__attribute__((weak));

static int
apemain3(int argc, char **argv, char **envp)
{
	extern int main(int, char**, char**);

	return main(argc, argv, envp);
}
#endif

/*
 * GCC port: front takes char *arg0 and passes &arg0 as argv, which
 * relies on 6c leaving the arguments in place on the initial stack.
 * Under System V they arrive in registers, so amd64/main9.s passes the
 * argv pointer itself (as libc/9sys/callmain.c). APE is compiled
 * -U__GNUC__ (amd64/ape.json); natively (6c, 6a's main9.s) front's.
 */
void
#ifdef __GNUC_MINOR__
_callmain(int (*f)(int, char**), int argc, char **argv)
#else
_callmain(int (*f)(int, char**), int argc, char *arg0)
#endif
{
	int errno;
	char err[ERRMAX];
	void *privates[NPRIVATES];

	memset(privates, 0, sizeof(privates));
	err[0] = '\0';
	_privates = privates;
	_nprivates = NPRIVATES;
	_errnoloc = &errno;
	_plan9err = &err[0];
#ifdef __GNUC_MINOR__
	exit(f(argc, argv));
#else
	exit(f(argc, &arg0));
#endif
}

int
_apemain(int argc, char **argv)
{
	_envsetup();
	extern int main(int, char**, char**);
#ifdef __GNUC_MINOR__
	if(__plan9_mainstack)
		return __plan9_mainstack(apemain3, argc, argv, environ);
#endif
	return main(argc, argv, environ);
}

#pragma profile on
