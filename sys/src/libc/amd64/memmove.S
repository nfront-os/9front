/*
 * GCC port: GAS/System V version of memmove.s: p1, p2, n arrive in %rdi, %rsi,
 * %rdx (6c: RARG, p2+8(FP), n+16(FP)).  As memmove.s it copies overlapping
 * memory either way and faults on a negative n; unlike it, it copies whole
 * quadwords at any alignment (musl's way: the destination aligned, then
 * rep movsq whatever the source's alignment; the backward copy so from the
 * end), not longs only when both are aligned and bytes else (through a
 * device's BAR, as a GPU's memory, each byte is an uncached access).  The
 * direction flag is clear on return, as System V requires.
 */
.globl memmove
memmove:
	movq	%rdi, %rax		/* return value */
	testq	%rdx, %rdx
	jg	1f
	je	9f			/* nothing to do if n == 0 */
	xorl	%esi, %esi		/* fault if n < 0 */
	movb	(%rsi), %cl

/*
 * backwards if p1 is in p2's n bytes: 0 < p1-p2 < n (unsigned)
 */
1:
	movq	%rdi, %rcx
	subq	%rsi, %rcx
	je	9f			/* nothing to do if p2 == p1 */
	cmpq	%rdx, %rcx
	jb	4f

/*
 * forwards: bytes until p1 is aligned, quadwords, the rest by bytes
 */
	cmpq	$8, %rdx
	jb	3f
2:
	testl	$7, %edi
	jz	2f
	movsb
	decq	%rdx
	jmp	2b
2:
	movq	%rdx, %rcx
	shrq	$3, %rcx
	rep; movsq
	andl	$7, %edx
3:
	movq	%rdx, %rcx
	rep; movsb
9:
	ret

/*
 * backwards, from the last byte: bytes until p1+1 is aligned, quadwords,
 * the rest by bytes
 */
4:
	std
	leaq	-1(%rdi,%rdx), %rdi
	leaq	-1(%rsi,%rdx), %rsi
	cmpq	$8, %rdx
	jb	6f
5:
	leaq	1(%rdi), %rcx
	testl	$7, %ecx
	jz	5f
	movsb
	decq	%rdx
	jmp	5b
5:
	subq	$7, %rdi
	subq	$7, %rsi
	movq	%rdx, %rcx
	shrq	$3, %rcx
	rep; movsq
	addq	$7, %rdi
	addq	$7, %rsi
	andl	$7, %edx
6:
	movq	%rdx, %rcx
	rep; movsb
	cld
	ret
