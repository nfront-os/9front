#include "lib.h"
#include <errno.h>
#include <unistd.h>
#include "sys9.h"

pid_t
fork(void)
{
	int n;

	n = _RFORK(RFENVG|RFFDG|RFPROC);
	if(n < 0)
		_syserrno();
	if(n == 0) {
		_detachbuf();
		_sessleader = 0;
	}
	if(n > 0){	/* GCC port: SIGCHLD (gnu/sigchld.c) */
		extern void __sigchld_forked(void);

		__sigchld_forked();
	}
	return n;
}
