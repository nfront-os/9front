/*
 * long callcheck(void *fn, long a, long b, long c, long d, long e)
 * The 386 counterpart of callcheck.s: calls fn(a, b, c, d, e) (cdecl)
 * with %ebx, %esi, %edi, %ebp holding known values and returns a bit
 * mask of those fn did not preserve (bit 0 ebx, 1 ebp, 2 esi, 3 edi)
 * or'ed with 0x40 if the direction flag was left set.
 */
	.globl	callcheck
callcheck:
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	pushl	%ebp
	pushl	40(%esp)		/* e */
	pushl	40(%esp)		/* d */
	pushl	40(%esp)		/* c */
	pushl	40(%esp)		/* b */
	pushl	40(%esp)		/* a */
	movl	40(%esp), %eax		/* fn */
	movl	$0x11111111, %ebx
	movl	$0x22222222, %ebp
	movl	$0x33333333, %esi
	movl	$0x44444444, %edi
	call	*%eax
	addl	$20, %esp
	xorl	%eax, %eax
	cmpl	$0x11111111, %ebx
	je	1f
	orl	$1, %eax
1:	cmpl	$0x22222222, %ebp
	je	1f
	orl	$2, %eax
1:	cmpl	$0x33333333, %esi
	je	1f
	orl	$4, %eax
1:	cmpl	$0x44444444, %edi
	je	1f
	orl	$8, %eax
1:	pushfl
	popl	%ecx
	testl	$0x400, %ecx
	je	1f
	orl	$64, %eax
	cld
1:	popl	%ebp
	popl	%edi
	popl	%esi
	popl	%ebx
	ret
