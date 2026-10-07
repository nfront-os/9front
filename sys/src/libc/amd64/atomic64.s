/*
 * GCC port: GAS/System V translation, see atomic.s.
 */

/* get variants */
.globl agetv
agetv:
	movq	(%rdi), %rax
	ret

/* set variants */
.globl aswapv
aswapv:
	movq	%rsi, %rax
	lock; xchgq	%rax, (%rdi)
	ret

/* inc variants */
.globl aincv
aincv:
	movq	%rsi, %rcx
	movq	%rcx, %rax
	lock; xaddq	%rax, (%rdi)
	addq	%rcx, %rax
	ret

/* cas variants */
.globl acasv
acasv:
	movq	%rsi, %rax
	movq	%rdx, %rcx
	lock; cmpxchgq %rcx, (%rdi)
	sete	%al
	movzbl	%al, %eax
	ret
