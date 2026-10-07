/*
 * GCC port: GAS/System V translation: p1, p2 arrive in %rdi, %rsi; p1
 * is kept in %r8 (the original reread RARG), %r9 replaces the
 * callee-saved BX.
 */
.globl strcpy
strcpy:
	movq	%rdi, %r8
	movl	$0, %eax
	movq	$-1, %rcx
	cld
/*
 * find end of second string
 */
	movq	%rsi, %rdi
	repne; scasb

	movq	%rdi, %r9
	subq	%rsi, %r9

/*
 * copy the memory
 */
	movq	%r8, %rdi
/*
 * copy whole longs, if aligned
 */
	movq	%rdi, %rcx
	orq	%rsi, %rcx
	andl	$3, %ecx
	jne	1f
	movq	%r9, %rcx
	shrq	$2, %rcx
	rep; movsl
/*
 * copy the rest, by bytes
 */
	andl	$3, %r9d
1:
	movl	%r9d, %ecx
	rep; movsb

	movq	%r8, %rax
	ret
