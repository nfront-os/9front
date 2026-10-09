/*
 * GCC port: GAS translation of atom.s. %edx/%ecx instead of %ebx;
 * cas64 saves %ebx and %edi (callee-saved under cdecl, scratch for 8c).
 */
.globl ainc
ainc:					/* long ainc(long *); */
	movl	4(%esp), %edx		/* addr */
1:	movl	(%edx), %eax
	movl	%eax, %ecx
	incl	%ecx
	lock
	cmpxchgl %ecx, (%edx)
	jnz	1b
	movl	%ecx, %eax
	ret

.globl adec
adec:					/* long adec(long*); */
	movl	4(%esp), %edx		/* addr */
1:	movl	(%edx), %eax
	movl	%eax, %ecx
	decl	%ecx
	lock
	cmpxchgl %ecx, (%edx)
	jnz	1b
	movl	%ecx, %eax
	ret

/*
 * int cas32(u32int *p, u32int ov, u32int nv);
 * int cas(uint *p, int ov, int nv);
 * int casp(void **p, void *ov, void *nv);
 * int casl(ulong *p, ulong ov, ulong nv);
 */
.globl cas32
.globl cas
.globl casp
.globl casl
cas32:
cas:
casp:
casl:
	movl	4(%esp), %ecx		/* p */
	movl	8(%esp), %eax		/* ov */
	movl	12(%esp), %edx		/* nv */
	lock
	cmpxchgl %edx, (%ecx)
	jne	fail
	movl	$1, %eax
	ret
fail:
	movl	$0, %eax
	ret

/*
 * int cas64(u64int *p, u64int ov, u64int nv);
 */
.globl cas64
cas64:
	pushl	%ebx
	pushl	%edi
	movl	12(%esp), %edi		/* p */
	movl	16(%esp), %eax		/* ov */
	movl	20(%esp), %edx
	movl	24(%esp), %ebx		/* nv */
	movl	28(%esp), %ecx
	lock
	cmpxchg8b (%edi)
	popl	%edi
	popl	%ebx
	jne	fail
	movl	$1, %eax
	ret
