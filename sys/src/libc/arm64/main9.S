/*
 * GCC port: GAS/AAPCS64 translation of main9.s. The kernel starts the
 * process with the Tos pointer in R0 and argc, argv[0], ... on the
 * stack. The original makes room for _callmain's 7c frame, whose
 * argument slots then are argc and arg0 with argv at &arg0; gcc's
 * _callmain (9sys/callmain.c, __GNUC__) takes argc and argv in
 * registers. 7c's SB register (R28) is not used by gcc code; the frame
 * chain ends here (fp, lr 0).
 */
.globl _main
_main:
	adrp	x9, _tos
	str	x0, [x9, :lo12:_tos]
	ldr	x1, [sp]		/* argc */
	add	x2, sp, #8		/* argv */
	mov	x9, sp
	and	x9, x9, #~15
	mov	sp, x9
	adrp	x0, main
	add	x0, x0, :lo12:main
	mov	x29, #0
	mov	x30, #0
	bl	_callmain
