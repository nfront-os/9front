#include <u.h>
#include <libc.h>

void **_privates;
int _nprivates;

enum{
	NPRIVATES=16,
};

#pragma profile off

/*
 * GCC port: front takes char *arg0 and passes &arg0 as argv, which
 * relies on 6c leaving the arguments in place on the initial stack.
 * Under System V they arrive in registers, so amd64/main9.s passes the
 * argv pointer itself.
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

#pragma profile on
