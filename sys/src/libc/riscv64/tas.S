/* GCC port: GAS translation of tas.s: int _tas(int *keyp) */
.globl _tas
_tas:
	li	t0, 1
	fence	rw, rw
	amoswap.w.aqrl	a0, t0, (a0)
	fence	rw, rw
	ret
