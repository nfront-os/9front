/*
 * GCC port: GAS/psABI version of memmove.s (not a translation: bytes,
 * then doublewords when both pointers are aligned alike):
 * void* memmove(void *s1, void *s2, usize n), memcpy the same
 */
.globl memmove
.globl memcpy
memmove:
memcpy:
	mv	t0, a0			/* dst */
	beq	a0, a1, 9f
	beqz	a2, 9f
	bltu	a1, a0, 5f		/* src below dst: backwards */
	/* forward */
	xor	t1, a0, a1
	andi	t1, t1, 7
	bnez	t1, 3f
1:	andi	t1, t0, 7		/* align */
	beqz	t1, 2f
	beqz	a2, 9f
	lbu	t2, 0(a1)
	sb	t2, 0(t0)
	addi	a1, a1, 1
	addi	t0, t0, 1
	addi	a2, a2, -1
	j	1b
2:	li	t1, 8
	bltu	a2, t1, 3f
	ld	t2, 0(a1)
	sd	t2, 0(t0)
	addi	a1, a1, 8
	addi	t0, t0, 8
	addi	a2, a2, -8
	j	2b
3:	beqz	a2, 9f
	lbu	t2, 0(a1)
	sb	t2, 0(t0)
	addi	a1, a1, 1
	addi	t0, t0, 1
	addi	a2, a2, -1
	j	3b
	/* backward */
5:	add	t0, a0, a2
	add	a1, a1, a2
6:	beqz	a2, 9f
	addi	a1, a1, -1
	addi	t0, t0, -1
	lbu	t2, 0(a1)
	sb	t2, 0(t0)
	addi	a2, a2, -1
	j	6b
9:	ret
