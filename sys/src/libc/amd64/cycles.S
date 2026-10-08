/*
 * GCC port: GAS/System V translation (pointer argument in %rdi).
 */
.globl cycles
cycles:				/* time stamp counter; cycles since power up */
	rdtsc
	movl	%eax, 0(%rdi)		/* lo */
	movl	%edx, 4(%rdi)		/* hi */
	ret
