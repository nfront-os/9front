/*
 * GCC port: GAS translation of strchr.s. %edx instead of %ebx and %edi
 * saved (callee-saved under cdecl, scratch for 8c).
 */
.globl strchr
strchr:
	pushl	%edi
	movl	8(%esp), %edi		/* s */
	movb	12(%esp), %al		/* c */
	cmpb	$0, %al
	je	l2
/*
 * char is not null
 */
l1:
	movb	(%edi), %dl
	cmpb	$0, %dl
	je	ret0
	addl	$1, %edi
	cmpb	%dl, %al
	jne	l1
	movl	%edi, %eax
	subl	$1, %eax
	popl	%edi
	ret
/*
 * char is null
 */
l2:
	movl	$-1, %ecx
	cld
	repne; scasb
	movl	%edi, %eax
	subl	$1, %eax
	popl	%edi
	ret
ret0:
	movl	$0, %eax
	popl	%edi
	ret
