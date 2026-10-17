/* GCC port: GAS translation of atomic.s (long is 32 bits) */
.globl agetl
agetl:
	fence	rw, rw
	lw	a0, 0(a0)
	fence	rw, rw
	ret

.globl agetp
agetp:
	fence	rw, rw
	ld	a0, 0(a0)
	fence	rw, rw
	ret

.globl aswapl
aswapl:
	fence	rw, rw
	amoswap.w.aqrl	a0, a1, (a0)
	fence	rw, rw
	ret

.globl aswapp
aswapp:
	fence	rw, rw
	amoswap.d.aqrl	a0, a1, (a0)
	fence	rw, rw
	ret

.globl aincl
aincl:
	fence	rw, rw
	amoadd.w.aqrl	t0, a1, (a0)
	fence	rw, rw
	addw	a0, t0, a1
	ret

.globl acasl
acasl:
	sext.w	a1, a1
	fence	rw, rw
1:	lr.w.aq	t0, (a0)
	bne	t0, a1, 2f
	sc.w.aqrl	t1, a2, (a0)
	bnez	t1, 1b
	fence	rw, rw
	li	a0, 1
	ret
2:	fence	rw, rw
	li	a0, 0
	ret

.globl acasp
acasp:
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

.globl coherence
coherence:
	fence	iorw, iorw
	ret
