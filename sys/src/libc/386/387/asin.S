/*
 * GCC port: GAS translation. The argument is where 8c left it (cdecl,
 * 4(%esp)); the result is in %st(0), as 8c returns it.
 * FSUBRDP F0, F1 (F1 = F0 - F1, pop) is GAS's fsubp %st, %st(1): GAS
 * swaps fsub/fsubr for a %st(i) destination (AT&T's historic syntax).
 */
.globl asin
asin:
	fldl	4(%esp)		/* a */
	fld	%st(0)		/* a,a */
	fmul	%st(0), %st	/* a*a,a */
	fld1			/* 1,a*a,a */
	fsubp	%st, %st(1)	/* 1-a*a,a */

	ftst
	fwait
	fstsw	%ax
	sahf
	jb	bad

	fsqrt			/* sqrt(1-a*a),a */
	fpatan			/* atan2(sqrt(1-a*a),a) */
	ret

.globl acos
acos:
	fldl	4(%esp)
	fld	%st(0)
	fmul	%st(0), %st
	fld1
	fsubp	%st, %st(1)

	ftst
	fwait
	fstsw	%ax
	sahf
	jb	bad

	fsqrt
	fxch	%st(1)		/* identical except this */
	fpatan
	ret

bad:
	fstp	%st(0)
	fstp	%st(0)
	call	NaN
	ret
