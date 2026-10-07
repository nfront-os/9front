/*
 * GCC port: GAS/SysV translation of the original 6a source. The original
 * used RARG (6c's amd64 first-argument register convention) for the Label*
 * argument; under gcc's SysV ABI the first integer/pointer argument is in
 * %rdi instead, so RARG -> %rdi (not a register-for-register swap to %rbp --
 * that's this fork's unrelated kernel syscall-trap convention, not the C
 * calling convention).
 *
 * Also save/restore the SysV callee-saved registers (%rbp, %rbx,
 * %r12-%r15; jmp_buf layout in amd64/include/u.h), which 6c never had to:
 * a longjmp() into another context (libthread's thread switch) must give
 * the frames above setjmp() back their own values. The return pc is
 * staged in %rax/%rdx instead of %rbx for the same reason.
 */

.globl longjmp
longjmp:
	movl	%esi, %eax	/* r: second arg */
	cmpl	$0, %eax
	jne	1f
	movl	$1, %eax	/* ansi: "longjmp(0) => longjmp(1)" */
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
