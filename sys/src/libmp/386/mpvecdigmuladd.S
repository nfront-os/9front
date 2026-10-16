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
 *	the registers are:
 *		hi = DX		- constrained by hardware
 *		lo = AX		- constrained by hardware
 *		b+n = SI	- can't be BP
 *		p+n = DI	- can't be BP
 *		i-n = BP
 *		m = BX
 *		oldhi = CX
 */
/*
 * GCC port: GAS translation. The arguments are where 8c left them
 * (cdecl, 4(%esp) up); the original used BX, SI, DI, BP freely, which
 * cdecl has the callee save: they are pushed first, the arguments are
 * then 16 bytes further up (a+0(FP) is 20(%esp)).
 */
.globl mpvecdigmuladd
mpvecdigmuladd:
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	pushl	%ebp
	movl	20(%esp), %esi		/* b */
	movl	24(%esp), %ecx		/* n */
	movl	28(%esp), %ebx		/* m */
	movl	32(%esp), %edi		/* p */
	movl	%ecx, %ebp
	negl	%ebp			/* BP = -n */
	shll	$2, %ecx
	addl	%ecx, %esi		/* SI = b + n */
	addl	%ecx, %edi		/* DI = p + n */
	xorl	%ecx, %ecx
1:
	movl	(%esi,%ebp,4), %eax	/* lo = b[i] */
	mull	%ebx			/* hi, lo = b[i] * m */
	addl	%ecx, %eax		/* lo += oldhi */
	adcl	$0, %edx		/* hi += carry */
	addl	%eax, (%edi,%ebp,4)	/* p[i] += lo */
	adcl	$0, %edx		/* hi += carry */
	movl	%edx, %ecx		/* oldhi = hi */
	incl	%ebp			/* i++ */
	jnz	1b
	xorl	%eax, %eax
	addl	%ecx, (%edi,%ebp,4)	/* p[n] + oldhi */
	adcl	%eax, %eax		/* return carry out of p[n] */
	popl	%ebp
	popl	%edi
	popl	%esi
	popl	%ebx
	ret
