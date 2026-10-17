/* GCC port: GAS/psABI version of strchr.s: char* strchr(char *s, int c) */
.globl strchr
strchr:
	andi	a1, a1, 0xff
1:	lbu	t0, 0(a0)
	beq	t0, a1, 9f
	beqz	t0, 8f
	addi	a0, a0, 1
	j	1b
8:	li	a0, 0
9:	ret
