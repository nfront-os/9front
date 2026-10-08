#include <u.h>
#include <libc.h>

void **_privates;
int _nprivates;

enum{
	NPRIVATES=16,
};

#pragma profile off

#ifdef __GNUC__
/*
 * GCC port: front takes char *arg0 and passes &arg0 as argv, which
 * relies on 6c leaving the arguments in place on the initial stack.
 * Under System V they arrive in registers, so amd64/main9.S passes the
 * argv pointer itself. Natively (6c with 6a's main9.s) front's below.
 */
void
_callmain(void (*main)(int, char**), int argc, char **argv)
{
	void *privates[NPRIVATES];

	memset(privates, 0, sizeof(privates));
	_privates = privates;
	_nprivates = NPRIVATES;
	main(argc, argv);
	exits("main");
}
#else
void
_callmain(void (*main)(int, char**), int argc, char *arg0)
{
	void *privates[NPRIVATES];

	memset(privates, 0, sizeof(privates));
	_privates = privates;
	_nprivates = NPRIVATES;
	main(argc, &arg0);
	exits("main");
}
#endif

#pragma profile on
