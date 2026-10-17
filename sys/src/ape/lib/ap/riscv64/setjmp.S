/*
 * GCC port: GAS/psABI translation of setjmp.s, as libc/riscv64/setjmp.S:
 * the callee-saved registers are saved too (jc never had to):
 * jmp_buf [0]=sp [1]=pc [2..13]=s0-s11 [14..25]=fs0-fs11
 * (sys/include/ape/setjmp.h). sigsetjmp: savemask and the blocked set
 * as 32-bit words, as the original, then the jmp_buf at 8.
 */
.globl sigsetjmp
sigsetjmp:
	lla	t0, _psigblocked
	lw	t0, 0(t0)
	sw	a1, 0(a0)		/* savemask */
	sw	t0, 4(a0)
	addi	a0, a0, 8
	/* wet floor */

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
	sext.w	a0, a1
	bnez	a0, 1f
	li	a0, 1
1:	ret
