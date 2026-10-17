/* GCC port: GAS translation of tas.s (APE: tas) */
.globl tas
tas:
	li	t0, 1
	fence	rw, rw
	amoswap.w.aqrl	a0, t0, (a0)
	fence	rw, rw
	ret

.globl _barrier
_barrier:
	fence	iorw, iorw
	ret
