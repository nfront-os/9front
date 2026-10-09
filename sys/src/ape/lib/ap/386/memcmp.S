/*
 * GCC port: GAS translation of the 8a original. %ebx, %esi and %edi
 * are callee-saved under cdecl (scratch for 8c): they are saved on
 * entry, so the arguments are 12 bytes further up the stack.
 */
#define p1	16(%esp)
#define p2	20(%esp)
#define n	24(%esp)
.globl memcmp
memcmp:
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	movl	n, %ebx
	cmpl	$0, %ebx
	je	none
	movl	p1, %edi
	movl	p2, %esi
	cld
/*
 * first by longs
 */
	movl	%ebx, %ecx
	shrl	$2, %ecx
	repe; cmpsl
	jne	found
/*
 * then by bytes
 */
	andl	$3, %ebx
	movl	%ebx, %ecx
	repe; cmpsb
	jne	found1
none:
	movl	$0, %eax
	jmp	out
/*
 * if long found,
 * back up and look by bytes
 */
found:
	movl	$4, %ecx
	subl	%ecx, %edi
	subl	%ecx, %esi
	repe; cmpsb
found1:
	jbe	lt
	movl	$-1, %eax
	jmp	out
lt:
	movl	$1, %eax
out:
	popl	%edi
	popl	%esi
	popl	%ebx
	ret
