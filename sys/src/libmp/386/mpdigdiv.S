/*
 * GCC port: GAS translation. The arguments are where 8c left them
 * (cdecl, 4(%esp) up); the original used BX, SI, DI, BP freely, which
 * cdecl has the callee save: they are pushed first, the arguments are
 * then 16 bytes further up (a+0(FP) is 20(%esp)).
 */
.globl mpdigdiv
mpdigdiv:
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	pushl	%ebp
	movl	20(%esp), %ebx		/* dividend */
	movl	0(%ebx), %eax
	movl	4(%ebx), %edx
	movl	24(%esp), %ebx		/* divisor */
	movl	28(%esp), %ebp		/* quotient */
	xorl	%ecx, %ecx
	cmpl	%ebx, %edx		/* dividend >= 2^32 * divisor */
	jae	1f
	cmpl	%ecx, %ebx		/* divisor == 0 */
	je	1f
	divl	%ebx			/* AX = DX:AX/BX */
	movl	%eax, 0(%ebp)
	jmp	2f

	/* return all 1's */
1:
	notl	%ecx
	movl	%ecx, 0(%ebp)
2:
	popl	%ebp
	popl	%edi
	popl	%esi
	popl	%ebx
	ret
