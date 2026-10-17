#define RARG R8

/* user-accessible CSRs */
#define CYCLO		0xc00

/*
 * void cycles(uvlong*): cycles since power up.
 * 9front: through the pointer, as libc.h declares it
 */
TEXT cycles(SB), 1, $-4
	MOV	CSR(CYCLO), R9
	MOV	R9, 0(RARG)
	RET
