/* GCC port: GAS/psABI version of memcmp.s: int memcmp(void *s1, void *s2, usize n), bytes unsigned */
.globl memcmp
memcmp:
1:	beqz	a2, 8f
	lbu	t0, 0(a0)
	lbu	t1, 0(a1)
	bne	t0, t1, 9f
	addi	a0, a0, 1
	addi	a1, a1, 1
	addi	a2, a2, -1
	j	1b
8:	li	a0, 0
	ret
9:	sltu	a0, t1, t0		/* 1 if s1's byte is greater */
	bnez	a0, 7f
	li	a0, -1
7:	ret
