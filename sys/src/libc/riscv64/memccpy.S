/* GCC port: GAS/psABI version of memccpy.s: void* memccpy(void *s1, void *s2, int c, usize n) */
.globl memccpy
memccpy:
	andi	a2, a2, 0xff
1:	beqz	a3, 8f
	lbu	t0, 0(a1)
	sb	t0, 0(a0)
	addi	a1, a1, 1
	addi	a0, a0, 1
	addi	a3, a3, -1
	bne	t0, a2, 1b
	ret				/* just past the copy of c */
8:	li	a0, 0
	ret
