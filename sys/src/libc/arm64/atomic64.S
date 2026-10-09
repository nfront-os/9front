/* GCC port: GAS/AAPCS64 translation of atomic64.s (arguments in x1, x2) */

/* get variants */
.globl agetv
agetv:
	ldar	x0, [x0]
	dmb	ish
	ret

/* set variants */
.globl aswapv
aswapv:
	mov	x2, x0
1:	ldxr	x0, [x2]
	stxr	w3, x1, [x2]
	cbnz	w3, 1b
	dmb	ish
	ret

/* inc variants */
.globl aincv
aincv:
	mov	x2, x0
1:	ldxr	x0, [x2]
	add	x3, x0, x1
	stxr	w4, x3, [x2]
	cbnz	w4, 1b
	dmb	ish
	mov	x0, x3
	ret

/* cas variants */
.globl acasv
acasv:
	dmb	ish
1:	ldxr	x3, [x0]
	cmp	x3, x1
	b.ne	2f
	stxr	w4, x2, [x0]
	cbnz	w4, 1b
	mov	x0, #1
	dmb	ish
	ret
2:	clrex
	mov	x0, #0
	dmb	ish
	ret
