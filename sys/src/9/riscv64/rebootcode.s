/*
 * reboot trampoline, copied to a page of its own and called at its
 * physical address (main.c: rebootjump) as
 *	main(entry, code, size, hartid, dtb)
 * with physical addresses: turn paging off, copy the new kernel to
 * entry and enter it as the firmware would (R10 hart id, R11 device tree).
 * Position independent: no static data.
 */
#include "mem.h"
#include "riscv.h"
#include <atom.h>

#define SFENCEVMA	WORD $(011<<25|0x73)

TEXT main(SB), 1, $-4
	MOV	code+XLEN(FP), R9
	MOVWU	size+(2*XLEN)(FP), R12
	MOV	hart+(3*XLEN)(FP), R10
	MOV	dtb+(4*XLEN)(FP), R11
	MOV	R0, CSR(SATP)
	SFENCEVMA
	MOV	R8, R13
_copy:
	BEQ	R12, _copied
	MOVBU	0(R9), R14
	MOVB	R14, 0(R13)
	ADD	$1, R9
	ADD	$1, R13
	ADD	$-1, R12
	JMP	_copy
_copied:
	FENCE
	FENCE_I
	JMP	(R8)
