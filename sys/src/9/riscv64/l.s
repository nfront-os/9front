/*
 * riscv64 kernel assembler (jc's ja), qemu virt machine under OpenSBI.
 * Parts follow Geoff Collyer's 9k riscv64 port (rv/start.s, strap.s).
 *
 * Registers: R2 sp, R3 static base, R6 up, R7 m (jc's extern registers);
 * jc passes the first argument in R8 and returns in R8.
 * CSR(SSCRATCH) always holds this cpu's m.
 */
#include "mem.h"
#include "riscv.h"
#include <atom.h>

#define SRET		SYS $0x102
#define WFI		SYS $0x105
#define ECALL		SYS $0
#define SFENCEVMA	WORD $(011<<25|0x73)		/* sfence.vma zero, zero */
#define SFENCEVA(r)	WORD $(011<<25|(r)<<15|0x73)	/* sfence.vma r, zero */
#define SFENCEASID(r)	WORD $(011<<25|(r)<<20|0x73)	/* sfence.vma zero, r */

/* offsets in Mach, known to asm */
#define MPROC		16
#define MSAVE2		24
#define MSAVE9		32
#define MHARTID		56

/* the trap frame: two slots for trap()'s argument and link, the Ureg, padding */
#define UR(n)		(UREG+(n)*XLEN)	/* offset of Rn in the frame */
#define UPC		(UREG+0)
#define USTATUS		(UREG+32*XLEN)
#define UIE		(UREG+33*XLEN)
#define UCAUSE		(UREG+34*XLEN)
#define UTVAL		(UREG+35*XLEN)
#define UMODE		(UREG+36*XLEN)

#define PTEKRWX		(PTEVALID|PTEREAD|PTEWRITE|PTEEXEC|PTEKERNEL)

/*
 * the firmware enters here at PHYSKTZERO, paging off, with the hart id
 * in R10 and the device tree's physical address in R11. Until paging is
 * on everything is relative to the pc: R3 (setSB) is a physical address.
 */
TEXT _main(SB), 1, $-4
	MOV	R0, CSR(SIE)
	CSRRC	CSR(SSTATUS), $Sie, R0
	MOV	R0, CSR(SATP)
	MOV	$setSB(SB), R3
	MOV	R10, R20			/* hart id */
	MOV	R11, R21			/* device tree */

	/* clear bss */
	MOV	$edata(SB), R9
	MOV	$end(SB), R12
_zbss:
	BGEU	R12, R9, _zbssdone
	MOVB	R0, 0(R9)
	ADD	$1, R9
	JMP	_zbss
_zbssdone:

	/* clear the Machs and the page tables after end (mem.h) */
	MOV	$end(SB), R22
	ADD	$(BY2PG-1), R22
	SRL	$PGSHIFT, R22
	SLL	$PGSHIFT, R22			/* R22: MACHADDR(0) */
	MOV	$(MAXMACH*MACHSIZE), R13
	ADD	R22, R13			/* R13: L1 */
	ADD	$(2*BY2PG), R13, R12
	MOV	R22, R9
_zmach:
	BGEU	R12, R9, _zmachdone
	MOV	R0, 0(R9)
	ADD	$XLEN, R9
	JMP	_zmach
_zmachdone:

	/*
	 * boot page table: a gigapage for the first GB of ram at its
	 * physical address (to turn paging on), four at KZERO+ (the
	 * device tree is near the top of ram, bootargsinit reads it
	 * before meminit maps the rest); devices (the first GB) at
	 * KZERO; the level 1 table of VMAP.
	 */
	MOV	$PHYSDRAM, R14
	SRL	$PGSHIFT, R14
	SLL	$PTEPPNSHIFT, R14
	OR	$PTEKRWX, R14
	MOV	R14, (2*XLEN)(R13)		/* PTLX(PHYSDRAM, 2) */
	MOV	R14, (258*XLEN)(R13)		/* PTLX(KZERO+PHYSDRAM, 2) */
	MOV	$(GiB>>PGSHIFT<<PTEPPNSHIFT), R15
	ADD	R15, R14
	MOV	R14, (259*XLEN)(R13)
	ADD	R15, R14
	MOV	R14, (260*XLEN)(R13)
	ADD	R15, R14
	MOV	R14, (261*XLEN)(R13)
	MOV	$(PTEVALID|PTEREAD|PTEWRITE|PTEKERNEL), R14
	MOV	R14, (256*XLEN)(R13)		/* PTLX(KZERO, 2) */
	ADD	$BY2PG, R13, R14
	SRL	$PGSHIFT, R14
	SLL	$PTEPPNSHIFT, R14
	OR	$PTEVALID, R14
	MOV	R14, (384*XLEN)(R13)		/* PTLX(VMAP, 2) */

	SRL	$PGSHIFT, R13, R9
	MOV	$Sv39, R12
	OR	R12, R9
	SFENCEVMA
	MOV	R9, CSR(SATP)
	SFENCEVMA

	/*
	 * continue at KZERO+: jl makes $sym(SB) pc-relative, so the
	 * address is physical here; R3 is set again up there
	 */
	MOV	$KZERO, R12
	ADD	R12, R22
	MOV	$_virtual(SB), R9
	ADD	R12, R9
	JMP	(R9)

TEXT _virtual(SB), 1, $-4
	MOV	$setSB(SB), R3
	MOV	R22, R7				/* m = MACHADDR(0) */
	MOV	R0, R6				/* up = nil */
	ADD	$MACHSIZE, R7, R2
	MOV	R7, CSR(SSCRATCH)
	MOV	$strap(SB), R9
	MOV	R9, CSR(STVEC)
	MOV	R21, dtbpa(SB)
	MOVW	R20, MHARTID(R7)
	JAL	R1, main(SB)
_halt:
	WFI
	JMP	_halt

/*
 * secondary harts, started by sbihartstart(hart, PADDR(_secondary), PADDR(m)):
 * R10 is the hart id, R11 the physical address of its Mach, the
 * satp for the kernel's page table is in its regsave[0] (mpinit).
 */
TEXT _secondary(SB), 1, $-4
	MOV	R0, CSR(SIE)
	CSRRC	CSR(SSTATUS), $Sie, R0
	MOV	$setSB(SB), R3
	MOV	MSAVE2(R11), R9			/* satp: regsave[0] */
	SFENCEVMA
	MOV	R9, CSR(SATP)
	SFENCEVMA
	MOV	$KZERO, R12
	ADD	R12, R11
	MOV	$_virtual2(SB), R9
	ADD	R12, R9
	JMP	(R9)

TEXT _virtual2(SB), 1, $-4
	MOV	$setSB(SB), R3
	MOV	R11, R7
	MOV	R0, R6
	ADD	$MACHSIZE, R7, R2
	MOV	R7, CSR(SSCRATCH)
	MOV	$strap(SB), R9
	MOV	R9, CSR(STVEC)
	JAL	R1, main(SB)
	JMP	_halt

/*
 * trap and interrupt entry, from user or kernel mode: save everything
 * in a Ureg (on up's kernel stack, whose top is up, coming from user
 * mode; on the current stack from kernel mode) and call trap(ureg).
 */
TEXT strap(SB), 1, $-4
	CSRRW	CSR(SSCRATCH), R7, R7		/* R7 = m, sscratch = old R7 */
	MOV	R2, MSAVE2(R7)
	MOV	R9, MSAVE9(R7)
	MOV	CSR(SSTATUS), R9
	AND	$Spp, R9
	BNE	R9, _fromkernel
	MOV	MPROC(R7), R2			/* top of up's kernel stack */
_fromkernel:
	SRL	$4, R2
	SLL	$4, R2
	ADD	$-TRAPFRAME, R2
	MOV	R1, UR(1)(R2)
	MOV	R3, UR(3)(R2)
	MOV	R4, UR(4)(R2)
	MOV	R5, UR(5)(R2)
	MOV	R6, UR(6)(R2)
	MOV	R8, UR(8)(R2)
	MOV	R10, UR(10)(R2)
	MOV	R11, UR(11)(R2)
	MOV	R12, UR(12)(R2)
	MOV	R13, UR(13)(R2)
	MOV	R14, UR(14)(R2)
	MOV	R15, UR(15)(R2)
	MOV	R16, UR(16)(R2)
	MOV	R17, UR(17)(R2)
	MOV	R18, UR(18)(R2)
	MOV	R19, UR(19)(R2)
	MOV	R20, UR(20)(R2)
	MOV	R21, UR(21)(R2)
	MOV	R22, UR(22)(R2)
	MOV	R23, UR(23)(R2)
	MOV	R24, UR(24)(R2)
	MOV	R25, UR(25)(R2)
	MOV	R26, UR(26)(R2)
	MOV	R27, UR(27)(R2)
	MOV	R28, UR(28)(R2)
	MOV	R29, UR(29)(R2)
	MOV	R30, UR(30)(R2)
	MOV	R31, UR(31)(R2)
	MOV	MSAVE2(R7), R9
	MOV	R9, UR(2)(R2)
	MOV	MSAVE9(R7), R9
	MOV	R9, UR(9)(R2)
	MOV	CSR(SSCRATCH), R9
	MOV	R9, UR(7)(R2)
	MOV	R7, CSR(SSCRATCH)
	MOV	CSR(SEPC), R9
	MOV	R9, UPC(R2)
	MOV	CSR(SSTATUS), R9
	MOV	R9, USTATUS(R2)
	SRL	$8, R9
	AND	$1, R9
	MOV	R9, UMODE(R2)			/* 1: from supervisor mode */
	MOV	CSR(SIE), R9
	MOV	R9, UIE(R2)
	MOV	CSR(SCAUSE), R9
	MOV	R9, UCAUSE(R2)
	MOV	CSR(STVAL), R9
	MOV	R9, UTVAL(R2)

	MOV	$setSB(SB), R3
	MOV	MPROC(R7), R6			/* up = m->proc */
	ADD	$UREG, R2, R8
	JAL	R1, trap(SB)

/*
 * return from trap, fork (the child's frame made by forkchild) or
 * notify: restore from the Ureg at UREG(R2) and sret. The fpu state
 * (sstatus.FS) stays as fpukexit left it; returning to kernel mode,
 * m stays this cpu's (the process may have moved).
 */
TEXT forkret(SB), 1, $-4
TEXT noteret(SB), 1, $-4
	CSRRC	CSR(SSTATUS), $Sie, R0
	MOV	UPC(R2), R9
	MOV	R9, CSR(SEPC)
	MOV	USTATUS(R2), R9
	MOV	$~(Fsst|Sie), R10
	AND	R10, R9
	MOV	CSR(SSTATUS), R11
	MOV	$Fsst, R10
	AND	R10, R11
	OR	R11, R9
	MOV	$Sum, R10
	OR	R10, R9
	MOV	R9, CSR(SSTATUS)
	AND	$Spp, R9
	BNE	R9, _tokernel
	MOV	UR(7)(R2), R7
_tokernel:
	MOV	UR(1)(R2), R1
	MOV	UR(3)(R2), R3
	MOV	UR(4)(R2), R4
	MOV	UR(5)(R2), R5
	MOV	UR(6)(R2), R6
	MOV	UR(8)(R2), R8
	MOV	UR(9)(R2), R9
	MOV	UR(10)(R2), R10
	MOV	UR(11)(R2), R11
	MOV	UR(12)(R2), R12
	MOV	UR(13)(R2), R13
	MOV	UR(14)(R2), R14
	MOV	UR(15)(R2), R15
	MOV	UR(16)(R2), R16
	MOV	UR(17)(R2), R17
	MOV	UR(18)(R2), R18
	MOV	UR(19)(R2), R19
	MOV	UR(20)(R2), R20
	MOV	UR(21)(R2), R21
	MOV	UR(22)(R2), R22
	MOV	UR(23)(R2), R23
	MOV	UR(24)(R2), R24
	MOV	UR(25)(R2), R25
	MOV	UR(26)(R2), R26
	MOV	UR(27)(R2), R27
	MOV	UR(28)(R2), R28
	MOV	UR(29)(R2), R29
	MOV	UR(30)(R2), R30
	MOV	UR(31)(R2), R31
	MOV	UR(2)(R2), R2
	SRET

/*
 * to user mode for the first time (init0): sp in R8, pc at the start
 * of initcode's text (after its a.out header)
 */
TEXT touser(SB), 1, $-4
	MOV	$(Spp|Sie|Fsst), R9
	CSRRC	CSR(SSTATUS), R9, R0
	MOV	$(Spie|Sum), R9
	CSRRS	CSR(SSTATUS), R9, R0
	MOV	$(UTZERO+32), R9
	MOV	R9, CSR(SEPC)
	MOV	R8, R2
	MOV	R0, R1
	MOV	R0, R8
	SRET

TEXT setlabel(SB), 1, $-4
	MOV	R2, 0(R8)
	MOV	R1, XLEN(R8)
	MOV	R0, R8
	RET

TEXT gotolabel(SB), 1, $-4
	MOV	0(R8), R2
	MOV	XLEN(R8), R1
	MOV	$1, R8
	RET

TEXT islo(SB), 1, $-4
	MOV	CSR(SSTATUS), R8
	AND	$Sie, R8
	RET

TEXT splhi(SB), 1, $-4
	MOV	R1, XLEN(R7)			/* m->splpc */
	CSRRC	CSR(SSTATUS), $Sie, R8
	AND	$Sie, R8
	RET

TEXT spllo(SB), 1, $-4
	CSRRS	CSR(SSTATUS), $Sie, R8
	AND	$Sie, R8
	RET

TEXT splx(SB), 1, $-4
	BEQ	R8, _splxhi
	CSRRS	CSR(SSTATUS), $Sie, R0
	RET
_splxhi:
	MOV	R1, XLEN(R7)
	CSRRC	CSR(SSTATUS), $Sie, R0
	RET

TEXT tas(SB), 1, $-4
TEXT _tas(SB), 1, $-4
	MOV	$1, R9
	FENCE_RW
	AMOW(Amoswap, AQ|RL, 9, 8, 8)
	FENCE_RW
	RET

/* int cmpswap(long *p, long ov, long nv) */
TEXT cmpswap(SB), 1, $-4
	MOVW	ov+XLEN(FP), R12
	MOVW	nv+(XLEN+4)(FP), R13
	FENCE_RW
_cas:
	LRW(8, 14)
	BNE	R12, R14, _casfail
	SCW(13, 8, 14)
	BNE	R14, _cas
	FENCE_RW
	MOV	$1, R8
	RET
_casfail:
	FENCE_RW
	MOV	R0, R8
	RET

TEXT coherence(SB), 1, $-4
	FENCE
	RET

TEXT idlehands(SB), 1, $-4
	FENCE
	MOVW	nrdy(SB), R9
	BNE	R9, _ready
	WFI
_ready:
	RET

TEXT vcycles(SB), 1, $-4
	MOV	CSR(TIMELO), R8
	RET

TEXT setsatp(SB), 1, $-4
	SFENCEVMA
	MOV	R8, CSR(SATP)
	SFENCEVMA
	RET

TEXT flushtlb(SB), 1, $-4
	SFENCEVMA
	RET

TEXT flushtlbva(SB), 1, $-4
	SFENCEVA(8)
	RET

TEXT flushtlbasid(SB), 1, $-4
	SFENCEASID(8)
	RET

TEXT fencei(SB), 1, $-4
	FENCE_I
	RET

TEXT getsie(SB), 1, $-4
	MOV	CSR(SIE), R8
	RET

TEXT setsie(SB), 1, $-4
	MOV	R8, CSR(SIE)
	RET

TEXT getsip(SB), 1, $-4
	MOV	CSR(SIP), R8
	RET

TEXT setsip(SB), 1, $-4
	MOV	R8, CSR(SIP)
	RET

TEXT setstvec(SB), 1, $-4
	MOV	R8, CSR(STVEC)
	RET

TEXT setscounteren(SB), 1, $-4
	MOV	R8, CSR(SCOUNTEREN)
	RET

/*
 * vlong sbicall(uvlong ext, uvlong fn, uvlong a0, uvlong a1, uvlong a2, uvlong *val):
 * the error code from R10 (a0); R11 (a1) to *val
 */
TEXT sbicall(SB), 1, $-4
	MOV	R8, R17
	MOV	fn+XLEN(FP), R16
	MOV	a0+(2*XLEN)(FP), R10
	MOV	a1+(3*XLEN)(FP), R11
	MOV	a2+(4*XLEN)(FP), R12
	ECALL
	MOV	val+(5*XLEN)(FP), R9
	BEQ	R9, _sbinoval
	MOV	R11, 0(R9)
_sbinoval:
	MOV	R10, R8
	RET

/*
 * floating point: sstatus.FS off makes every fp instruction
 * trap (illegal instruction, see trap.c)
 */
TEXT fpon(SB), 1, $-4
	MOV	$Fsst, R9
	CSRRC	CSR(SSTATUS), R9, R0
	MOV	$Fsclean, R9
	CSRRS	CSR(SSTATUS), R9, R0
	RET

TEXT fpoff(SB), 1, $-4
	MOV	$Fsst, R9
	CSRRC	CSR(SSTATUS), R9, R0
	RET

TEXT fpsaveregs(SB), 1, $-4
	MOVD	F0, (0*8)(R8)
	MOVD	F1, (1*8)(R8)
	MOVD	F2, (2*8)(R8)
	MOVD	F3, (3*8)(R8)
	MOVD	F4, (4*8)(R8)
	MOVD	F5, (5*8)(R8)
	MOVD	F6, (6*8)(R8)
	MOVD	F7, (7*8)(R8)
	MOVD	F8, (8*8)(R8)
	MOVD	F9, (9*8)(R8)
	MOVD	F10, (10*8)(R8)
	MOVD	F11, (11*8)(R8)
	MOVD	F12, (12*8)(R8)
	MOVD	F13, (13*8)(R8)
	MOVD	F14, (14*8)(R8)
	MOVD	F15, (15*8)(R8)
	MOVD	F16, (16*8)(R8)
	MOVD	F17, (17*8)(R8)
	MOVD	F18, (18*8)(R8)
	MOVD	F19, (19*8)(R8)
	MOVD	F20, (20*8)(R8)
	MOVD	F21, (21*8)(R8)
	MOVD	F22, (22*8)(R8)
	MOVD	F23, (23*8)(R8)
	MOVD	F24, (24*8)(R8)
	MOVD	F25, (25*8)(R8)
	MOVD	F26, (26*8)(R8)
	MOVD	F27, (27*8)(R8)
	MOVD	F28, (28*8)(R8)
	MOVD	F29, (29*8)(R8)
	MOVD	F30, (30*8)(R8)
	MOVD	F31, (31*8)(R8)
	RET

TEXT fploadregs(SB), 1, $-4
	MOVD	(0*8)(R8), F0
	MOVD	(1*8)(R8), F1
	MOVD	(2*8)(R8), F2
	MOVD	(3*8)(R8), F3
	MOVD	(4*8)(R8), F4
	MOVD	(5*8)(R8), F5
	MOVD	(6*8)(R8), F6
	MOVD	(7*8)(R8), F7
	MOVD	(8*8)(R8), F8
	MOVD	(9*8)(R8), F9
	MOVD	(10*8)(R8), F10
	MOVD	(11*8)(R8), F11
	MOVD	(12*8)(R8), F12
	MOVD	(13*8)(R8), F13
	MOVD	(14*8)(R8), F14
	MOVD	(15*8)(R8), F15
	MOVD	(16*8)(R8), F16
	MOVD	(17*8)(R8), F17
	MOVD	(18*8)(R8), F18
	MOVD	(19*8)(R8), F19
	MOVD	(20*8)(R8), F20
	MOVD	(21*8)(R8), F21
	MOVD	(22*8)(R8), F22
	MOVD	(23*8)(R8), F23
	MOVD	(24*8)(R8), F24
	MOVD	(25*8)(R8), F25
	MOVD	(26*8)(R8), F26
	MOVD	(27*8)(R8), F27
	MOVD	(28*8)(R8), F28
	MOVD	(29*8)(R8), F29
	MOVD	(30*8)(R8), F30
	MOVD	(31*8)(R8), F31
	RET

/*
 * int peek(void *src, void *dst, int len): fault-proof memmove for
 * the kernel; returns the bytes not copied: a fault between peek and
 * _peekend returns to the caller (trap.c)
 */
TEXT peek(SB), 1, $-4
	MOV	R8, R9
	MOV	dst+XLEN(FP), R10
	MOVW	len+(2*XLEN)(FP), R8
_peekloop:
	BEQ	R8, _peekdone
	MOVBU	0(R9), R12
	MOVB	R12, 0(R10)
	ADD	$1, R9
	ADD	$1, R10
	ADD	$-1, R8
	JMP	_peekloop
_peekdone:
	RET
TEXT _peekend(SB), 1, $-4
	RET
