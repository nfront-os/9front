/* GCC port: GAS translation of tas.s; %ecx instead of %ebx (callee-saved) */
.globl _tas
_tas:
	movl	$0xdeadead, %eax
	movl	4(%esp), %ecx		/* l */
	xchgl	%eax, (%ecx)
	ret
