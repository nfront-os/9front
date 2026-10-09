/*
 * GCC port: GAS/AAPCS64 translation of atom.s: the further arguments
 * arrive in x1, x2 instead of ov+8(FP)/nv+16(FP).
 * long ainc(long*), long adec(long*) (libc.h): 64-bit operations, gcc's
 * long is 64 bits (as libc/amd64/atom.S).
 *
 * int cas32(u32int *p, u32int ov, u32int nv);
 * int cas(uint *p, int ov, int nv);
 * int casl(ulong *p, ulong ov, ulong nv);
 */
.globl cas0
cas0:
_cas0:
	clrex
	mov	w0, #0
	ret

.globl cas32
.globl cas
.globl casl
cas32:
cas:
casl:
	mov	w1, w1			/* ov: MOVWU, zero-extended */
	mov	w2, w2			/* nv */
1:	ldxr	w3, [x0]
	cmp	x1, x3
	b.ne	_cas0
	stxr	w4, w2, [x0]
	cbnz	w4, 1b
	mov	w0, #1
	b	_barrier

.globl casp
casp:
1:	ldxr	x3, [x0]
	cmp	x1, x3
	b.ne	_cas0
	stxr	w4, x2, [x0]
	cbnz	w4, 1b
	mov	w0, #1
	b	_barrier

.globl _xinc
.globl ainc
_xinc:					/* void	_xinc(long *); */
ainc:					/* long ainc(long *); */
1:	ldxr	x3, [x0]
	add	x3, x3, #1
	stxr	w4, x3, [x0]
	cbnz	w4, 1b
	mov	x0, x3
	b	_barrier

.globl _xdec
.globl adec
_xdec:					/* long _xdec(long *); */
adec:					/* long adec(long *); */
1:	ldxr	x3, [x0]
	sub	x3, x3, #1
	stxr	w4, x3, [x0]
	cbnz	w4, 1b
	mov	x0, x3
	b	_barrier
