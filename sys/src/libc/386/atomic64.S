/*
 * GCC port: GAS translation of atomic64.s. 8c returns a vlong through
 * a hidden pointer in front of the arguments (r+0(FP)), cdecl returns
 * it in %edx:%eax: the argument offsets drop by 4 and the results go
 * to %edx:%eax. %ebx and %edi are saved (callee-saved under cdecl).
 */

/* get variants */
.globl agetv
agetv:
	movl	4(%esp), %ecx		/* p */
	subl	$8, %esp
	fldl	(%ecx)			/* one 64-bit load, as the original */
	fstpl	(%esp)
	movl	0(%esp), %eax
	movl	4(%esp), %edx
	addl	$8, %esp
	ret

/* set variants */
.globl aswapv
aswapv:
	pushl	%ebx
	pushl	%edi
	movl	12(%esp), %edi		/* p */
	movl	16(%esp), %ebx		/* nv */
	movl	20(%esp), %ecx
	movl	0(%edi), %eax
	movl	4(%edi), %edx
1:	lock; cmpxchg8b (%edi)
	jne	1b
	popl	%edi			/* old value in %edx:%eax */
	popl	%ebx
	ret

/* inc variants */
.globl aincv
aincv:
	pushl	%ebx
	pushl	%edi
	movl	12(%esp), %edi		/* p */
1:	movl	0(%edi), %eax
	movl	4(%edi), %edx
	movl	%eax, %ebx
	movl	%edx, %ecx
	addl	16(%esp), %ebx		/* v */
	adcl	20(%esp), %ecx
	lock; cmpxchg8b (%edi)
	jne	1b
	movl	%ebx, %eax		/* new value */
	movl	%ecx, %edx
	popl	%edi
	popl	%ebx
	ret

/* cas variants */
.globl acasv
acasv:
	pushl	%ebx
	pushl	%edi
	movl	12(%esp), %edi		/* p */
	movl	16(%esp), %eax		/* ov */
	movl	20(%esp), %edx
	movl	24(%esp), %ebx		/* nv */
	movl	28(%esp), %ecx
	lock; cmpxchg8b (%edi)
	popl	%edi
	popl	%ebx
	jne	fail64
	movl	$1, %eax
	ret
fail64:
	movl	$0, %eax
	ret
