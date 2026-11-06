/*
 * GCC port: GAS/psABI version of memmove.s:
 * void* memmove(void *s1, void *s2, usize n), memcpy the same.
 * Bytes until the destination is aligned, then doublewords (four a
 * round) if the source is aligned too; else doublewords made of two
 * aligned loads of the source shifted together (no misaligned access:
 * many harts trap on those), the rest by bytes.  Backwards (the
 * destination in the source's n bytes) the same from the end.
 * Loads never leave the doublewords holding the source's bytes.
 */
.globl memmove
.globl memcpy
memmove:
memcpy:
	mv	t0, a0			/* dst */
	beq	a0, a1, 9f
	beqz	a2, 9f
	sub	t1, a0, a1
	bltu	t1, a2, 20f		/* 0 < dst-src < n: backwards */

	/* forwards */
	li	t6, 16
	bltu	a2, t6, 8f
1:	andi	t1, t0, 7		/* align dst */
	beqz	t1, 2f
	lbu	t2, 0(a1)
	sb	t2, 0(t0)
	addi	a1, a1, 1
	addi	t0, t0, 1
	addi	a2, a2, -1
	j	1b
2:	andi	t1, a1, 7
	bnez	t1, 6f
	li	t6, 32			/* both aligned */
3:	bltu	a2, t6, 4f
	ld	a3, 0(a1)
	ld	a4, 8(a1)
	ld	a5, 16(a1)
	ld	a6, 24(a1)
	sd	a3, 0(t0)
	sd	a4, 8(t0)
	sd	a5, 16(t0)
	sd	a6, 24(t0)
	addi	a1, a1, 32
	addi	t0, t0, 32
	addi	a2, a2, -32
	j	3b
4:	li	t6, 8
5:	bltu	a2, t6, 8f
	ld	a3, 0(a1)
	sd	a3, 0(t0)
	addi	a1, a1, 8
	addi	t0, t0, 8
	addi	a2, a2, -8
	j	5b
6:	slli	t2, t1, 3		/* src k bytes past alignment: shifts 8k, 64-8k */
	li	t3, 64
	sub	t3, t3, t2
	andi	a3, a1, -8
	ld	a4, 0(a3)
	li	t6, 8
7:	bltu	a2, t6, 8f
	ld	a5, 8(a3)
	srl	a6, a4, t2
	sll	a7, a5, t3
	or	a6, a6, a7
	sd	a6, 0(t0)
	mv	a4, a5
	addi	a3, a3, 8
	addi	a1, a1, 8
	addi	t0, t0, 8
	addi	a2, a2, -8
	j	7b
8:	beqz	a2, 9f			/* the rest */
	lbu	t2, 0(a1)
	sb	t2, 0(t0)
	addi	a1, a1, 1
	addi	t0, t0, 1
	addi	a2, a2, -1
	j	8b
9:	ret

	/* backwards, from the ends */
20:	add	t0, a0, a2
	add	a1, a1, a2
	li	t6, 16
	bltu	a2, t6, 28f
21:	andi	t1, t0, 7		/* align the end of dst */
	beqz	t1, 22f
	addi	a1, a1, -1
	addi	t0, t0, -1
	lbu	t2, 0(a1)
	sb	t2, 0(t0)
	addi	a2, a2, -1
	j	21b
22:	andi	t1, a1, 7
	bnez	t1, 26f
	li	t6, 32			/* both aligned */
23:	bltu	a2, t6, 24f
	ld	a3, -8(a1)
	ld	a4, -16(a1)
	ld	a5, -24(a1)
	ld	a6, -32(a1)
	sd	a3, -8(t0)
	sd	a4, -16(t0)
	sd	a5, -24(t0)
	sd	a6, -32(t0)
	addi	a1, a1, -32
	addi	t0, t0, -32
	addi	a2, a2, -32
	j	23b
24:	li	t6, 8
25:	bltu	a2, t6, 28f
	ld	a3, -8(a1)
	sd	a3, -8(t0)
	addi	a1, a1, -8
	addi	t0, t0, -8
	addi	a2, a2, -8
	j	25b
26:	slli	t2, t1, 3		/* src's end k bytes past alignment */
	li	t3, 64
	sub	t3, t3, t2
	andi	a3, a1, -8
	ld	a4, 0(a3)
	li	t6, 8
27:	bltu	a2, t6, 28f
	ld	a5, -8(a3)
	srl	a6, a5, t2
	sll	a7, a4, t3
	or	a6, a6, a7
	sd	a6, -8(t0)
	mv	a4, a5
	addi	a3, a3, -8
	addi	a1, a1, -8
	addi	t0, t0, -8
	addi	a2, a2, -8
	j	27b
28:	beqz	a2, 9b			/* the rest */
	addi	a1, a1, -1
	addi	t0, t0, -1
	lbu	t2, 0(a1)
	sb	t2, 0(t0)
	addi	a2, a2, -1
	j	28b
