/* GCC port: GAS/psABI version of memchr.s: void* memchr(void *s, int c, usize n) */
.globl memchr
memchr:
	andi	a1, a1, 0xff
1:	beqz	a2, 8f
	lbu	t0, 0(a0)
	beq	t0, a1, 9f
	addi	a0, a0, 1
	addi	a2, a2, -1
	j	1b
8:	li	a0, 0
9:	ret
