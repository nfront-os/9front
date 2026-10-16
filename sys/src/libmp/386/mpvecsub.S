/*
 *	mpvecsub(mpdigit *a, int alen, mpdigit *b, int blen, mpdigit *diff)
 *
 *		diff[0:alen-1] = a[0:alen-1] - b[0:blen-1]
 *
 *	prereq: alen >= blen, diff has room for alen digits
 */
/*
 * GCC port: GAS translation. The arguments are where 8c left them
 * (cdecl, 4(%esp) up); the original used BX, SI, DI, BP freely, which
 * cdecl has the callee save: they are pushed first, the arguments are
 * then 16 bytes further up (a+0(FP) is 20(%esp)).
 */
.globl mpvecsub
mpvecsub:
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	pushl	%ebp
	movl	20(%esp), %esi		/* a */
	movl	28(%esp), %ebx		/* b */
	movl	24(%esp), %edx		/* alen */
	movl	32(%esp), %ecx		/* blen */
	movl	36(%esp), %edi		/* diff */
	subl	%ecx, %edx
	xorl	%ebp, %ebp		/* this also sets carry to 0 */

	/* skip subraction if b is zero */
	testl	%ecx, %ecx
	jz	2f

	/* diff[0:blen-1],borrow = a[0:blen-1] - b[0:blen-1] */
1:
	movl	(%esi,%ebp,4), %eax
	sbbl	(%ebx,%ebp,4), %eax
	movl	%eax, (%edi,%ebp,4)
	incl	%ebp
	loop	1b

2:
	incl	%edx
	movl	%edx, %ecx
	loop	4f
3:
	popl	%ebp
	popl	%edi
	popl	%esi
	popl	%ebx
	ret

	/* diff[blen:alen-1] = a[blen:alen-1] - 0 */
4:
	movl	(%esi,%ebp,4), %eax
	sbbl	$0, %eax
	movl	%eax, (%edi,%ebp,4)
	incl	%ebp
	loop	4b
	jmp	3b
