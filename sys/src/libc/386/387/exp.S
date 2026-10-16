/*
 * GCC port: GAS translation. The argument is where 8c left it (cdecl,
 * 4(%esp)); the result is in %st(0), as 8c returns it.
 * FSUBD F0, F1 (F1 -= F0) is GAS's fsubr %st, %st(1) (GAS swaps
 * fsub/fsubr for a %st(i) destination); FADDD $1.0, F0 adds 1.0 from
 * memory, as the original's constant.
 */
.globl exp
exp:
	fldl2e
	fmull	4(%esp)		/* now we want 2^ this number */

	fld	%st(0)		/* x, x */
	frndint			/* ix, x -- this is best in round mode */
	fsubr	%st, %st(1)	/* ix, fx */
	fxch	%st(1)		/* fx, ix */
	f2xm1			/* 2^fx-1, ix */
	faddl	one		/* 2^fx, ix */
	fscale			/* 2^(fx+ix), ix */
	fstp	%st(1)		/* 2^(fx+ix) == 2^x */
	ret

	.section .rodata
	.balign	8
one:
	.double	1.0
