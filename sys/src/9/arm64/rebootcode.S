/*
 * GCC port: GAS/AAPCS64 translation of rebootcode.s:
 * _start(entry, code, size), code and size in x1, w2 (7c: code+8(FP),
 * size+16(FP)). 7l's R28 = setSB is not needed.
 */
#include "mem.h"

.globl _start
_start:
	mov	x27, x0

	mov	w2, w2		// MOVWU size
	bic	x2, x2, #3
	add	x3, x1, x2

_copy:
	ldr	w4, [x1], #4
	str	w4, [x0], #4
	cmp	x3, x1
	b.ne	_copy

	bl	cachedwbinv
	bl	l2cacheuwbinv

	isb	sy
	mrs	x0, sctlr_el1
	ldr	x9, =(1<<0 | 1<<2 | 1<<12)
	bic	x0, x0, x9
	isb	sy
	msr	sctlr_el1, x0
	isb	sy

	dsb	nshst
	tlbi	vmalle1
	dsb	nsh
	isb	sy

	bl	cachedwbinv
	bl	cacheiinv

	mov	w0, #0
	mov	w1, #0
	mov	w2, #0
	mov	w3, #0

	mov	x30, x27

	ret
