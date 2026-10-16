/*
 *	mpvecdigmulsub(mpdigit *b, int n, mpdigit m, mpdigit *p)
 *
 *	p -= b*m
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
 *		b = R10
 *		p = R9
 *		i = SI		(the original's BP, callee-saved here)
 *		n = CX		- constrained by LOOP instr
 *		m = R11		(BX)
 *		oldhi = R8
 */
.globl mpvecdigmulsub
mpvecdigmulsub:
	movq	%rdi, %r10
	movq	%rcx, %r9
	movl	%edx, %r11d
	movl	%esi, %ecx
	xorl	%esi, %esi
	movl	%esi, %r8d
1:
	movl	(%r10,%rsi,4), %eax	/* lo = b[i] */
	mull	%r11d			/* hi, lo = b[i] * m */
	addl	%r8d, %eax		/* lo += oldhi */
	adcl	$0, %edx		/* hi += carry */
	subl	%eax, (%r9,%rsi,4)
	adcl	$0, %edx		/* hi += carry */
	movl	%edx, %r8d
	incl	%esi
	loop	1b
	movl	%ecx, %eax
	subl	%r8d, (%r9,%rsi,4)
	sbbq	%rcx, %rax
	orq	$1, %rax
	ret
