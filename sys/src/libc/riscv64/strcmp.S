/* GCC port: GAS/psABI version of strcmp.s: int strcmp(char *s1, char *s2), bytes unsigned */
.globl strcmp
strcmp:
1:	lbu	t0, 0(a0)
	lbu	t1, 0(a1)
	bne	t0, t1, 9f
	beqz	t0, 8f
	addi	a0, a0, 1
	addi	a1, a1, 1
	j	1b
8:	li	a0, 0
	ret
9:	sltu	a0, t1, t0
	bnez	a0, 7f
	li	a0, -1
7:	ret
