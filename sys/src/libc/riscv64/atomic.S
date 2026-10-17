/*
 * GCC port: GAS translation of atomic.s; the long variants
 * (long agetl(Along*), ...) are 64-bit operations: gcc's long is 64
 * bits (as libc/arm64/atomic.S); jc's are 32-bit.
 */
.globl agetl
agetl:
	fence	rw, rw
	ld	a0, 0(a0)
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
	amoswap.d.aqrl	a0, a1, (a0)
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
	amoadd.d.aqrl	t0, a1, (a0)
	fence	rw, rw
	add	a0, t0, a1
	ret

.globl acasl
acasl:
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
