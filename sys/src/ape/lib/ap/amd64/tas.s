/*
 * GCC port: GAS/System V translation: the pointer arrives in %rdi.
 */
.globl tas
tas:
	movl	$0xdeadead, %eax
	xchgl	%eax, (%rdi)
	ret
