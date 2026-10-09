/*
 * GCC port: GAS/AAPCS64 translation of cache.v8.s. The length argument
 * arrives in x1 (7c: len+8(FP)). The original keeps the outer return
 * address in R29 while the inner helper (cachedva, cachedsw) calls the
 * per-line operation; x29 is gcc's frame pointer (callee-saved), so it is
 * kept in x16 (IP0, caller-saved, untouched by the operations) instead.
 * Not on the stack: _start calls cachedwbinv before it has one.
 */

/*
 * instruction cache operations
 */
.globl cacheiinvse
cacheiinvse:
	mov	w2, w1		// MOVWU len
	add	x2, x2, x0

	mrs	x11, daif
	msr	daifset, #0x2
	mov	w10, #1
	msr	csselr_el1, x10
	isb	sy
	mrs	x4, ccsidr_el1

	and	w4, w4, #7
	add	w4, w4, #4	// log2(linelen)
	lsl	x10, x10, x4
	lsr	x0, x0, x4
	lsl	x0, x0, x4

_iinvse:
	ic	ivau, x0
	add	x0, x0, x10
	cmp	x2, x0
	b.gt	_iinvse
	dsb	nsh
	isb	sy
	msr	daif, x11
	ret

.globl cacheiinv
cacheiinv:
	ic	iallu
	dsb	nsh
	isb	sy
	ret

.globl cacheuwbinv
cacheuwbinv:
	stp	x29, x30, [sp, #-16]!
	mov	x29, sp
	bl	cachedwbinv
	bl	cacheiinv
	ldp	x29, x30, [sp], #16
	ret

/*
 * data cache operations
 */
.globl cachedwbse
cachedwbse:
	mov	x16, x30
	bl	cachedva
.globl dccvac
dccvac:
	dc	cvac, x0
	ret

.globl cacheduwbse
cacheduwbse:
	mov	x16, x30
	bl	cachedva
.globl dccvau
dccvau:
	dc	cvau, x0
	ret

.globl cachedinvse
cachedinvse:
	mov	x16, x30
	bl	cachedva
.globl dcivac
dcivac:
	dc	ivac, x0
	ret

.globl cachedwbinvse
cachedwbinvse:
	mov	x16, x30
	bl	cachedva
.globl dccivac
dccivac:
	dc	civac, x0
	ret

/* op is the instruction after the caller's bl; returns from the caller */
cachedva:
	mov	w2, w1		// MOVWU len
	mov	x1, x30		// op
	add	x2, x2, x0

	mrs	x11, daif
	msr	daifset, #0x2
	mov	w10, #0
	msr	csselr_el1, x10
	isb	sy
	mrs	x4, ccsidr_el1

	and	w4, w4, #7
	add	w4, w4, #4	// log2(linelen)
	mov	w10, #1
	lsl	x10, x10, x4
	lsr	x0, x0, x4
	lsl	x0, x0, x4

	dsb	sy
	isb	sy
_cachedva:
	blr	x1
	add	x0, x0, x10
	cmp	x2, x0
	b.gt	_cachedva
	dsb	sy
	isb	sy
	msr	daif, x11
	ret	x16

/*
 * l1 cache operations
 */
.globl cachedwb
cachedwb:
	mov	w0, #0
_cachedwb:
	mov	x16, x30
	bl	cachedsw
.globl dccsw
dccsw:
	dc	csw, x0
	ret

.globl cachedinv
cachedinv:
	mov	w0, #0
_cachedinv:
	mov	x16, x30
	bl	cachedsw
.globl dcisw
dcisw:
	dc	isw, x0
	ret

.globl cachedwbinv
cachedwbinv:
	mov	w0, #0
_cachedwbinv:
	mov	x16, x30
	bl	cachedsw
.globl dccisw
dccisw:
	dc	cisw, x0
	ret

/*
 * l2 cache operations
 */
.globl l2cacheuwb
l2cacheuwb:
	mov	w0, #1
	b	_cachedwb
.globl l2cacheuinv
l2cacheuinv:
	mov	w0, #1
	b	_cachedinv
.globl l2cacheuwbinv
l2cacheuwbinv:
	mov	w0, #1
	b	_cachedwbinv

.globl cachesize
cachesize:
	mrs	x11, daif
	msr	daifset, #0x2
	msr	csselr_el1, x0
	isb	sy
	mrs	x0, ccsidr_el1
	msr	daif, x11
	ret

/* op is the instruction after the caller's bl; returns from the caller */
cachedsw:
	mov	x1, x30		// op

	mrs	x11, daif
	msr	daifset, #0x2
	add	w8, w0, w0
	msr	csselr_el1, x8
	isb	sy
	mrs	x4, ccsidr_el1

	lsr	x7, x4, #3
	and	w7, w7, #1023	// lastway
	add	w5, w7, #1	// #ways

	lsr	x2, x4, #13
	and	w2, w2, #32767	// lastset
	add	w2, w2, #1	// #sets

	and	w4, w4, #7
	add	w4, w4, #4	// log2(linelen)

	mov	w3, #32		// wayshift = 32 - log2(#ways)
_countlog2ways:
	cbz	x7, _loop	// lastway == 0?
	lsr	x7, x7, #1	// lastway >>= 1
	sub	x3, x3, #1	// wayshift--
	b	_countlog2ways
_loop:
	dsb	sy
	isb	sy
_nextway:
	mov	w6, #0		// set
_nextset:
	lsl	x0, x7, x3	// way<<wayshift
	lsl	x9, x6, x4	// set<<log2(linelen)
	orr	w0, w0, w8	// level
	orr	w0, w0, w9	// setway

	blr	x1		// op(setway)

	add	w6, w6, #1	// set++
	cmp	w6, w2
	b.lt	_nextset

	add	w7, w7, #1	// way++
	cmp	w7, w5
	b.lt	_nextway

	dsb	sy
	isb	sy
	msr	daif, x11
	ret	x16
