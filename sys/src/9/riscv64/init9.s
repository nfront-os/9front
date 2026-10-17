/*
 * initcode's entry: the first text after its a.out header (touser).
 * init0() (main.c) leaves sp[0] = argv = &sp[1], sp[1] = "boot":
 * startboot(boot, argv) gets boot in R8 and argv from its 8(FP), which
 * is sp[0] when it is entered with sp-16 (FP is SP+8, as in main9.s).
 */
TEXT main(SB), 1, $-4
	MOV	$setSB(SB), R3
	ADD	$-16, R2
	MOV	$boot(SB), R8
	MOV	R0, R1
	JAL	R0, startboot(SB)
