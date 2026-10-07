/*
 * GCC port: GAS/System V translation: p1, p2, n arrive in %rdi, %rsi,
 * %rdx (6c: RARG, p2+8(FP), n+16(FP)); %r9 replaces the callee-saved
 * BX. The backward copy sets the direction flag; System V requires it
 * clear on return, so _return clears it (the original left it set).
 */
.globl memmove
memmove:
	movq	%rdi, %rax		/* return value */
	movq	%rdx, %r9
	cmpq	$0, %r9
	jg	1f
	je	9f			/* nothing to do if n == 0 */
	movl	$0, %esi		/* fault if n < 0 */

/*
 * check and set for backwards:
 *	(p2 < p1) && ((p2+n) > p1)
 */
1:
	cmpq	%rdi, %rsi
	jg	2f
	je	9f			/* nothing to do if p2 == p1 */
	movq	%rsi, %rdx
	addq	%r9, %rdx
	cmpq	%rdi, %rdx
	jg	4f

/*
 * copy whole longs if aligned
 */
2:
	cld
	movq	%rsi, %rdx
	orq	%rdi, %rdx
	andl	$3, %edx
	jne	3f
	movq	%r9, %rcx
	shrq	$2, %rcx
	andl	$3, %r9d
	rep; movsl

/*
 * copy the rest, by bytes
 */
	je	9f			/* flags set by above ANDL */
3:
	movq	%r9, %rcx
	rep; movsb

	ret

/*
 * whole thing backwards has
 * adjusted addresses
 */
4:
	addq	%r9, %rdi
	addq	%r9, %rsi
	std
	subq	$4, %rdi
	subq	$4, %rsi
/*
 * copy whole longs, if aligned
 */
	movq	%rdi, %rdx
	orq	%rsi, %rdx
	andl	$3, %edx
	jne	5f
	movq	%r9, %rcx
	shrq	$2, %rcx
	andl	$3, %r9d
	rep; movsl
/*
 * copy the rest, by bytes
 */
	je	9f			/* flags set by above ANDL */

5:
	addq	$3, %rdi
	addq	$3, %rsi
	movq	%r9, %rcx
	rep; movsb

9:
	cld
	ret
