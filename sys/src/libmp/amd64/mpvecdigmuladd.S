/*
 *	mpvecdigmul(mpdigit *b, int n, mpdigit m, mpdigit *p)
 *
 *	p += b*m
 *
 *	each step look like:
 *		hi,lo = m*b[i]
 *		lo += oldhi + carry
 *		hi += carry
 *		p[i] += lo
 *		oldhi = hi
 *
 * GCC port: GAS/System V translation: b, n, m, p arrive in %rdi, %esi,
 * %edx, %rcx. The registers are:
 *		hi = DX		- constrained by hardware
 *		lo = AX		- constrained by hardware
 *		b+n = R10
 *		p+n = R8
 *		i-n = R11	(the original's BP, callee-saved here)
 *		m = R9		(BX)
 *		oldhi = CX
 */
.globl mpvecdigmuladd
mpvecdigmuladd:
	movq	%rdi, %r10
	movq	%rcx, %r8
	movl	%edx, %r9d
	movl	%esi, %ecx
	movq	%rcx, %r11
	negq	%r11			/* R11 = -n */
	shll	$2, %ecx
	addq	%rcx, %r10		/* R10 = b + n */
	addq	%rcx, %r8		/* R8 = p + n */
	xorl	%ecx, %ecx
1:
	movl	(%r10,%r11,4), %eax	/* lo = b[i] */
	mull	%r9d			/* hi, lo = b[i] * m */
	addl	%ecx, %eax		/* lo += oldhi */
	adcl	$0, %edx		/* hi += carry */
	addl	%eax, (%r8,%r11,4)	/* p[i] += lo */
	adcl	$0, %edx		/* hi += carry */
	movl	%edx, %ecx		/* oldhi = hi */
	incq	%r11			/* i++ */
	jnz	1b
	xorl	%eax, %eax
	addl	%ecx, (%r8,%r11,4)	/* p[n] + oldhi */
	adcl	%eax, %eax		/* return carry out of p[n] */
	ret
