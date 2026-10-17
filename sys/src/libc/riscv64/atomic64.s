/*
 * 9front's atomic(2) operations on vlong, rv64a; see atomic.s.
 */
#include <atom.h>

#define ARG	8

TEXT agetv(SB), 1, $-4			/* vlong agetv(Avlong*) */
	FENCE_RW
	MOV	0(R(ARG)), R(ARG)
	FENCE_RW
	RET

TEXT aswapv(SB), 1, $-4			/* vlong aswapv(Avlong*, vlong) */
	MOV	v+XLEN(FP), R9
	FENCE_RW
	AMOD(Amoswap, AQ|RL, 9, ARG, ARG)
	FENCE_RW
	RET

TEXT aincv(SB), 1, $-4			/* vlong aincv(Avlong*, vlong) */
	MOV	v+XLEN(FP), R9
	FENCE_RW
	AMOD(Amoadd, AQ|RL, 9, ARG, 10)
	FENCE_RW
	ADD	R9, R10, R(ARG)
	RET

TEXT acasv(SB), 1, $-4			/* int acasv(Avlong*, vlong ov, vlong nv) */
	MOV	ov+XLEN(FP), R12
	MOV	nv+(2*XLEN)(FP), R13
	FENCE_RW
_casv:
	LRD(ARG, 14)
	BNE	R12, R14, _casvf
	SCD(13, ARG, 14)
	BNE	R14, _casv
	FENCE_RW
	MOV	$1, R(ARG)
	RET
_casvf:
	FENCE_RW
	MOV	R0, R(ARG)
	RET
