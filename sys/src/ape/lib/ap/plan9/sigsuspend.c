#include <signal.h>
#include <errno.h>

/*
 * BUG: doesn't work
 */

int
sigsuspend(const sigset_t *)	/* GCC port: types as in the prototype, kencc ignored the difference */
{
	errno = EINVAL;
	return -1;
}
