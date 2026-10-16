/*
 *	mpvecadd(mpdigit *a, int alen, mpdigit *b, int blen, mpdigit *sum)
 *
 *		sum[0:alen] = a[0:alen-1] + b[0:blen-1]
 *
 *	prereq: alen >= blen, sum has room for alen+1 digits
 */
/*
 * GCC port: GAS translation. The arguments are where 8c left them
 * (cdecl, 4(%esp) up); the original used BX, SI, DI, BP freely, which
 * cdecl has the callee save: they are pushed first, the arguments are
 * then 16 bytes further up (a+0(FP) is 20(%esp)).
 */
.globl mpvecadd
mpvecadd:
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	pushl	%ebp
	movl	24(%esp), %edx		/* alen */
	movl	32(%esp), %ecx		/* blen */
	movl	20(%esp), %esi		/* a */
	movl	28(%esp), %ebx		/* b */
	subl	%ecx, %edx
	movl	36(%esp), %edi		/* sum */
	xorl	%ebp, %ebp		/* this also sets carry to 0 */

	/* skip addition if b is zero */
	testl	%ecx, %ecx
	jz	2f

	/* sum[0:blen-1],carry = a[0:blen-1] + b[0:blen-1] */
1:
	movl	(%esi,%ebp,4), %eax
	adcl	(%ebx,%ebp,4), %eax
	movl	%eax, (%edi,%ebp,4)
	incl	%ebp
	loop	1b

2:
	/* jump if alen > blen */
	incl	%edx
	movl	%edx, %ecx
	loop	4f

	/* sum[alen] = carry */
3:
	adcl	$0, %ecx
	movl	%ecx, (%edi,%ebp,4)
	popl	%ebp
	popl	%edi
	popl	%esi
	popl	%ebx
	ret

	/* sum[blen:alen-1],carry = a[blen:alen-1] + 0 */
4:
	movl	(%esi,%ebp,4), %eax
	adcl	$0, %eax
	movl	%eax, (%edi,%ebp,4)
	incl	%ebp
	loop	4b
	jmp	3b
