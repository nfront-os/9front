.comm	_tos, 8, 8

/*
 * GCC port: GAS/psABI translation of main9.s, as libc/riscv64/main9.S
 * with _apemain: the Tos pointer in R8 (s0), argc, argv[0], ... on the
 * stack; gcc's _callmain (plan9/callmain.c) takes them in registers.
 */
.globl _main
_main:
	lla	t0, _tos
	sd	s0, 0(t0)
	ld	a1, 0(sp)		/* argc */
	addi	a2, sp, 8		/* argv */
	andi	sp, sp, -16
	lla	a0, _apemain
	li	s0, 0
	li	ra, 0
	call	_callmain
