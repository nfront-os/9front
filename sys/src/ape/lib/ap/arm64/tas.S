/* GCC port: GAS translation of tas.s (APE: tas) */
.globl tas
tas:
	mov	w2, #1
1:	ldxr	w1, [x0]
	stxr	w3, w2, [x0]
	cbnz	w3, 1b
	mov	w0, w1
	b	_barrier

.globl _barrier
_barrier:
	dmb	ish
	ret
