/*
 * GCC port: GAS/AAPCS64 translation of memset.s: c and n arrive in
 * w1, x2 instead of c+8(FP)/n+16(FP).
 */
.globl memset
memset:
	and	x1, x1, #0xff		/* MOVBU c */
	mov	w2, w2			/* MOVWU n (APE) */

	mov	x5, x0
	add	x3, x0, x2
	and	x4, x2, #~15
	cbz	x4, _loop1
	add	x4, x0, x4

	orr	x1, x1, x1, lsl #8
	orr	x1, x1, x1, lsl #16
	orr	x1, x1, x1, lsl #32

_loop16:
	stp	x1, x1, [x5], #16
	cmp	x5, x4
	b.ne	_loop16

_loop1:
	cmp	x5, x3
	b.eq	_done

	strb	w1, [x5], #1
	b	_loop1

_done:
	ret
