/*
 * GCC port: GAS/System V translation: s, c arrive in %rdi, %esi; %dl
 * replaces the callee-saved BX.
 */
.globl strchr
strchr:
	movl	%esi, %eax
	cmpb	$0, %al
	je	2f

/*
 * char is not null
 */
1:
	movb	(%rdi), %dl
	cmpb	$0, %dl
	je	3f
	addq	$1, %rdi
	cmpb	%dl, %al
	jne	1b

	movq	%rdi, %rax
	subq	$1, %rax
	ret

/*
 * char is null
 */
2:
	movq	$-1, %rcx
	cld

	repne; scasb

	movq	%rdi, %rax
	subq	$1, %rax
	ret

3:
	movq	$0, %rax
	ret
