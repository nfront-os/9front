/*
 * GCC port: GAS translation of atom.s, rv64a; the fences around
 * each operation as in the original.
 */
/* long ainc(long*), long adec(long*): the new value */
.globl ainc
ainc:
	li	t0, 1
	j	1f
.globl adec
adec:
	li	t0, -1
1:	fence	rw, rw
	amoadd.w.aqrl	t1, t0, (a0)
	fence	rw, rw
	addw	a0, t1, t0
	ret

/* int cas(uint *p, int ov, int nv): 1 if *p was ov and is now nv, else 0 */
.globl cas
cas:
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

/* int casp(void **p, void *ov, void *nv), int casv(uvlong*, uvlong, uvlong) */
.globl casp
.globl casv
casp:
casv:
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

/* long loadlinked(long*); int storecond(long*, long) */
.globl loadlinked
loadlinked:
	lr.w.aq	a0, (a0)
	ret

.globl storecond
storecond:
	fence	rw, rw
	sc.w.aqrl	t0, a1, (a0)
	seqz	a0, t0
	fence	rw, rw
	ret
