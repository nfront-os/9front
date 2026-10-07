/*
 * GCC port: GAS/System V translation. The pointer argument (6c: RARG)
 * is %rdi, the further arguments arrive in %esi/%rsi and %edx/%rdx
 * instead of exp+8(FP)/new+16(FP). BX is callee-saved under System V,
 * so the scratch register is %ecx. Logic unchanged.
 */

.globl ainc			/* int ainc(int *); */
ainc:
1:
	movl	(%rdi), %eax	/* exp */
	movl	%eax, %ecx
	incl	%ecx		/* new */
	lock; cmpxchgl %ecx, (%rdi)
	jnz	1b
	movl	%ecx, %eax
	ret

.globl adec			/* int adec(int*); */
adec:
1:
	movl	(%rdi), %eax
	movl	%eax, %ecx
	decl	%ecx
	lock; cmpxchgl %ecx, (%rdi)
	jnz	1b
	movl	%ecx, %eax
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
