/*
 * GCC port: GAS/psABI translation of rebootcode.s:
 *	main(entry, code, size, hartid, dtb)
 * in a0-a4, physical addresses; position independent
 */
.globl main
main:
	mv	t0, a0			/* entry */
	mv	t1, a1			/* code */
	slli	t2, a2, 32		/* size, a ulong */
	srli	t2, t2, 32
	mv	a0, a3			/* hart id */
	mv	a1, a4			/* device tree */
	csrw	satp, zero
	sfence.vma
	mv	t3, t0
1:	beqz	t2, 2f
	lbu	t4, 0(t1)
	sb	t4, 0(t3)
	addi	t1, t1, 1
	addi	t3, t3, 1
	addi	t2, t2, -1
	j	1b
2:	fence	iorw, iorw
	fence.i
	jr	t0
