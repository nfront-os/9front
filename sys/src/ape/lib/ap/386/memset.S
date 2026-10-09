/*
 * GCC port: GAS translation of the 8a original. %ebx, %esi and %edi
 * are callee-saved under cdecl (scratch for 8c): they are saved on
 * entry, so the arguments are 12 bytes further up the stack.
 */
#define p	16(%esp)
#define c	20(%esp)
#define n	24(%esp)
.globl memset
memset:
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	cld
	movl	p, %edi
	movzbl	c, %eax
	movl	n, %ebx
/*
 * if not enough bytes, just set bytes
 */
	cmpl	$9, %ebx
	jbe	c3
/*
 * build word in AX
 */
	movb	%al, %ah
	movl	%eax, %ecx
	shll	$16, %ecx
	orl	%ecx, %eax
/*
 * set whole longs
 */
c1:
	movl	%ebx, %ecx
	shrl	$2, %ecx
	andl	$3, %ebx
	rep; stosl
/*
 * set the rest, by bytes
 */
c3:
	movl	%ebx, %ecx
	rep; stosb
ret:
	movl	p, %eax
	popl	%edi
	popl	%esi
	popl	%ebx
	ret
