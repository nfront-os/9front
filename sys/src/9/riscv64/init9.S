/*
 * GCC port: GAS/psABI translation of init9.s. init0() (main.c) leaves
 * sp[0] = argv = &sp[1], sp[1] = "boot"; the original passes boot in R8
 * and leaves argv on the stack where jc's startboot finds its second
 * argument. Under the psABI both go in registers: startboot(boot, argv),
 * from a 16-byte aligned stack.
 */
.globl main
main:
	ld	a1, 0(sp)		/* argv */
	lla	a0, boot
	andi	sp, sp, -16
	li	s0, 0
	li	ra, 0
	tail	startboot
