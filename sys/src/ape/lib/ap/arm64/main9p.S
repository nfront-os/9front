.comm	_tos, 8, 8

/*
 * GCC port: GAS/AAPCS64 translation of main9p.s, _mainp as main9.S's
 * _main. _callpc/_savearg/_saveret serve port/profile.c's 7c -p
 * instrumentation, which gcc does not emit; _callpc keeps the original.
 */
.globl _mainp
_mainp:
	adrp	x9, _tos
	str	x0, [x9, :lo12:_tos]
	ldr	x1, [sp]		/* argc */
	add	x2, sp, #8		/* argv */
	mov	x9, sp
	and	x9, x9, #~15
	mov	sp, x9
	adrp	x0, _profmain
	add	x0, x0, :lo12:_profmain
	mov	x29, #0
	mov	x30, #0
	bl	_callmain

.globl _callpc
.globl _saveret
.globl _savearg
_callpc:
	ldr	x0, [sp]
_saveret:
_savearg:
	ret
