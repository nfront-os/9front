#include	"l.h"

/*
 * fake malloc
 * 9front: sizes are ulong as in 9front's libc.h (Miller's libc widened them to uintptr)
 */
void*
malloc(ulong n)
{
	void *p;

	while(n & 7)
		n++;
	while(nhunk < n)
		gethunk();
	p = hunk;
	nhunk -= n;
	hunk += n;
	return p;
}

void*
mallocz(ulong n, int clr)
{
	void *p;

	while(n & 7)
		n++;
	while(nhunk < n)
		gethunk();
	p = hunk;
	if (clr)
		memset(p, 0, n);
	nhunk -= n;
	hunk += n;
	return p;
}

void
free(void *p)
{
	USED(p);
}

void*
calloc(ulong m, ulong n)
{
	void *p;

	n *= m;
	p = malloc(n);
	memset(p, 0, n);
	return p;
}

void*
realloc(void*, ulong)
{
	fprint(2, "realloc called\n");
	abort();
	return 0;
}

void
setmalloctag(void *v, uintptr pc)
{
	USED(v, pc);
}
