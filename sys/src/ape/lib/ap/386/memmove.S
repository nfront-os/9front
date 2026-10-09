/*
 * GCC port: GAS translation of the 8a original. %ebx, %esi and %edi
 * are callee-saved under cdecl (scratch for 8c): they are saved on
 * entry, so the arguments are 12 bytes further up the stack.
 * The backward copy leaves the direction flag set; cdecl requires it
 * clear on return, hence the cld there.
 */
#define p1	16(%esp)
#define p2	20(%esp)
#define n	24(%esp)
.globl memmove
memmove:
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	movl	p1, %edi
	movl	p2, %esi
	movl	n, %ebx
	cmpl	$0, %ebx
	jge	ok
	movl	$0, %esi
ok:
	cld
/*
 * check and set for backwards
 */
	cmpl	%edi, %esi
	jbe	back
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
	jmp	out
/*
 * whole thing backwards has
 * adjusted addresses
 */
back:
	addl	%ebx, %edi
	addl	%ebx, %esi
	subl	$4, %edi
	subl	$4, %esi
	std
/*
 * copy whole longs
 */
	movl	%ebx, %ecx
	shrl	$2, %ecx
	andl	$3, %ebx
	rep; movsl
/*
 * copy the rest, by bytes
 */
	addl	$3, %edi
	addl	$3, %esi
	movl	%ebx, %ecx
	rep; movsb
	cld
	movl	p1, %eax
out:
	popl	%edi
	popl	%esi
	popl	%ebx
	ret
