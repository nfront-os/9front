/* GCC port: GAS translation of memchr.s; saves %edi (callee-saved under cdecl) */
.globl memchr
memchr:
	pushl	%edi
	movl	16(%esp), %ecx		/* n */
	cmpl	$0, %ecx
	je	none
	movl	8(%esp), %edi		/* p */
	movzbl	12(%esp), %eax		/* c */
	cld
/*
 * SCASB is memchr instruction
 */
	repne; scasb
	je	found
none:
	movl	$0, %eax
	popl	%edi
	ret
found:
	movl	%edi, %eax
	subl	$1, %eax
	popl	%edi
	ret
