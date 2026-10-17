/*
 * GCC port: GAS/psABI translation of l.s. jc keeps m and up in R7 and
 * R6 (its extern registers); gcc code has them in s11 and s10
 * (dat.h, -ffixed-s10 -ffixed-s11). Arguments arrive in a0-a5, the
 * result goes in a0; Label also holds s0-s9 (dat.h). The trap frame
 * and the Ureg are those of l.s; CSR(SSCRATCH) always holds m.
 */
#include "mem.h"
#include "riscv.h"

/* offsets in Mach, known to asm */
#define MSPLPC		8
#define MPROC		16
#define MSAVE2		24
#define MSAVE9		32
#define MHARTID		56

#define UR(n)		(UREG+(n)*8)	/* offset of xn in the frame */
#define UPC		(UREG+0)
#define USTATUS		(UREG+32*8)
#define UIE		(UREG+33*8)
#define UCAUSE		(UREG+34*8)
#define UTVAL		(UREG+35*8)
#define UMODE		(UREG+36*8)

#define PTEKRWX		(PTEVALID|PTEREAD|PTEWRITE|PTEEXEC|PTEKERNEL)

.option norelax

/*
 * the firmware enters here at PHYSKTZERO, paging off, with the hart id
 * in a0 and the device tree's physical address in a1; until paging is
 * on everything is relative to the pc (-mcmodel=medany): physical.
 */
.text
.globl _main
_main:
	csrw	sie, zero
	csrci	sstatus, Sie
	csrw	satp, zero
	mv	s2, a0				/* hart id */
	mv	s3, a1				/* device tree */

	/* clear bss */
	lla	t0, edata
	lla	t1, end
1:	bgeu	t0, t1, 2f
	sb	zero, 0(t0)
	addi	t0, t0, 1
	j	1b
2:
	/* clear the Machs and the page tables after end (mem.h) */
	lla	s4, end
	li	t0, BY2PG-1
	add	s4, s4, t0
	srli	s4, s4, PGSHIFT
	slli	s4, s4, PGSHIFT			/* s4: MACHADDR(0) */
	li	t0, MAXMACH*MACHSIZE
	add	s5, s4, t0			/* s5: L1 */
	li	t0, 2*BY2PG
	add	t1, s5, t0
	mv	t0, s4
3:	bgeu	t0, t1, 4f
	sd	zero, 0(t0)
	addi	t0, t0, 8
	j	3b
4:
	/* boot page table, as l.s */
	li	t0, PHYSDRAM
	srli	t0, t0, PGSHIFT
	slli	t0, t0, PTEPPNSHIFT
	ori	t0, t0, PTEKRWX
	sd	t0, (2*8)(s5)			/* PTLX(PHYSDRAM, 2) */
	li	t1, 256*8
	add	t1, s5, t1
	sd	t0, (2*8)(t1)			/* PTLX(KZERO+PHYSDRAM, 2) */
	li	t0, PTEVALID|PTEREAD|PTEWRITE|PTEKERNEL
	sd	t0, 0(t1)			/* PTLX(KZERO, 2) */
	li	t0, BY2PG
	add	t0, s5, t0
	srli	t0, t0, PGSHIFT
	slli	t0, t0, PTEPPNSHIFT
	ori	t0, t0, PTEVALID
	li	t1, 384*8
	add	t1, s5, t1
	sd	t0, 0(t1)			/* PTLX(VMAP, 2) */

	srli	t0, s5, PGSHIFT
	li	t1, Sv39
	or	t0, t0, t1
	sfence.vma
	csrw	satp, t0
	sfence.vma

	/* continue at KZERO+ */
	li	t1, KZERO
	add	s4, s4, t1
	lla	t0, _virtual
	add	t0, t0, t1
	jr	t0

_virtual:
	mv	s11, s4				/* m = MACHADDR(0) */
	li	s10, 0				/* up = nil */
	li	t0, MACHSIZE
	add	sp, s11, t0
	csrw	sscratch, s11
	lla	t0, strap
	csrw	stvec, t0
	lla	t0, dtbpa
	sd	s3, 0(t0)
	sw	s2, MHARTID(s11)
	li	s0, 0
	call	main
_halt:
	wfi
	j	_halt

/*
 * secondary harts, as in l.s: a0 the hart id, a1 the physical address
 * of its Mach, whose regsave[0] is the satp
 */
.globl _secondary
_secondary:
	csrw	sie, zero
	csrci	sstatus, Sie
	ld	t0, MSAVE2(a1)
	sfence.vma
	csrw	satp, t0
	sfence.vma
	li	t1, KZERO
	add	a1, a1, t1
	lla	t0, _virtual2
	add	t0, t0, t1
	jr	t0
_virtual2:
	mv	s11, a1
	li	s10, 0
	li	t0, MACHSIZE
	add	sp, s11, t0
	csrw	sscratch, s11
	lla	t0, strap
	csrw	stvec, t0
	li	s0, 0
	call	main
	j	_halt

/*
 * trap and interrupt entry, from user or kernel mode: save everything
 * in a Ureg (on up's kernel stack, whose top is up, coming from user
 * mode; on the current stack from kernel mode) and call trap(ureg).
 */
.balign 4
.globl strap
strap:
	csrrw	s11, sscratch, s11		/* s11 = m, sscratch = old s11 */
	sd	sp, MSAVE2(s11)
	sd	t0, MSAVE9(s11)
	csrr	t0, sstatus
	andi	t0, t0, Spp
	bnez	t0, 1f
	ld	sp, MPROC(s11)			/* top of up's kernel stack */
1:	andi	sp, sp, -16
	addi	sp, sp, -TRAPFRAME
	sd	x1, UR(1)(sp)
	sd	x3, UR(3)(sp)
	sd	x4, UR(4)(sp)
	sd	x6, UR(6)(sp)
	sd	x7, UR(7)(sp)
	sd	x8, UR(8)(sp)
	sd	x9, UR(9)(sp)
	sd	x10, UR(10)(sp)
	sd	x11, UR(11)(sp)
	sd	x12, UR(12)(sp)
	sd	x13, UR(13)(sp)
	sd	x14, UR(14)(sp)
	sd	x15, UR(15)(sp)
	sd	x16, UR(16)(sp)
	sd	x17, UR(17)(sp)
	sd	x18, UR(18)(sp)
	sd	x19, UR(19)(sp)
	sd	x20, UR(20)(sp)
	sd	x21, UR(21)(sp)
	sd	x22, UR(22)(sp)
	sd	x23, UR(23)(sp)
	sd	x24, UR(24)(sp)
	sd	x25, UR(25)(sp)
	sd	x26, UR(26)(sp)
	sd	x28, UR(28)(sp)
	sd	x29, UR(29)(sp)
	sd	x30, UR(30)(sp)
	sd	x31, UR(31)(sp)
	ld	t0, MSAVE2(s11)
	sd	t0, UR(2)(sp)
	ld	t0, MSAVE9(s11)
	sd	t0, UR(5)(sp)			/* t0 is x5 */
	csrr	t0, sscratch
	sd	t0, UR(27)(sp)			/* the old s11 */
	csrw	sscratch, s11
	csrr	t0, sepc
	sd	t0, UPC(sp)
	csrr	t0, sstatus
	sd	t0, USTATUS(sp)
	srli	t0, t0, 8
	andi	t0, t0, 1
	sd	t0, UMODE(sp)			/* 1: from supervisor mode */
	csrr	t0, sie
	sd	t0, UIE(sp)
	csrr	t0, scause
	sd	t0, UCAUSE(sp)
	csrr	t0, stval
	sd	t0, UTVAL(sp)

	ld	s10, MPROC(s11)			/* up = m->proc */
	li	s0, 0
	addi	a0, sp, UREG
	call	trap

/*
 * return from trap, fork or notify, as in l.s; returning to kernel
 * mode, m (s11) stays this cpu's
 */
.globl forkret
.globl noteret
forkret:
noteret:
	csrci	sstatus, Sie
	ld	t0, UPC(sp)
	csrw	sepc, t0
	ld	t0, USTATUS(sp)
	li	t1, ~(Fsst|Sie)
	and	t0, t0, t1
	csrr	t2, sstatus
	li	t1, Fsst
	and	t2, t2, t1
	or	t0, t0, t2
	li	t1, Sum
	or	t0, t0, t1
	csrw	sstatus, t0
	andi	t0, t0, Spp
	bnez	t0, 1f
	ld	x27, UR(27)(sp)
1:	ld	x1, UR(1)(sp)
	ld	x3, UR(3)(sp)
	ld	x4, UR(4)(sp)
	ld	x5, UR(5)(sp)
	ld	x6, UR(6)(sp)
	ld	x7, UR(7)(sp)
	ld	x8, UR(8)(sp)
	ld	x9, UR(9)(sp)
	ld	x10, UR(10)(sp)
	ld	x11, UR(11)(sp)
	ld	x12, UR(12)(sp)
	ld	x13, UR(13)(sp)
	ld	x14, UR(14)(sp)
	ld	x15, UR(15)(sp)
	ld	x16, UR(16)(sp)
	ld	x17, UR(17)(sp)
	ld	x18, UR(18)(sp)
	ld	x19, UR(19)(sp)
	ld	x20, UR(20)(sp)
	ld	x21, UR(21)(sp)
	ld	x22, UR(22)(sp)
	ld	x23, UR(23)(sp)
	ld	x24, UR(24)(sp)
	ld	x25, UR(25)(sp)
	ld	x26, UR(26)(sp)
	ld	x28, UR(28)(sp)
	ld	x29, UR(29)(sp)
	ld	x30, UR(30)(sp)
	ld	x31, UR(31)(sp)
	ld	x2, UR(2)(sp)
	sret

/* to user mode for the first time (init0), as in l.s: sp in a0 */
.globl touser
touser:
	li	t0, Spp|Sie|Fsst
	csrc	sstatus, t0
	li	t0, Spie|Sum
	csrs	sstatus, t0
	li	t0, UTZERO+32
	csrw	sepc, t0
	mv	sp, a0
	li	ra, 0
	li	a0, 0
	li	s0, 0
	sret

.globl setlabel
setlabel:
	sd	sp, 0(a0)
	sd	ra, 8(a0)
	sd	s0, 16(a0)
	sd	s1, 24(a0)
	sd	s2, 32(a0)
	sd	s3, 40(a0)
	sd	s4, 48(a0)
	sd	s5, 56(a0)
	sd	s6, 64(a0)
	sd	s7, 72(a0)
	sd	s8, 80(a0)
	sd	s9, 88(a0)
	li	a0, 0
	ret

.globl gotolabel
gotolabel:
	ld	sp, 0(a0)
	ld	ra, 8(a0)
	ld	s0, 16(a0)
	ld	s1, 24(a0)
	ld	s2, 32(a0)
	ld	s3, 40(a0)
	ld	s4, 48(a0)
	ld	s5, 56(a0)
	ld	s6, 64(a0)
	ld	s7, 72(a0)
	ld	s8, 80(a0)
	ld	s9, 88(a0)
	li	a0, 1
	ret

.globl islo
islo:
	csrr	a0, sstatus
	andi	a0, a0, Sie
	ret

.globl splhi
splhi:
	sd	ra, MSPLPC(s11)
	csrrci	a0, sstatus, Sie
	andi	a0, a0, Sie
	ret

.globl spllo
spllo:
	csrrsi	a0, sstatus, Sie
	andi	a0, a0, Sie
	ret

.globl splx
splx:
	beqz	a0, 1f
	csrsi	sstatus, Sie
	ret
1:	sd	ra, MSPLPC(s11)
	csrci	sstatus, Sie
	ret

.globl tas
.globl _tas
tas:
_tas:
	li	t0, 1
	fence	rw, rw
	amoswap.w.aqrl	a0, t0, (a0)
	fence	rw, rw
	ret

/*
 * int cmpswap(long *p, long ov, long nv): a long is 64 bits with gcc
 * (Ref, Proc pids); int cas(int *p, int ov, int nv) stays 32-bit
 * (semcas in fns.h), as on arm64
 */
.globl cas
cas:
	sext.w	a1, a1
	fence	rw, rw
1:	lr.w.aq	t0, (a0)
	bne	t0, a1, 2f
	sc.w.aqrl	t1, a2, (a0)
	bnez	t1, 1b
	fence	rw, rw
	li	a0, 1
	ret
2:	fence	rw, rw
	li	a0, 0
	ret

.globl cmpswap
cmpswap:
	fence	rw, rw
1:	lr.d.aq	t0, (a0)
	bne	t0, a1, 2f
	sc.d.aqrl	t1, a2, (a0)
	bnez	t1, 1b
	fence	rw, rw
	li	a0, 1
	ret
2:	fence	rw, rw
	li	a0, 0
	ret

.globl coherence
coherence:
	fence	iorw, iorw
	ret

.globl idlehands
idlehands:
	fence	iorw, iorw
	lla	t0, nrdy
	lw	t0, 0(t0)
	bnez	t0, 1f
	wfi
1:	ret

.globl vcycles
vcycles:
	rdtime	a0
	ret

.globl setsatp
setsatp:
	sfence.vma
	csrw	satp, a0
	sfence.vma
	ret

.globl flushtlb
flushtlb:
	sfence.vma
	ret

.globl flushtlbva
flushtlbva:
	sfence.vma	a0, zero
	ret

.globl flushtlbasid
flushtlbasid:
	sfence.vma	zero, a0
	ret

.globl fencei
fencei:
	fence.i
	ret

.globl getsie
getsie:
	csrr	a0, sie
	ret

.globl setsie
setsie:
	csrw	sie, a0
	ret

.globl getsip
getsip:
	csrr	a0, sip
	ret

.globl setsip
setsip:
	csrw	sip, a0
	ret

.globl setstvec
setstvec:
	csrw	stvec, a0
	ret

.globl setscounteren
setscounteren:
	csrw	scounteren, a0
	ret

/*
 * vlong sbicall(uvlong ext, uvlong fn, uvlong a0, uvlong a1, uvlong a2, uvlong *val),
 * as in l.s
 */
.globl sbicall
sbicall:
	mv	a7, a0
	mv	a6, a1
	mv	a0, a2
	mv	a1, a3
	mv	a2, a4
	ecall
	beqz	a5, 1f
	sd	a1, 0(a5)
1:	ret

/* floating point, as in l.s; the kernel itself is built without */
.option push
.option arch, +d
.globl fpon
fpon:
	li	t0, Fsst
	csrc	sstatus, t0
	li	t0, Fsclean
	csrs	sstatus, t0
	ret

.globl fpoff
fpoff:
	li	t0, Fsst
	csrc	sstatus, t0
	ret

.globl fpsaveregs
fpsaveregs:
	fsd	f0, 0*8(a0)
	fsd	f1, 1*8(a0)
	fsd	f2, 2*8(a0)
	fsd	f3, 3*8(a0)
	fsd	f4, 4*8(a0)
	fsd	f5, 5*8(a0)
	fsd	f6, 6*8(a0)
	fsd	f7, 7*8(a0)
	fsd	f8, 8*8(a0)
	fsd	f9, 9*8(a0)
	fsd	f10, 10*8(a0)
	fsd	f11, 11*8(a0)
	fsd	f12, 12*8(a0)
	fsd	f13, 13*8(a0)
	fsd	f14, 14*8(a0)
	fsd	f15, 15*8(a0)
	fsd	f16, 16*8(a0)
	fsd	f17, 17*8(a0)
	fsd	f18, 18*8(a0)
	fsd	f19, 19*8(a0)
	fsd	f20, 20*8(a0)
	fsd	f21, 21*8(a0)
	fsd	f22, 22*8(a0)
	fsd	f23, 23*8(a0)
	fsd	f24, 24*8(a0)
	fsd	f25, 25*8(a0)
	fsd	f26, 26*8(a0)
	fsd	f27, 27*8(a0)
	fsd	f28, 28*8(a0)
	fsd	f29, 29*8(a0)
	fsd	f30, 30*8(a0)
	fsd	f31, 31*8(a0)
	ret

.globl fploadregs
fploadregs:
	fld	f0, 0*8(a0)
	fld	f1, 1*8(a0)
	fld	f2, 2*8(a0)
	fld	f3, 3*8(a0)
	fld	f4, 4*8(a0)
	fld	f5, 5*8(a0)
	fld	f6, 6*8(a0)
	fld	f7, 7*8(a0)
	fld	f8, 8*8(a0)
	fld	f9, 9*8(a0)
	fld	f10, 10*8(a0)
	fld	f11, 11*8(a0)
	fld	f12, 12*8(a0)
	fld	f13, 13*8(a0)
	fld	f14, 14*8(a0)
	fld	f15, 15*8(a0)
	fld	f16, 16*8(a0)
	fld	f17, 17*8(a0)
	fld	f18, 18*8(a0)
	fld	f19, 19*8(a0)
	fld	f20, 20*8(a0)
	fld	f21, 21*8(a0)
	fld	f22, 22*8(a0)
	fld	f23, 23*8(a0)
	fld	f24, 24*8(a0)
	fld	f25, 25*8(a0)
	fld	f26, 26*8(a0)
	fld	f27, 27*8(a0)
	fld	f28, 28*8(a0)
	fld	f29, 29*8(a0)
	fld	f30, 30*8(a0)
	fld	f31, 31*8(a0)
	ret
.option pop

/*
 * int peek(void *src, void *dst, int len), as in l.s: the bytes not
 * copied in a0, a fault between peek and _peekend returns to the caller
 */
.globl peek
peek:
	mv	t0, a0
	mv	a0, a2
1:	beqz	a0, 2f
	lbu	t1, 0(t0)
	sb	t1, 0(a1)
	addi	t0, t0, 1
	addi	a1, a1, 1
	addi	a0, a0, -1
	j	1b
2:	ret
.globl _peekend
_peekend:
	ret
