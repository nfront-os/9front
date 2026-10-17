/*
 * GCC port: GAS/psABI translation of main9.s. The kernel starts the
 * process with the Tos pointer in R8 (s0: jc's return register) and
 * argc, argv[0], ... on the stack. gcc's _callmain (9sys/callmain.c,
 * __GNUC__) takes argc and argv in registers; the stack is aligned to
 * 16 bytes and the frame chain ends here (s0, ra 0).
 */
.globl _main
_main:
	lla	t0, _tos
	sd	s0, 0(t0)
	ld	a1, 0(sp)		/* argc */
	addi	a2, sp, 8		/* argv */
	andi	sp, sp, -16
	lla	a0, main
	li	s0, 0
	li	ra, 0
	call	_callmain
