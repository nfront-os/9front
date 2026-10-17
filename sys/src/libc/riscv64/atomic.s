/*
 * 9front's atomic(2) operations on long (32 bits) and pointers,
 * rv64a. The fences around each operation make it sequentially
 * consistent, as in atom.s.
 */
#include <atom.h>

#define ARG	8

/* get variants */
TEXT agetl(SB), 1, $-4			/* long agetl(Along*) */
	FENCE_RW
	MOVW	0(R(ARG)), R(ARG)
	FENCE_RW
	RET

TEXT agetp(SB), 1, $-4			/* void* agetp(Aptr*) */
	FENCE_RW
	MOV	0(R(ARG)), R(ARG)
	FENCE_RW
	RET

/* set variants: return the old value */
TEXT aswapl(SB), 1, $-4			/* long aswapl(Along*, long) */
	MOVW	v+XLEN(FP), R9
	FENCE_RW
	AMOW(Amoswap, AQ|RL, 9, ARG, ARG)
	FENCE_RW
	RET

TEXT aswapp(SB), 1, $-4			/* void* aswapp(Aptr*, void*) */
	MOV	v+XLEN(FP), R9
	FENCE_RW
	AMOD(Amoswap, AQ|RL, 9, ARG, ARG)
	FENCE_RW
	RET

/* inc variants: return the new value */
TEXT aincl(SB), 1, $-4			/* long aincl(Along*, long) */
	MOVW	v+XLEN(FP), R9
	FENCE_RW
	AMOW(Amoadd, AQ|RL, 9, ARG, 10)
	FENCE_RW
	ADDW	R9, R10, R(ARG)
	RET

/* cas variants: 1 if *p was ov and is now nv, else 0 */
TEXT acasl(SB), 1, $-4			/* int acasl(Along*, long ov, long nv) */
	MOVW	ov+XLEN(FP), R12	/* sign-extended, as LR.W loads */
	MOVW	nv+(XLEN+4)(FP), R13
	FENCE_RW
_casl:
	LRW(ARG, 14)
	BNE	R12, R14, _caslf
	SCW(13, ARG, 14)
	BNE	R14, _casl
	FENCE_RW
	MOV	$1, R(ARG)
	RET
_caslf:
	FENCE_RW
	MOV	R0, R(ARG)
	RET

TEXT acasp(SB), 1, $-4			/* int acasp(Aptr*, void* ov, void* nv) */
	MOV	ov+XLEN(FP), R12
	MOV	nv+(2*XLEN)(FP), R13
	FENCE_RW
_casp:
	LRD(ARG, 14)
	BNE	R12, R14, _caspf
	SCD(13, ARG, 14)
	BNE	R14, _casp
	FENCE_RW
	MOV	$1, R(ARG)
	RET
_caspf:
	FENCE_RW
	MOV	R0, R(ARG)
	RET

/* barriers */
TEXT coherence(SB), 1, $-4
	FENCE
	RET
