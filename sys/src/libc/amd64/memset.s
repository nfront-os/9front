/*
 * GCC port: GAS/System V translation: p, c, n arrive in %rdi, %esi,
 * %rdx; p is kept in %r8 for the return value (stosb advances %rdi, the
 * original kept p in RARG), %r9 replaces the callee-saved BX.
 */
.globl memset
memset:
	cld
	movq	%rdi, %r8
	movzbl	%sil, %eax
	movq	%rdx, %r9
/*
 * if not enough bytes, just set bytes
 */
	cmpq	$9, %r9
	jbe	3f
/*
 * if not aligned, just set bytes
 */
	movq	%rdi, %rcx
	andl	$3, %ecx
	jne	3f
/*
 * build word in AX
 */
	movb	%al, %ah
	movl	%eax, %ecx
	shll	$16, %ecx
	orl	%ecx, %eax
/*
 * set whole longs
 */
	movq	%r9, %rcx
	shrq	$2, %rcx
	andl	$3, %r9d
	rep; stosl
/*
 * set the rest, by bytes
 */
3:
	movl	%r9d, %ecx
	rep; stosb
	movq	%r8, %rax
	ret
