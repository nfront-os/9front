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
 *	the registers are:
 *		hi = DX		- constrained by hardware
 *		lo = AX		- constrained by hardware
 *		b = SI		- can't be BP
 *		p = DI		- can't be BP
 *		i = BP
 *		n = CX		- constrained by LOOP instr
 *		m = BX
 *		oldhi = 0(SP)
 */
/*
 * GCC port: GAS translation. The arguments are where 8c left them
 * (cdecl, 4(%esp) up); the original used BX, SI, DI, BP freely, which
 * cdecl has the callee save: they are pushed first, the arguments are
 * then 16 bytes further up (a+0(FP) is 20(%esp)).
 */
/* The original's frame ($4) for oldhi is 0(%esp) below the saved
 * registers: the arguments start at 24(%esp). */
.globl mpvecdigmulsub
mpvecdigmulsub:
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	pushl	%ebp
	subl	$4, %esp
	movl	24(%esp), %esi		/* b */
	movl	28(%esp), %ecx		/* n */
	movl	32(%esp), %ebx		/* m */
	movl	36(%esp), %edi		/* p */
	xorl	%ebp, %ebp
	movl	%ebp, 0(%esp)
1:
	movl	(%esi,%ebp,4), %eax	/* lo = b[i] */
	mull	%ebx			/* hi, lo = b[i] * m */
	addl	0(%esp), %eax		/* lo += oldhi */
	adcl	$0, %edx		/* hi += carry */
	subl	%eax, (%edi,%ebp,4)
	adcl	$0, %edx		/* hi += carry */
	movl	%edx, 0(%esp)
	incl	%ebp
	loop	1b
	movl	%ecx, %eax
	movl	0(%esp), %ebx
	subl	%ebx, (%edi,%ebp,4)
	sbbl	%ecx, %eax
	orl	$1, %eax
	addl	$4, %esp
	popl	%ebp
	popl	%edi
	popl	%esi
	popl	%ebx
	ret
