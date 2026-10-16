/*
 * GCC port: GAS/System V translation. The AES-NI instructions are
 * GAS's (the original encoded them as bytes); aesenc %xmmS, %xmmD is
 * the original's AESENC(S, D). aes_encrypt/aes_decrypt take rk, Nr,
 * in, out in %rdi, %esi, %rdx, %rcx (the original read Nr, in, out
 * from the stack and wrote out through DI, here %r9). The round keys
 * are 16-byte aligned (setupAESstate), as the aligned loads need.
 */
.globl aesni_init
aesni_init:
	pushq	%rbx			/* CPUID writes it, callee-saved */
	movl	$1, %eax
	cpuid
	xorl	%eax, %eax
	andl	$(1<<25), %ecx
	jz	1f

	/* override aes function pointers */
	leaq	AESencrypt(%rip), %rax
	movq	%rax, aes_encrypt(%rip)
	leaq	AESdecrypt(%rip), %rax
	movq	%rax, aes_decrypt(%rip)

	/* return setup function pointer */
	leaq	AESsetup(%rip), %rax
1:
	popq	%rbx
	ret

AESencrypt:
	movq	%rcx, %r9		/* ct */
	movl	%esi, %ecx		/* Nr */
	movq	%rdx, %rsi		/* pt */
	movdqa	(%rdi), %xmm0
	movdqu	(%rsi), %xmm7
	addq	$16, %rdi
	pxor	%xmm7, %xmm0
	cmpl	$12, %ecx
	jl	erounds10
	je	erounds12
erounds14:
	movdqa	0(%rdi), %xmm1
	movdqa	16(%rdi), %xmm2
	addq	$32, %rdi
	aesenc	%xmm1, %xmm0
	aesenc	%xmm2, %xmm0
erounds12:
	movdqa	0(%rdi), %xmm3
	movdqa	16(%rdi), %xmm4
	addq	$32, %rdi
	aesenc	%xmm3, %xmm0
	aesenc	%xmm4, %xmm0
erounds10:
	movdqa	0(%rdi), %xmm1
	movdqa	16(%rdi), %xmm2
	movdqa	32(%rdi), %xmm3
	movdqa	48(%rdi), %xmm4
	movdqa	64(%rdi), %xmm5
	movdqa	80(%rdi), %xmm6
	movdqa	96(%rdi), %xmm7
	aesenc	%xmm1, %xmm0
	movdqa	112(%rdi), %xmm1
	aesenc	%xmm2, %xmm0
	movdqa	128(%rdi), %xmm2
	aesenc	%xmm3, %xmm0
	movdqa	144(%rdi), %xmm3
	aesenc	%xmm4, %xmm0
	aesenc	%xmm5, %xmm0
	aesenc	%xmm6, %xmm0
	aesenc	%xmm7, %xmm0

	aesenc	%xmm1, %xmm0
	aesenc	%xmm2, %xmm0
	aesenclast	%xmm3, %xmm0
	movdqu	%xmm0, (%r9)
	ret

AESdecrypt:
	movq	%rcx, %r9		/* pt */
	movl	%esi, %ecx		/* Nr */
	movq	%rdx, %rsi		/* ct */
	movdqa	(%rdi), %xmm0
	movdqu	(%rsi), %xmm7
	addq	$16, %rdi
	pxor	%xmm7, %xmm0
	cmpl	$12, %ecx
	jl	drounds10
	je	drounds12
drounds14:
	movdqa	0(%rdi), %xmm1
	movdqa	16(%rdi), %xmm2
	addq	$32, %rdi
	aesdec	%xmm1, %xmm0
	aesdec	%xmm2, %xmm0
drounds12:
	movdqa	0(%rdi), %xmm3
	movdqa	16(%rdi), %xmm4
	addq	$32, %rdi
	aesdec	%xmm3, %xmm0
	aesdec	%xmm4, %xmm0
drounds10:
	movdqa	0(%rdi), %xmm1
	movdqa	16(%rdi), %xmm2
	movdqa	32(%rdi), %xmm3
	movdqa	48(%rdi), %xmm4
	movdqa	64(%rdi), %xmm5
	movdqa	80(%rdi), %xmm6
	movdqa	96(%rdi), %xmm7
	aesdec	%xmm1, %xmm0
	movdqa	112(%rdi), %xmm1
	aesdec	%xmm2, %xmm0
	movdqa	128(%rdi), %xmm2
	aesdec	%xmm3, %xmm0
	movdqa	144(%rdi), %xmm3
	aesdec	%xmm4, %xmm0
	aesdec	%xmm5, %xmm0
	aesdec	%xmm6, %xmm0
	aesdec	%xmm7, %xmm0

	aesdec	%xmm1, %xmm0
	aesdec	%xmm2, %xmm0
	aesdeclast	%xmm3, %xmm0
	movdqu	%xmm0, (%r9)
	ret

/*
 * erk, drk, key, nkey arrive in %rdi, %rsi, %rdx, %ecx; erk and drk
 * stay in %r8, %r9 over the calls of setupEnc*, which take rk, key in
 * %rdi, %rsi (the original passed key on the stack). The original's BX
 * is %r10 (callee-saved).
 */
AESsetup:
	movq	%rdi, %r8		/* erk */
	movq	%rsi, %r9		/* drk */
	movq	%rdx, %rsi		/* key */
	cmpl	$32, %ecx
	je	esetup256
	cmpl	$24, %ecx
	je	esetup192
	cmpl	$16, %ecx
	je	esetup128
	xorl	%eax, %eax
	ret
esetup256:
	call	setupEnc256
	jmp	dsetup
esetup192:
	call	setupEnc192
	jmp	dsetup
esetup128:
	call	setupEnc128
dsetup:
	movq	%r8, %rsi
	movq	%r9, %rdi

	movl	%eax, %r10d
	shll	$4, %r10d
	addq	%r10, %rsi

	movdqa	(%rsi), %xmm0
	movdqa	%xmm0, (%rdi)

	movdqa	-16(%rsi), %xmm1
	movdqa	-32(%rsi), %xmm2
	movdqa	-48(%rsi), %xmm3
	movdqa	-64(%rsi), %xmm4
	aesimc	%xmm1, %xmm1
	aesimc	%xmm2, %xmm2
	aesimc	%xmm3, %xmm3
	aesimc	%xmm4, %xmm4
	movdqa	%xmm1, 16(%rdi)
	movdqa	%xmm2, 32(%rdi)
	movdqa	%xmm3, 48(%rdi)
	movdqa	%xmm4, 64(%rdi)

	movdqa	-80(%rsi), %xmm1
	movdqa	-96(%rsi), %xmm2
	movdqa	-112(%rsi), %xmm3
	movdqa	-128(%rsi), %xmm4
	aesimc	%xmm1, %xmm1
	aesimc	%xmm2, %xmm2
	aesimc	%xmm3, %xmm3
	aesimc	%xmm4, %xmm4
	movdqa	%xmm1, 80(%rdi)
	movdqa	%xmm2, 96(%rdi)
	movdqa	%xmm3, 112(%rdi)
	movdqa	%xmm4, 128(%rdi)

	movdqa	-144(%rsi), %xmm1
	aesimc	%xmm1, %xmm1
	movdqa	%xmm1, 144(%rdi)

	cmpl	$10, %eax
	je	dsetupend
	
	movdqa	-160(%rsi), %xmm1
	movdqa	-176(%rsi), %xmm2
	aesimc	%xmm1, %xmm1
	aesimc	%xmm2, %xmm2
	movdqa	%xmm1, 160(%rdi)
	movdqa	%xmm2, 176(%rdi)

	cmpl	$12, %eax
	je	dsetupend

	movdqa	-192(%rsi), %xmm1
	movdqa	-208(%rsi), %xmm2
	aesimc	%xmm1, %xmm1
	aesimc	%xmm2, %xmm2
	movdqa	%xmm1, 192(%rdi)
	movdqa	%xmm2, 208(%rdi)
dsetupend:
	subq	%r10, %rsi
	addq	%r10, %rdi
	movdqa	(%rsi), %xmm0
	movdqa	%xmm0, (%rdi)
	ret

setupEnc128:
	movdqu	(%rsi), %xmm1
	movdqa	%xmm1, (%rdi)
	aeskeygenassist	$0x01, %xmm1, %xmm0
	call	rk128
	movdqa	%xmm1, 16(%rdi)
	aeskeygenassist	$0x02, %xmm1, %xmm0
	call	rk128
	movdqa	%xmm1, 32(%rdi)
	aeskeygenassist	$0x04, %xmm1, %xmm0
	call	rk128
	movdqa	%xmm1, 48(%rdi)
	aeskeygenassist	$0x08, %xmm1, %xmm0
	call	rk128
	movdqa	%xmm1, 64(%rdi)
	aeskeygenassist	$0x10, %xmm1, %xmm0
	call	rk128
	movdqa	%xmm1, 80(%rdi)
	aeskeygenassist	$0x20, %xmm1, %xmm0
	call	rk128
	movdqa	%xmm1, 96(%rdi)
	aeskeygenassist	$0x40, %xmm1, %xmm0
	call	rk128
	movdqa	%xmm1, 112(%rdi)
	aeskeygenassist	$0x80, %xmm1, %xmm0
	call	rk128
	movdqa	%xmm1, 128(%rdi)
	aeskeygenassist	$0x1b, %xmm1, %xmm0
	call	rk128
	movdqa	%xmm1, 144(%rdi)
	aeskeygenassist	$0x36, %xmm1, %xmm0
	call	rk128
	movdqa	%xmm1, 160(%rdi)
	movl	$10, %eax
	ret
rk128:
	pshufd	$0xff, %xmm0, %xmm0
	movdqa	%xmm1, %xmm2
	pslldq	$4, %xmm2
	pxor	%xmm2, %xmm1
	pslldq	$4, %xmm2
	pxor	%xmm2, %xmm1
	pslldq	$4, %xmm2
	pxor	%xmm2, %xmm1
	pxor	%xmm0, %xmm1
	ret
	
setupEnc192:
	movdqu	(%rsi), %xmm1
	movdqu	16(%rsi), %xmm2
	movdqa	%xmm1, (%rdi)
	movdqa	%xmm2, %xmm5
	aeskeygenassist	$0x01, %xmm2, %xmm0
	call	rk192
	shufpd	$0, %xmm1, %xmm5
	movdqa	%xmm5, 16(%rdi)
	movdqa	%xmm1, %xmm6
	shufpd	$1, %xmm2, %xmm6
	movdqa	%xmm6, 32(%rdi)
	aeskeygenassist	$0x02, %xmm2, %xmm0
	call	rk192
	movdqa	%xmm1, 48(%rdi)
	movdqa	%xmm2, %xmm5
	aeskeygenassist	$0x04, %xmm2, %xmm0
	call	rk192
	shufpd	$0, %xmm1, %xmm5
	movdqa	%xmm5, 64(%rdi)
	movdqa	%xmm1, %xmm6
	shufpd	$1, %xmm2, %xmm6
	movdqa	%xmm6, 80(%rdi)
	aeskeygenassist	$0x08, %xmm2, %xmm0
	call	rk192
	movdqa	%xmm1, 96(%rdi)
	movdqa	%xmm2, %xmm5
	aeskeygenassist	$0x10, %xmm2, %xmm0
	call	rk192
	shufpd	$0, %xmm1, %xmm5
	movdqa	%xmm5, 112(%rdi)
	movdqa	%xmm1, %xmm6
	shufpd	$1, %xmm2, %xmm6
	movdqa	%xmm6, 128(%rdi)
	aeskeygenassist	$0x20, %xmm2, %xmm0
	call	rk192
	movdqa	%xmm1, 144(%rdi)
	movdqa	%xmm2, %xmm5
	aeskeygenassist	$0x40, %xmm2, %xmm0
	call	rk192
	shufpd	$0, %xmm1, %xmm5
	movdqa	%xmm5, 160(%rdi)
	movdqa	%xmm1, %xmm6
	shufpd	$1, %xmm2, %xmm6
	movdqa	%xmm6, 176(%rdi)
	aeskeygenassist	$0x80, %xmm2, %xmm0
	call	rk192
	movdqa	%xmm1, 192(%rdi)
	movl	$12, %eax
	ret
rk192:
	pshufd	$0x55, %xmm0, %xmm0
	movdqu	%xmm1, %xmm4
	pslldq	$4, %xmm4
	pxor	%xmm4, %xmm1
	pslldq	$4, %xmm4
	pxor	%xmm4, %xmm1
	pslldq	$4, %xmm4
	pxor	%xmm4, %xmm1
	pxor	%xmm0, %xmm1
	pshufd	$0xff, %xmm1, %xmm0
	movdqu	%xmm2, %xmm4
	pslldq	$4, %xmm4
	pxor	%xmm4, %xmm2
	pxor	%xmm0, %xmm2
	ret
	
setupEnc256:
	movdqu	(%rsi), %xmm1
	movdqu	16(%rsi), %xmm2
	movdqa	%xmm1, (%rdi)
	movdqa	%xmm2, 16(%rdi)
	aeskeygenassist	$0x01, %xmm2, %xmm0
	call	rk256_a
	movdqa	%xmm1, 32(%rdi)
	aeskeygenassist	$0x00, %xmm1, %xmm0
	call	rk256_b
	movdqa	%xmm2, 48(%rdi)
	aeskeygenassist	$0x02, %xmm2, %xmm0
	call	rk256_a
	movdqa	%xmm1, 64(%rdi)
	aeskeygenassist	$0x00, %xmm1, %xmm0
	call	rk256_b
	movdqa	%xmm2, 80(%rdi)
	aeskeygenassist	$0x04, %xmm2, %xmm0
	call	rk256_a
	movdqa	%xmm1, 96(%rdi)
	aeskeygenassist	$0x00, %xmm1, %xmm0
	call	rk256_b
	movdqa	%xmm2, 112(%rdi)
	aeskeygenassist	$0x08, %xmm2, %xmm0
	call	rk256_a
	movdqa	%xmm1, 128(%rdi)
	aeskeygenassist	$0x00, %xmm1, %xmm0
	call	rk256_b
	movdqa	%xmm2, 144(%rdi)
	aeskeygenassist	$0x10, %xmm2, %xmm0
	call	rk256_a
	movdqa	%xmm1, 160(%rdi)
	aeskeygenassist	$0x00, %xmm1, %xmm0
	call	rk256_b
	movdqa	%xmm2, 176(%rdi)
	aeskeygenassist	$0x20, %xmm2, %xmm0
	call	rk256_a
	movdqa	%xmm1, 192(%rdi)
	aeskeygenassist	$0x00, %xmm1, %xmm0
	call	rk256_b
	movdqa	%xmm2, 208(%rdi)
	aeskeygenassist	$0x40, %xmm2, %xmm0
	call	rk256_a
	movdqa	%xmm1, 224(%rdi)
	movl	$14, %eax
	ret
rk256_a:
	pshufd	$0xff, %xmm0, %xmm0
	movdqa	%xmm1, %xmm4
	pslldq	$4, %xmm4
	pxor	%xmm4, %xmm1
	pslldq	$4, %xmm4
	pxor	%xmm4, %xmm1
	pslldq	$4, %xmm4
	pxor	%xmm4, %xmm1
	pxor	%xmm0, %xmm1
	ret
rk256_b:
	pshufd	$0xaa, %xmm0, %xmm0
	movdqa	%xmm2, %xmm4
	pslldq	$4, %xmm4
	pxor	%xmm4, %xmm2
	pslldq	$4, %xmm4
	pxor	%xmm4, %xmm2
	pslldq	$4, %xmm4
	pxor	%xmm4, %xmm2
	pxor	%xmm0, %xmm2
	ret
