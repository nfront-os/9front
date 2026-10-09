/* GCC port: GAS translation of strlen.s; saves %edi (callee-saved under cdecl) */
.globl strlen
strlen:
	pushl	%edi
	movl	$0, %eax
	movl	$-1, %ecx
	cld
/*
 * look for end of string
 */
	movl	8(%esp), %edi		/* p */
	repne; scasb
	movl	%edi, %eax
	subl	8(%esp), %eax
	subl	$1, %eax
	popl	%edi
	ret
