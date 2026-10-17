/* APE's _main, as ../../../../libc/riscv64/main9.s with _apemain */
GLOBL	_tos(SB), $XLEN

TEXT	_main(SB), 1, $-4
	MOV	$setSB(SB), R3
	MOV	R8, _tos(SB)
	ADD	$-(2*XLEN), R2
	MOV	$_apemain(SB), R8
	MOV	R0, R1
	JAL	R0, _callmain(SB)
