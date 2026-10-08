#include "lib.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "sys9.h"

typedef long long vlong;
typedef unsigned int ulong;	/* GCC port: was unsigned long, 32 bits with kencc */
typedef unsigned long long uvlong;
#include "../../../../../include/tos.h"	/* GCC port: was "/sys/include/tos.h", a Plan 9 absolute path */

pid_t
getpid(void)
{
	return _tos->pid;
}
