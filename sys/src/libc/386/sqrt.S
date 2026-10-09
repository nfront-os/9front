/* GCC port: GAS translation of sqrt.s (double in and out on the x87 stack, as cdecl) */
.globl sqrt
sqrt:
	fldl	4(%esp)			/* a */
	ftst
	wait
	fstsw	%ax
	sahf
	jb	1f
	fsqrt
	ret
1:	fstp	%st(0)
	call	NaN
	ret
