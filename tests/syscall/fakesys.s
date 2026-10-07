/*
 * Stands in for the kernel side of a 9front amd64 system call in the
 * host test: the stubs' `syscall' is replaced by `call fakesys', which
 * records what the kernel would read (call number in %rbp, arguments at
 * 8(%rsp), 16(%rsp), ... as of the syscall instruction -- one slot
 * further here because of the call's return address) and then zeroes
 * %rbx, %rbp, %r12, %r13 like forkret does. Returns the call number.
 */
	.globl	fakesys, sysnum, sysargs
	.bss
	.p2align 3
sysnum:	.quad	0
sysargs:	.zero	48
	.text
fakesys:
	movq	%rbp, sysnum(%rip)
	leaq	sysargs(%rip), %rax
	movq	16(%rsp), %rcx
	movq	%rcx, 0(%rax)
	movq	24(%rsp), %rcx
	movq	%rcx, 8(%rax)
	movq	32(%rsp), %rcx
	movq	%rcx, 16(%rax)
	movq	40(%rsp), %rcx
	movq	%rcx, 24(%rax)
	movq	48(%rsp), %rcx
	movq	%rcx, 32(%rax)
	movq	56(%rsp), %rcx
	movq	%rcx, 40(%rax)
	movq	sysnum(%rip), %rax
	xorl	%ebx, %ebx
	xorl	%ebp, %ebp
	xorl	%r12d, %r12d
	xorl	%r13d, %r13d
	ret
