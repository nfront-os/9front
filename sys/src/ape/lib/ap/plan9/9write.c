#include "lib.h"
#include <string.h>
#include <stdlib.h>
#include "sys9.h"
#include "dir.h"

long
_WRITE(int fd, const void *buf, long n)	/* GCC port: types as in the prototype, kencc ignored the difference */
{
	return _PWRITE(fd, buf, n, -1LL);
}
