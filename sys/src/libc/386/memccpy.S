/*
 * GCC port: GAS translation of the 8a original. %ebx, %esi and %edi
 * are callee-saved under cdecl (scratch for 8c): they are saved on
 * entry, so the arguments are 12 bytes further up the stack.
 */
#define p1	16(%esp)
#define p2	20(%esp)
#define c	24(%esp)
#define n	28(%esp)
.globl memccpy
memccpy:
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	movl	n, %ecx
	cmpl	$0, %ecx
	je	none
	movl	p2, %edi
	movzbl	c, %eax
	cld
/*
 * find the character in the second string
 */
	repne; scasb
	je	found
/*
 * if not found, set count to 'n'
 */
none:
	movl	$0, %eax
	movl	n, %ebx
	jmp	copy
/*
 * if found, set count to bytes thru character
 */
found:
	movl	%edi, %eax
	subl	p2, %eax
	movl	%eax, %ebx
	addl	p1, %eax
/*
 * copy the memory
 */
copy:
	movl	p1, %edi
	movl	p2, %esi
/*
 * copy whole longs
 */
	movl	%ebx, %ecx
	shrl	$2, %ecx
	rep; movsl
/*
 * copy the rest, by bytes
 */
	andl	$3, %ebx
	movl	%ebx, %ecx
	rep; movsb
	popl	%edi
	popl	%esi
	popl	%ebx
	ret
