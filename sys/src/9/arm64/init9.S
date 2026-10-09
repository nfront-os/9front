/*
 * GCC port: GAS/AAPCS64 translation of init9.s. init0() (main.c) leaves
 * sp[0] = argv = &sp[1], sp[1] = "boot"; the original passes boot in R0
 * and leaves argv on the stack where 7c's startboot finds its second
 * argument. Under AAPCS64 both go in registers: startboot(boot, argv),
 * from a 16-byte aligned stack.
 */
.globl main
main:
	ldr	x1, [sp]		/* argv */
	adrp	x0, boot
	add	x0, x0, :lo12:boot
	mov	x9, sp
	and	x9, x9, #~15
	mov	sp, x9
	mov	x29, #0
	mov	x30, #0
	b	startboot
