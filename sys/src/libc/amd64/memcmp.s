/*
 * GCC port: GAS/System V translation: p1, p2, n arrive in %rdi, %rsi,
 * %rdx; %r9 replaces the callee-saved BX.
 */
.globl memcmp
memcmp:
	movq	%rdx, %r9
	cmpq	$0, %r9
	je	3f
	cld
	movq	%rdi, %rcx
	orq	%rsi, %rcx
	andl	$3, %ecx
	jne	2f
/*
 * first by longs
 */
	movq	%r9, %rcx
	shrq	$2, %rcx

	repe; cmpsl
	jne	4f

/*
 * then by bytes
 */
	andl	$3, %r9d
2:
	movq	%r9, %rcx
	repe; cmpsb
	jne	5f

3:
	movq	$0, %rax
	ret

/*
 * if long found,
 * back up and look by bytes
 */
4:
	movl	$4, %ecx
	subq	%rcx, %rdi
	subq	%rcx, %rsi
	repe; cmpsb

5:
	jbe	6f
	movq	$-1, %rax
	ret
6:
	movq	$1, %rax
	ret
