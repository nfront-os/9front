/*
 * GCC port: GAS translation of setjmp.s, as libc/386/setjmp.S: the
 * cdecl callee-saved registers are saved too (8c never had to), after
 * sp and pc: jmp_buf [2]=bp [3]=bx [4]=si [5]=di (jmp_buf is int[10] in
 * sys/include/ape/setjmp.h). sigjmp_buf: savemask, blocked, then the
 * jmp_buf at 8 (notetramp.c). The return pc is staged in %ecx/%edx
 * instead of %ebx.
 */
.globl longjmp
longjmp:
	movl	8(%esp), %eax		/* r */
	cmpl	$0, %eax
	jne	1f			/* ansi: "longjmp(0) => longjmp(1)" */
	movl	$1, %eax		/* bless their pointed heads */
1:	movl	4(%esp), %edx		/* l */
	movl	8(%edx), %ebp		/* restore callee-saved registers */
	movl	12(%edx), %ebx
	movl	16(%edx), %esi
	movl	20(%edx), %edi
	movl	0(%edx), %esp		/* restore sp */
	movl	4(%edx), %ecx		/* put return pc on the stack */
	movl	%ecx, 0(%esp)
	ret

.globl setjmp
setjmp:
	movl	4(%esp), %eax		/* l */
	movl	%esp, 0(%eax)		/* store sp */
	movl	0(%esp), %ecx		/* store return pc */
	movl	%ecx, 4(%eax)
	movl	%ebp, 8(%eax)		/* store callee-saved registers */
	movl	%ebx, 12(%eax)
	movl	%esi, 16(%eax)
	movl	%edi, 20(%eax)
	movl	$0, %eax		/* return 0 */
	ret

.globl sigsetjmp
sigsetjmp:
	movl	4(%esp), %eax		/* buf */
	movl	8(%esp), %ecx		/* savemask */
	movl	%ecx, 0(%eax)
	movl	$_psigblocked, 4(%eax)
	movl	%esp, 8(%eax)		/* store sp */
	movl	0(%esp), %ecx		/* store return pc */
	movl	%ecx, 12(%eax)
	movl	%ebp, 16(%eax)		/* store callee-saved registers */
	movl	%ebx, 20(%eax)
	movl	%esi, 24(%eax)
	movl	%edi, 28(%eax)
	movl	$0, %eax		/* return 0 */
	ret
