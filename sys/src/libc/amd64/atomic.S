/*
 * GCC port: GAS/System V translation: pointer in %rdi (6c: RARG),
 * value/compare arguments in %esi/%rsi and %edx/%rdx instead of
 * v+8(FP)/c+8(FP)/v+16(FP); %ecx/%rcx instead of the callee-saved BX.
 */

/*
 * The l variants work on Along.v, a long: 64 bits under gcc (6c: 32),
 * so they use the 64-bit instructions.
 */

/* get variants */
.globl agetl
agetl:
	movq	(%rdi), %rax
	ret
.globl agetp
agetp:
	movq	(%rdi), %rax
	ret

/* set variants */
.globl aswapl
aswapl:
	movq	%rsi, %rax
	lock; xchgq	%rax, (%rdi)
	ret

.globl aswapp
aswapp:
	movq	%rsi, %rax
	lock; xchgq	%rax, (%rdi)
	ret

/* inc variants */
.globl aincl
aincl:
	movq	%rsi, %rcx
	movq	%rcx, %rax
	lock; xaddq	%rax, (%rdi)
	addq	%rcx, %rax
	ret

/* cas variants */
.globl acasl
acasl:
	movq	%rsi, %rax
	movq	%rdx, %rcx
	lock; cmpxchgq	%rcx, (%rdi)
	sete	%al
	movzbl	%al, %eax
	ret

.globl acasp
acasp:
	movq	%rsi, %rax
	movq	%rdx, %rcx
	lock; cmpxchgq %rcx, (%rdi)
	sete	%al
	movzbl	%al, %eax
	ret

/* barriers */
.globl coherence
coherence:
	mfence
	ret
