/* GCC port: GAS/psABI version of strcpy.s: char* strcpy(char *s1, char *s2) */
.globl strcpy
strcpy:
	mv	t1, a0
1:	lbu	t0, 0(a1)
	sb	t0, 0(t1)
	addi	a1, a1, 1
	addi	t1, t1, 1
	bnez	t0, 1b
	ret
