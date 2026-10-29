/*
 * __hwcap for musl's i386 fenv.s (ape/lib/musl): on Linux the kernel's
 * AT_HWCAP, cpuid's feature bits (EDX); fenv.s tests SSE (bit 25) to
 * set mxcsr along with the x87 control word. 9front has no auxiliary
 * vector: a constructor asks cpuid, before the program's own.
 */
#ifdef __i386__
/* cpuid itself: gcc's <cpuid.h> is not on the tree's include path (-nostdinc);
 * every CPU of the tree's -march=pentium has the instruction */

__attribute__((visibility("hidden"))) unsigned long __hwcap;

static void __attribute__((constructor(101)))
hwcapinit(void)
{
	unsigned int a, b, c, d;

	__asm__ volatile("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(1), "c"(0));
	__hwcap = d;
}
#endif
