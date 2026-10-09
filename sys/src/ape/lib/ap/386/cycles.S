/* GCC port: GAS translation of cycles.s (APE: _cycles) */
.globl _cycles
_cycles:					/* time stamp counter; cycles since power up */
	rdtsc
	movl	4(%esp), %ecx		/* &vlong */
	movl	%eax, 0(%ecx)		/* lo */
	movl	%edx, 4(%ecx)		/* hi */
	ret
