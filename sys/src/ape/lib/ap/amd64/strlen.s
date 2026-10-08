/*
 * GCC port: GAS/System V translation, as libc/amd64/strlen.s: s arrives
 * in %rdi and is kept in %r8 (SCASB advances %rdi, the original reread
 * RARG).
 */
.globl strlen
strlen:
	movl	$0, %eax
	movq	$-1, %rcx
	cld
/*
 * look for end of string
 */
	movq	%rdi, %r8
	repne; scasb

	movq	%rdi, %rax
	subq	%r8, %rax
	subq	$1, %rax
	ret
