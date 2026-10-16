/*
 * GCC port: GAS translation. The argument is where 8c left it (cdecl,
 * 4(%esp)); the result is in %st(0), as 8c returns it.
 */
.globl sqrt
sqrt:
	fldl	4(%esp)

	ftst
	fwait
	fstsw	%ax
	sahf
	jb	bad

	fsqrt
	ret

bad:
	fstp	%st(0)
	call	NaN
	ret
