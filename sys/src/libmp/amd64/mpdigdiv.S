/*
 * GCC port: GAS/System V translation: dividend, divisor, quotient
 * arrive in %rdi, %esi, %rdx (the original read divisor and quotient
 * from the stack); DIVL writes %edx, so quotient moves to %r8.
 */
.globl mpdigdiv
mpdigdiv:
	movq	%rdx, %r8
	movl	0(%rdi), %eax
	movl	4(%rdi), %edx
	xorl	%ecx, %ecx
	cmpl	%esi, %edx		/* dividend >= 2^32 * divisor */
	jae	1f
	cmpl	%ecx, %esi		/* divisor == 0 */
	je	1f
	divl	%esi			/* AX = DX:AX/BX */
	movl	%eax, 0(%r8)
	ret

	/* return all 1's */
1:
	notl	%ecx
	movl	%ecx, 0(%r8)
	ret
