/*
 * 9front: _main only sets _tos and enters _callmain(main, argc, arg0)
 * (../9sys/callmain.c) with argc and arg0 left in place on the initial
 * stack: jc passes the first argument in R8, the others from XLEN(FP),
 * and FP is SP+XLEN at entry, so SP-2*XLEN puts argc in the second
 * argument and arg0 in the third.
 */
TEXT	_main(SB), 1, $-4
	MOV	$setSB(SB), R3
	MOV	R8, _tos(SB)
	ADD	$-(2*XLEN), R2
	MOV	$main(SB), R8
	MOV	R0, R1
	JAL	R0, _callmain(SB)
