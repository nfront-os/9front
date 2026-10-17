/* GCC port: GAS/psABI version of memset.s: void* memset(void *p, int c, usize n) */
.globl memset
memset:
	mv	t0, a0
	andi	a1, a1, 0xff
1:	beqz	a2, 9f
	andi	t1, t0, 7
	beqz	t1, 2f
	sb	a1, 0(t0)
	addi	t0, t0, 1
	addi	a2, a2, -1
	j	1b
2:	slli	t1, a1, 8		/* c in every byte */
	or	a1, a1, t1
	slli	t1, a1, 16
	or	a1, a1, t1
	slli	t1, a1, 32
	or	a1, a1, t1
	li	t1, 8
3:	bltu	a2, t1, 4f
	sd	a1, 0(t0)
	addi	t0, t0, 8
	addi	a2, a2, -8
	j	3b
4:	beqz	a2, 9f
	sb	a1, 0(t0)
	addi	t0, t0, 1
	addi	a2, a2, -1
	j	4b
9:	ret
