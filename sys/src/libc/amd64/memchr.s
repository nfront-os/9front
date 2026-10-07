/*
 * GCC port: GAS/System V translation: p, c, n arrive in %rdi, %esi,
 * %rdx (p is already where SCASB wants it).
 */
.globl memchr
memchr:
	movq	%rdx, %rcx
	cmpq	$0, %rcx
	je	1f
	movzbl	%sil, %eax
	cld
/*
 * SCASB is memchr instruction
 */
	repne; scasb
	je	2f

1:
	movl	$0, %eax
	ret

2:
	movq	%rdi, %rax
	subq	$1, %rax
	ret
