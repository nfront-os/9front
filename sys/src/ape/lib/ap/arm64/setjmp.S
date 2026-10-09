/*
 * GCC port: GAS/AAPCS64 translation of setjmp.s, as libc/arm64/setjmp.S:
 * the AAPCS64 callee-saved registers are saved too (7c never had to):
 * jmp_buf [0]=sp [1]=pc [2..11]=x19-x28 [12]=x29 [13..20]=d8-d15
 * (sys/include/ape/setjmp.h). sigsetjmp: savemask and the blocked set
 * as 32-bit words, as the original, then the jmp_buf at 8.
 */
.globl sigsetjmp
sigsetjmp:
	adrp	x2, _psigblocked
	ldr	w2, [x2, :lo12:_psigblocked]
	str	w1, [x0, #0]		/* savemask */
	str	w2, [x0, #4]
	add	x0, x0, #8
	/* wet floor */

.globl setjmp
setjmp:
	mov	x1, sp
	stp	x1, x30, [x0, #0]
	stp	x19, x20, [x0, #16]
	stp	x21, x22, [x0, #32]
	stp	x23, x24, [x0, #48]
	stp	x25, x26, [x0, #64]
	stp	x27, x28, [x0, #80]
	str	x29, [x0, #96]
	stp	d8, d9, [x0, #104]
	stp	d10, d11, [x0, #120]
	stp	d12, d13, [x0, #136]
	stp	d14, d15, [x0, #152]
	mov	w0, #0
	ret

.globl longjmp
longjmp:
	ldp	x2, x30, [x0, #0]
	ldp	x19, x20, [x0, #16]
	ldp	x21, x22, [x0, #32]
	ldp	x23, x24, [x0, #48]
	ldp	x25, x26, [x0, #64]
	ldp	x27, x28, [x0, #80]
	ldr	x29, [x0, #96]
	ldp	d8, d9, [x0, #104]
	ldp	d10, d11, [x0, #120]
	ldp	d12, d13, [x0, #136]
	ldp	d14, d15, [x0, #152]
	mov	sp, x2
	mov	w0, w1			/* arg */
	cbz	w0, 1f
	ret
1:	mov	w0, #1
	ret
