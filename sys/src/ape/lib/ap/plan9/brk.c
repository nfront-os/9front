#include "lib.h"
#include <errno.h>
#include <stdint.h>
#include "sys9.h"

extern char	end[];	/* GCC port: was char end[]; gcc makes that a one-element common, and the linker's end then lies inside bss: the heap overwrote the variables after it */
static	char	*bloc = { end };
extern	int	_BRK_(void*);

char *
brk(char *p)
{
	unsigned long n;

	n = (uintptr_t)p;
	n += 3;
	n &= ~3;
	if(_BRK_((void*)n) < 0){
		errno = ENOMEM;
		return (char *)-1;
	}
	bloc = (char *)n;
	return 0;
}

void *
sbrk(unsigned long n)
{
	n += 3;
	n &= ~3;
	if(_BRK_((void *)(bloc+n)) < 0){
		errno = ENOMEM;
		return (void *)-1;
	}
	bloc += n;
	return (void *)(bloc-n);
}
