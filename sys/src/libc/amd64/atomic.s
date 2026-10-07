/*
 * GCC port: GAS/System V translation: pointer in %rdi (6c: RARG),
 * value/compare arguments in %esi/%rsi and %edx/%rdx instead of
 * v+8(FP)/c+8(FP)/v+16(FP); %ecx/%rcx instead of the callee-saved BX.
 */

/* get variants */
.globl agetl
agetl:
	movl	(%rdi), %eax
	ret
.globl agetp
agetp:
	movq	(%rdi), %rax
	ret

/* set variants */
.globl aswapl
aswapl:
	movl	%esi, %eax
	lock; xchgl	%eax, (%rdi)
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
	lock; xaddl	%eax, (%rdi)
	addq	%rcx, %rax
	ret

/* cas variants */
.globl acasl
acasl:
	movl	%esi, %eax
	movl	%edx, %ecx
	lock; cmpxchgl	%ecx, (%rdi)
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
