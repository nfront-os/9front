GLOBL	_tos(SB), $XLEN

TEXT	_mainp(SB), 1, $-4
	MOV	$setSB(SB), R3
	MOV	R8, _tos(SB)
	ADD	$-(2*XLEN), R2
	MOV	$_profmain(SB), R8
	MOV	R0, R1
	JAL	R0, _callmain(SB)

TEXT	_callpc(SB), 1, $0
	MOV	argp-XLEN(FP), R8
TEXT	_saveret(SB), 1, $0
TEXT	_savearg(SB), 1, $0
	RET
