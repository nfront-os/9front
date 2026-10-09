/*
 * GCC port: GAS/AAPCS64 translation of atomic.s: the further arguments
 * arrive in x1, x2 instead of 0x08(FP)/0x10(FP). The Along variants
 * (long agetl(Along*), ...) are 64-bit operations: gcc's long is 64
 * bits (as libc/amd64/atomic.S); 7c's were 32-bit (W).
 */

/* get variants */
.globl agetl
agetl:
	ldar	x0, [x0]
	dmb	ish
	ret

.globl agetp
agetp:
	ldar	x0, [x0]
	dmb	ish
	ret

/* set variants */
.globl aswapl
aswapl:
	mov	x2, x0
1:	ldxr	x0, [x2]
	stxr	w3, x1, [x2]
	cbnz	w3, 1b
	dmb	ish
	ret

.globl aswapp
aswapp:
	mov	x2, x0
1:	ldxr	x0, [x2]
	stxr	w3, x1, [x2]
	cbnz	w3, 1b
	dmb	ish
	ret

/* inc variants */
.globl aincl
aincl:
	mov	x2, x0
1:	ldxr	x0, [x2]
	add	x3, x0, x1
	stxr	w4, x3, [x2]
	cbnz	w4, 1b
	dmb	ish
	mov	x0, x3
	ret

/* cas variants */
.globl acasl
acasl:
	dmb	ish
1:	ldxr	x3, [x0]
	cmp	x3, x1
	b.ne	2f
	stxr	w4, x2, [x0]
	cbnz	w4, 1b
	mov	x0, #1
	dmb	ish
	ret
2:	clrex
	mov	x0, #0
	dmb	ish
	ret

.globl acasp
acasp:
	dmb	ish
1:	ldxr	x3, [x0]
	cmp	x3, x1
	b.ne	2f
	stxr	w4, x2, [x0]
	cbnz	w4, 1b
	mov	x0, #1
	dmb	ish
	ret
2:	clrex
	mov	x0, #0
	dmb	ish
	ret

/* barriers */
.globl coherence
coherence:
	dmb	ish
	ret
