/*
 * GCC port: GAS/psABI translation of setjmp.s. jc treats every
 * register as caller-saved, so its jmp_buf is just sp+pc. Under the
 * psABI s0-s11 and fs0-fs11 are callee-saved and live across setjmp()
 * in the callers above it (libthread switches threads with
 * setjmp/longjmp), so they are saved too: [0]=sp [1]=pc [2..13]=s0-s11
 * [14..25]=fs0-fs11 (jmp_buf in riscv64/include/u.h).
 */
.globl setjmp
setjmp:
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
	sd	s10, 96(a0)
	sd	s11, 104(a0)
	fsd	fs0, 112(a0)
	fsd	fs1, 120(a0)
	fsd	fs2, 128(a0)
	fsd	fs3, 136(a0)
	fsd	fs4, 144(a0)
	fsd	fs5, 152(a0)
	fsd	fs6, 160(a0)
	fsd	fs7, 168(a0)
	fsd	fs8, 176(a0)
	fsd	fs9, 184(a0)
	fsd	fs10, 192(a0)
	fsd	fs11, 200(a0)
	li	a0, 0
	ret

.globl longjmp
longjmp:
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
	ld	s10, 96(a0)
	ld	s11, 104(a0)
	fld	fs0, 112(a0)
	fld	fs1, 120(a0)
	fld	fs2, 128(a0)
	fld	fs3, 136(a0)
	fld	fs4, 144(a0)
	fld	fs5, 152(a0)
	fld	fs6, 160(a0)
	fld	fs7, 168(a0)
	fld	fs8, 176(a0)
	fld	fs9, 184(a0)
	fld	fs10, 192(a0)
	fld	fs11, 200(a0)
	sext.w	a0, a1			/* ansi: "longjmp(0) => longjmp(1)" */
	bnez	a0, 1f
	li	a0, 1
1:	ret
