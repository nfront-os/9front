/*
 * GCC port: GAS/System V translation: p1, p2, c, n arrive in %rdi,
 * %rsi, %edx, %rcx. p1 and p2 are kept in %r8/%r10 (the original reread
 * RARG and p2+8(FP)), n in %r11, %r9 replaces the callee-saved BX.
 */
.globl memccpy
memccpy:
	movq	%rdi, %r8
	movq	%rsi, %r10
	movq	%rcx, %r11
	cmpq	$0, %rcx
	je	1f
	movq	%r10, %rdi
	movzbl	%dl, %eax
	cld
/*
 * find the character in the second string
 */
	repne; scasb
	je	2f

/*
 * if not found, set count to 'n'
 */
1:
	movl	$0, %eax
	movq	%r11, %r9
	jmp	3f

/*
 * if found, set count to bytes thru character
 */
2:
	movq	%rdi, %rax
	subq	%r10, %rax
	movq	%rax, %r9
	addq	%r8, %rax

/*
 * copy the memory
 */
3:
	movq	%r8, %rdi
	movq	%r10, %rsi
/*
 * copy whole longs, if aligned
 */
	movq	%rdi, %rdx
	orq	%rsi, %rdx
	andl	$3, %edx
	jne	4f
	movq	%r9, %rcx
	shrq	$2, %rcx
	rep; movsl
/*
 * copy the rest, by bytes
 */
	andl	$3, %r9d
4:
	movq	%r9, %rcx
	rep; movsb

	ret
