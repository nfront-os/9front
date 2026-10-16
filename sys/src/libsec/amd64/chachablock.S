/*
 * GCC port: GAS/System V translation: x, rounds arrive in %rdi, %esi
 * (the original read rounds from the stack); the xmm registers are
 * caller-saved.
 */
#define ROTATE(n, v1, v2) \
	movdqa	v1, v2; \
	pslld	$(n), v1; \
	psrld	$(32-n), v2; \
	por	v1, v2

.globl _chachablock
_chachablock:
	movdqu	 0(%rdi), %xmm0
	movdqu	16(%rdi), %xmm1
	movdqu	32(%rdi), %xmm2
	movdqu	48(%rdi), %xmm3

	movl	%esi, %ecx
	shrl	$1, %ecx

1:
	paddd	%xmm1, %xmm0
	pxor	%xmm0, %xmm3
	/* ROTATE(16, X3, X3) */
	pshuflw $(1<<0 | 0<<2 | 3<<4 | 2<<6), %xmm3, %xmm3
	pshufhw $(1<<0 | 0<<2 | 3<<4 | 2<<6), %xmm3, %xmm3

	paddd	%xmm3, %xmm2
	movdqa	%xmm1, %xmm4
	pxor	%xmm2, %xmm4
	ROTATE(12, %xmm4, %xmm1)

	paddd	%xmm1, %xmm0
	movdqa	%xmm0, %xmm4
	pxor	%xmm3, %xmm4
	ROTATE(8, %xmm4, %xmm3)

	paddd	%xmm3, %xmm2
	movdqa	%xmm1, %xmm4
	pxor	%xmm2, %xmm4
	ROTATE(7, %xmm4, %xmm1)

	pshufd $(1<<0 | 2<<2 | 3<<4 | 0<<6), %xmm1, %xmm1
	pshufd $(2<<0 | 3<<2 | 0<<4 | 1<<6), %xmm2, %xmm2
	pshufd $(3<<0 | 0<<2 | 1<<4 | 2<<6), %xmm3, %xmm3

	paddd	%xmm1, %xmm0
	pxor	%xmm0, %xmm3
	/* ROTATE(16, X3, X3) */
	pshuflw $(1<<0 | 0<<2 | 3<<4 | 2<<6), %xmm3, %xmm3
	pshufhw $(1<<0 | 0<<2 | 3<<4 | 2<<6), %xmm3, %xmm3

	paddd	%xmm3, %xmm2
	movdqa	%xmm1, %xmm4
	pxor	%xmm2, %xmm4
	ROTATE(12, %xmm4, %xmm1)

	paddd	%xmm1, %xmm0
	movdqa	%xmm0, %xmm4
	pxor	%xmm3, %xmm4
	ROTATE(8, %xmm4, %xmm3)

	paddd	%xmm3, %xmm2
	movdqa	%xmm1, %xmm4
	pxor	%xmm2, %xmm4
	ROTATE(7, %xmm4, %xmm1)

	pshufd $(3<<0 | 0<<2 | 1<<4 | 2<<6), %xmm1, %xmm1
	pshufd $(2<<0 | 3<<2 | 0<<4 | 1<<6), %xmm2, %xmm2
	pshufd $(1<<0 | 2<<2 | 3<<4 | 0<<6), %xmm3, %xmm3

	decl	%ecx
	jne	1b

	movdqu	%xmm0, 0(%rdi)
	movdqu	%xmm1, 16(%rdi)
	movdqu	%xmm2, 32(%rdi)
	movdqu	%xmm3, 48(%rdi)
	ret
