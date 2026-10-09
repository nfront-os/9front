/* GCC port: GAS translation of tas.s (APE: tas); %ecx instead of %ebx (callee-saved) */
.globl tas
tas:
	movl	$0xdeadead, %eax
	movl	4(%esp), %ecx		/* l */
	xchgl	%eax, (%ecx)
	ret
