/*
 * GCC port: GAS/AAPCS64 translation of memmove.s: from and n arrive in
 * x1, x2 instead of from+8(FP)/n+16(FP). 7a's CMP a, b compares b
 * with a (cmp b, a).
 */
.globl memcpy
.globl memmove
memcpy:
memmove:
	cmp	x1, x0
	b.eq	_done
	b.lt	_backward

_forward:
	mov	x7, x0
	add	x3, x0, x2
	and	x4, x2, #~15
	cbz	x4, _floop1
	add	x4, x0, x4

_floop16:
	ldp	x5, x6, [x1], #16
	stp	x5, x6, [x7], #16
	cmp	x7, x4
	b.ne	_floop16

_floop1:
	cmp	x7, x3
	b.eq	_done
	ldrb	w5, [x1], #1
	strb	w5, [x7], #1
	b	_floop1

_done:
	ret

_backward:
	add	x1, x1, x2
	add	x3, x0, x2
	and	x4, x2, #~15
	cbz	x4, _bloop1
	sub	x4, x3, x4

_bloop16:
	ldp	x5, x6, [x1, #-16]!
	stp	x5, x6, [x3, #-16]!
	cmp	x3, x4
	b.ne	_bloop16

_bloop1:
	cmp	x3, x0
	b.eq	_done
	ldrb	w5, [x1, #-1]!
	strb	w5, [x3, #-1]!
	b	_bloop1
