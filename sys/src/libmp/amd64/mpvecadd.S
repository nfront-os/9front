/*
 *	mpvecadd(mpdigit *a, int alen, mpdigit *b, int blen, mpdigit *sum)
 *
 *		sum[0:alen] = a[0:alen-1] + b[0:blen-1]
 *
 *	prereq: alen >= blen, sum has room for alen+1 digits
 *
 * GCC port: GAS/System V translation: a, alen, b, blen, sum arrive in
 * %rdi, %esi, %rdx, %ecx, %r8. The original's BX (b) and BP (index) are
 * callee-saved under System V: b is in %r10, the index in %r11.
 */
.globl mpvecadd
mpvecadd:
	movq	%rdx, %r10		/* b */
	movl	%esi, %edx
	movl	%ecx, %ecx		/* blen, zero-extended for LOOP */
	subl	%ecx, %edx
	xorl	%r11d, %r11d		/* this also sets carry to 0 */

	/* skip addition if b is zero */
	testl	%ecx, %ecx
	jz	2f

	/* sum[0:blen-1],carry = a[0:blen-1] + b[0:blen-1] */
1:
	movl	(%rdi,%r11,4), %eax
	adcl	(%r10,%r11,4), %eax
	movl	%eax, (%r8,%r11,4)
	incl	%r11d
	loop	1b

2:
	/* jump if alen > blen */
	incl	%edx
	movl	%edx, %ecx
	loop	4f

	/* sum[alen] = carry */
3:
	adcl	$0, %ecx
	movl	%ecx, (%r8,%r11,4)
	ret

	/* sum[blen:alen-1],carry = a[blen:alen-1] + 0 */
4:
	movl	(%rdi,%r11,4), %eax
	adcl	$0, %eax
	movl	%eax, (%r8,%r11,4)
	incl	%r11d
	loop	4b
	jmp	3b
