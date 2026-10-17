/* GCC port: GAS translation of atomic64.s */
.globl agetv
agetv:
	fence	rw, rw
	ld	a0, 0(a0)
	fence	rw, rw
	ret

.globl aswapv
aswapv:
	fence	rw, rw
	amoswap.d.aqrl	a0, a1, (a0)
	fence	rw, rw
	ret

.globl aincv
aincv:
	fence	rw, rw
	amoadd.d.aqrl	t0, a1, (a0)
	fence	rw, rw
	add	a0, t0, a1
	ret

.globl acasv
acasv:
	fence	rw, rw
1:	lr.d.aq	t0, (a0)
	bne	t0, a1, 2f
	sc.d.aqrl	t1, a2, (a0)
	bnez	t1, 1b
	fence	rw, rw
	li	a0, 1
	ret
2:	fence	rw, rw
	li	a0, 0
	ret
