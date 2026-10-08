/*
 * GCC port: GAS/System V translation, as libc/amd64/setjmp.s. The
 * buffer arrives in %rdi (6c: RARG), the value in %esi. The System V
 * callee-saved registers are saved too (6c never had to): after a
 * longjmp the frames above setjmp need their own %rbp, %rbx, %r12-%r15.
 * jmp_buf: [0]=sp [1]=pc [2]=bp [3]=bx [4]=r12 [5]=r13 [6]=r14 [7]=r15
 * (sys/include/ape/setjmp.h). sigjmp_buf is notetramp.c's
 * sigjmp_buf_amd64: savemask, blocked (sigset_t, a long: 8 bytes under
 * gcc), then the jmp_buf at 16.
 * The return pc is staged in %rax/%rdx instead of BX.
 */
.globl longjmp
longjmp:
	movl	%esi, %eax
	cmpl	$0, %eax
	jne	1f		/* ansi: "longjmp(0) => longjmp(1)" */
	movl	$1, %eax	/* bless their pointed heads */
1:
	movq	16(%rdi), %rbp	/* restore callee-saved registers */
	movq	24(%rdi), %rbx
	movq	32(%rdi), %r12
	movq	40(%rdi), %r13
	movq	48(%rdi), %r14
	movq	56(%rdi), %r15
	movq	0(%rdi), %rsp	/* restore sp */
	movq	8(%rdi), %rdx	/* put return pc on the stack */
	movq	%rdx, 0(%rsp)
	ret

.globl setjmp
setjmp:
	movq	%rsp, 0(%rdi)	/* store sp */
	movq	0(%rsp), %rax	/* store return pc */
	movq	%rax, 8(%rdi)
	movq	%rbp, 16(%rdi)	/* store callee-saved registers */
	movq	%rbx, 24(%rdi)
	movq	%r12, 32(%rdi)
	movq	%r13, 40(%rdi)
	movq	%r14, 48(%rdi)
	movq	%r15, 56(%rdi)
	movl	$0, %eax	/* return 0 */
	ret

.globl sigsetjmp
sigsetjmp:
	movslq	%esi, %rax	/* savemask */
	movq	%rax, 0(%rdi)
	movq	$_psigblocked, 8(%rdi)
	movq	%rsp, 16(%rdi)	/* store sp */
	movq	0(%rsp), %rax	/* store return pc */
	movq	%rax, 24(%rdi)
	movq	%rbp, 32(%rdi)	/* store callee-saved registers */
	movq	%rbx, 40(%rdi)
	movq	%r12, 48(%rdi)
	movq	%r13, 56(%rdi)
	movq	%r14, 64(%rdi)
	movq	%r15, 72(%rdi)
	movl	$0, %eax	/* return 0 */
	ret
