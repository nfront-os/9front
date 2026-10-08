/*
 * GCC port: GAS/System V translation: p1, p2 arrive in %rdi, %rsi; p1
 * is kept in %r8 (the original reread RARG), %r9 replaces the
 * callee-saved BX.
 */
.globl strcat
strcat:
	movq	%rdi, %r8
	movl	$0, %eax
	movq	$-1, %rcx
	cld

/*
 * find length of second string
 */
	movq	%rsi, %rdi
	repne; scasb

	movq	%rdi, %r9
	subq	%rsi, %r9

/*
 * find end of first string
 */
	movq	%r8, %rdi
	repne; scasb

/*
 * copy the memory
 */
	subq	$1, %rdi
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
	movq	%r9, %rcx
	rep; movsb

	movq	%r8, %rax
	ret
