/*
 * GCC port: GAS translation. The argument is where 8c left it (cdecl,
 * 4(%esp)); the result is in %st(0), as 8c returns it.
 * The constants are the original's doubles, from memory.
 */
.globl log
log:
	fldl	ln2
	fldl	4(%esp)

	ftst
	fwait
	fstsw	%ax
	sahf
	jb	bad

	fyl2x
	ret

.globl log10
log10:
	fldl	log2
	fldl	4(%esp)

	ftst
	fwait
	fstsw	%ax
	sahf
	jb	bad

	fyl2x
	ret

bad:
	fstp	%st(0)
	fstp	%st(0)
	call	NaN
	ret

	.section .rodata
	.balign	8
ln2:
	.double	0.69314718055994530941
log2:
	.double	0.30102999566398119521
