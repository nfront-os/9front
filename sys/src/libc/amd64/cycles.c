#include <u.h>
#include <libc.h>

void
cycles(uvlong *x)
{
	u32int a, d;

	__asm__ __volatile__ ("rdtsc" : "=a" (a), "=d" (d));
	*x = ((uvlong)d << 32) | a;
}
