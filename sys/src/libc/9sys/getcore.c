/*
 * NIX: the core the process runs on and its role, from the Tos
 * (no system call); after NIX's libc/9sys/getcore.c.
 */
#include <u.h>
#include <libc.h>
#include <tos.h>

int
getcore(int *role)
{
	if(role != nil)
		*role = _tos->nixtype;
	return _tos->core;
}
