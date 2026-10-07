/*
 * long callcheck(void *fn, long a, long b, long c, long d)
 * Calls fn(a, b, c, d) with %rbx, %rbp, %r12-%r15 holding known values
 * and returns a bit mask of the callee-saved registers fn did not
 * preserve (bit 0 rbx, 1 rbp, 2 r12, 3 r13, 4 r14, 5 r15) or'ed with
 * 0x40 if the direction flag was left set.
 */
	.globl	callcheck
callcheck:
	pushq	%rbx
	pushq	%rbp
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15
	movq	%rdi, %rax
	movq	%rsi, %rdi
	movq	%rdx, %rsi
	movq	%rcx, %rdx
	movq	%r8, %rcx
	movq	$0x1111111111111111, %rbx
	movq	$0x2222222222222222, %rbp
	movq	$0x3333333333333333, %r12
	movq	$0x4444444444444444, %r13
	movq	$0x5555555555555555, %r14
	movq	$0x6666666666666666, %r15
	call	*%rax
	xorl	%eax, %eax
	movq	$0x1111111111111111, %rcx
	cmpq	%rcx, %rbx
	je	1f
	orl	$1, %eax
1:	movq	$0x2222222222222222, %rcx
	cmpq	%rcx, %rbp
	je	1f
	orl	$2, %eax
1:	movq	$0x3333333333333333, %rcx
	cmpq	%rcx, %r12
	je	1f
	orl	$4, %eax
1:	movq	$0x4444444444444444, %rcx
	cmpq	%rcx, %r13
	je	1f
	orl	$8, %eax
1:	movq	$0x5555555555555555, %rcx
	cmpq	%rcx, %r14
	je	1f
	orl	$16, %eax
1:	movq	$0x6666666666666666, %rcx
	cmpq	%rcx, %r15
	je	1f
	orl	$32, %eax
1:	pushfq
	popq	%rcx
	testl	$0x400, %ecx
	je	1f
	orl	$64, %eax
	cld
1:	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbp
	popq	%rbx
	ret
