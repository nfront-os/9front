/*
 * GCC port: GAS translation of the 8a original. %ebx, %esi and %edi
 * are callee-saved under cdecl (scratch for 8c): they are saved on
 * entry, so the arguments are 12 bytes further up the stack.
 */
#define p1	16(%esp)
#define p2	20(%esp)
.globl strcpy
strcpy:
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	movl	$0, %eax
	movl	$-1, %ecx
	cld
/*
 * find end of second string
 */
	movl	p2, %edi
	repne; scasb
	movl	%edi, %ebx
	subl	p2, %ebx
/*
 * copy the memory
 */
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
	movl	p1, %eax
	popl	%edi
	popl	%esi
	popl	%ebx
	ret
