#include	<sys/types.h>
#include	<sys/stat.h>
#include	<errno.h>

int
mkfifo(const char *, mode_t)	/* GCC port: types as in the prototype, kencc ignored the difference */
{
#pragma ref path
#pragma ref mode
	errno = 0;
	return -1;
}
