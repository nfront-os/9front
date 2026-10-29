/*
 * C99's llabs, lldiv and _Exit, C11's quick_exit and at_quick_exit,
 * which APE lacks.
 */
#include <stdlib.h>
#include <unistd.h>

long long
llabs(long long v)
{
	return v < 0 ? -v : v;
}

lldiv_t
lldiv(long long n, long long d)
{
	lldiv_t r;

	r.quot = n / d;
	r.rem = n % d;
	return r;
}

/* no atexit functions, no stdio flushing: _exit */
void
_Exit(int status)
{
	_exit(status);
}

/* C11: at least 32; run in reverse order of registration */
enum { Nquick = 32 };
static void (*quick[Nquick])(void);
static int nquick;

int
at_quick_exit(void (*f)(void))
{
	if(nquick >= Nquick)
		return -1;
	quick[nquick++] = f;
	return 0;
}

void
quick_exit(int status)
{
	while(nquick > 0)
		(*quick[--nquick])();
	_Exit(status);
}
