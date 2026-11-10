/*
 * GCC port: the pool imagmem for APE's libmemdraw (pool.h): malloc's memory
 */
#include <u.h>
#include <libc.h>
#include "pool.h"

static Pool apeimagmem = { "ape", nil };
Pool *imagmem = &apeimagmem;

void*
poolalloc(Pool *p, ulong n)
{
	USED(p);
	return malloc(n);
}

void
poolfree(Pool *p, void *v)
{
	USED(p);
	free(v);
}
