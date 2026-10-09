/*
 * GCC port: GAS translation of the 8a original. %ebx, %esi and %edi
 * are callee-saved under cdecl (scratch for 8c): they are saved on
 * entry, so the arguments are 12 bytes further up the stack.
 * The backward copy leaves the direction flag set; cdecl requires it
 * clear on return, hence the cld at _return.
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
	movl	%edi, %eax		/* return value */
	movl	p2, %esi
	movl	n, %ebx
	cmpl	$0, %ebx
	jg	_ok
	je	_return			/* nothing to do if n == 0 */
	movl	$0, %esi		/* fault if n < 0 */
/*
 * check and set for backwards:
 *	(p2 < p1) && ((p2+n) > p1)
 */
_ok:
	cmpl	%edi, %esi
	jg	_forward
	je	_return			/* nothing to do if p2 == p1 */
	movl	%esi, %edx
	addl	%ebx, %edx
	cmpl	%edi, %edx
	jg	_back
/*
 * copy whole longs
 */
_forward:
	movl	%ebx, %ecx
	cld
	shrl	$2, %ecx
	andl	$3, %ebx
	rep; movsl
/*
 * copy the rest, by bytes
 */
	je	_return			/* flags set by above ANDL */
	movl	%ebx, %ecx
	rep; movsb
	jmp	_return
/*
 * whole thing backwards has
 * adjusted addresses
 */
_back:
	addl	%ebx, %edi
	addl	%ebx, %esi
	std
	subl	$4, %edi
	subl	$4, %esi
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
	je	_return			/* flags set by above ANDL */
	addl	$3, %edi
	addl	$3, %esi
	movl	%ebx, %ecx
	rep; movsb
_return:
	cld
	popl	%edi
	popl	%esi
	popl	%ebx
	ret
