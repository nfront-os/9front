/*
 *	mpvecsub(mpdigit *a, int alen, mpdigit *b, int blen, mpdigit *diff)
 *
 *		diff[0:alen-1] = a[0:alen-1] - b[0:blen-1]
 *
 *	prereq: alen >= blen, diff has room for alen digits
 *
 * GCC port: GAS/System V translation: a, alen, b, blen, diff arrive in
 * %rdi, %esi, %rdx, %ecx, %r8. The original's BX (b) and BP (index) are
 * callee-saved under System V: b is in %r10, the index in %r11.
 */
.globl mpvecsub
mpvecsub:
	movq	%rdx, %r10		/* b */
	movl	%esi, %edx
	movl	%ecx, %ecx		/* blen, zero-extended for LOOP */
	subl	%ecx, %edx
	xorl	%r11d, %r11d		/* this also sets carry to 0 */

	/* skip subraction if b is zero */
	testl	%ecx, %ecx
	jz	2f

	/* diff[0:blen-1],borrow = a[0:blen-1] - b[0:blen-1] */
1:
	movl	(%rdi,%r11,4), %eax
	sbbl	(%r10,%r11,4), %eax
	movl	%eax, (%r8,%r11,4)
	incl	%r11d
	loop	1b

2:
	incl	%edx
	movl	%edx, %ecx
	loop	3f
	ret

	/* diff[blen:alen-1] = a[blen:alen-1] - 0 */
3:
	movl	(%rdi,%r11,4), %eax
	sbbl	$0, %eax
	movl	%eax, (%r8,%r11,4)
	incl	%r11d
	loop	3b
	ret
