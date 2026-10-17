/*
 * GCC port: GAS/psABI translation of main9p.s, _mainp as main9.S's
 * _main. _callpc/_savearg/_saveret serve port/profile.c's jc -p
 * instrumentation, which gcc does not emit.
 */
.globl _mainp
_mainp:
	lla	t0, _tos
	sd	s0, 0(t0)
	ld	a1, 0(sp)		/* argc */
	addi	a2, sp, 8		/* argv */
	andi	sp, sp, -16
	lla	a0, _profmain
	li	s0, 0
	li	ra, 0
	call	_callmain

.globl _callpc
.globl _saveret
.globl _savearg
_callpc:
	ld	a0, 8(a0)
_saveret:
_savearg:
	ret
