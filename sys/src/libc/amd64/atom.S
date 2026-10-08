/*
 * GCC port: GAS/System V translation. The pointer argument (6c: RARG)
 * is %rdi, the further arguments arrive in %esi/%rsi and %edx/%rdx
 * instead of exp+8(FP)/new+16(FP). BX is callee-saved under System V,
 * so the scratch register is %ecx. Logic unchanged.
 */

/*
 * long ainc(long*), long adec(long*) (libc.h): 64-bit operations, gcc's
 * long is 64 bits -- the 32-bit ones left the upper half alone and
 * returned a negative count zero-extended (libthread decref).
 */
.globl ainc			/* long ainc(long *); */
ainc:
1:
	movq	(%rdi), %rax	/* exp */
	movq	%rax, %rcx
	incq	%rcx		/* new */
	lock; cmpxchgq %rcx, (%rdi)
	jnz	1b
	movq	%rcx, %rax
	ret

.globl adec			/* long adec(long*); */
adec:
1:
	movq	(%rdi), %rax
	movq	%rax, %rcx
	decq	%rcx
	lock; cmpxchgq %rcx, (%rdi)
	jnz	1b
	movq	%rcx, %rax
	ret

/*
 * int cas32(u32int *p, u32int ov, u32int nv);
 * int cas(uint *p, int ov, int nv);
 * int casl(ulong *p, ulong ov, ulong nv);
 */

.globl cas32, cas, casul, casl
cas32:
cas:
casul:
casl:				/* back compat */
	movl	%esi, %eax
	lock; cmpxchgl %edx, (%rdi)
	movl	$1, %eax		/* use CMOVLEQ etc. here? */
	jnz	1f
	ret
1:
	decl	%eax
	ret

/*
 * int cas64(u64int *p, u64int ov, u64int nv);
 * int casp(void **p, void *ov, void *nv);
 */

.globl cas64, casp
cas64:
casp:
	movq	%rsi, %rax
	lock; cmpxchgq %rdx, (%rdi)
	movl	$1, %eax		/* use CMOVLEQ etc. here? */
	jnz	1f
	ret
1:
	decl	%eax
	ret

.globl fas64, fasp
fas64:
fasp:
	movq	%rsi, %rax
	lock; xchgq	%rax, (%rdi)
	ret

.globl fas32
fas32:
	movl	%esi, %eax
	lock; xchgl	%eax, (%rdi)
	ret
