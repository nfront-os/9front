/*
 * GCC port: GAS/System V translation: the argument is in %edi (6c:
 * RARG). The MXCSR scratch word lives in an explicit 8-byte stack slot
 * (the original's $4 frame) rather than the red zone.
 */

.globl setfcr
setfcr:
	xorl	$(0x3F<<7), %edi	/* bits are cleared in csr to enable them */
	andl	$0xFFC0, %edi		/* just the fcr bits */
	fwait				/* is this needed? */
	subq	$8, %rsp
	stmxcsr	0(%rsp)
	movl	0(%rsp), %eax
	andl	$0x3F, %eax
	orl	%edi, %eax
	movl	%eax, 0(%rsp)
	ldmxcsr	0(%rsp)
	addq	$8, %rsp
	ret

.globl getfcr
getfcr:
	subq	$8, %rsp
	fwait
	stmxcsr	0(%rsp)
	movzwl	0(%rsp), %eax
	andl	$0xFFC0, %eax
	xorl	$(0x3F<<7), %eax
	addq	$8, %rsp
	ret

.globl getfsr
getfsr:
	subq	$8, %rsp
	fwait
	stmxcsr	0(%rsp)
	movl	0(%rsp), %eax
	andl	$0x3F, %eax
	addq	$8, %rsp
	ret

.globl setfsr
setfsr:
	andl	$0x3F, %edi
	fwait
	subq	$8, %rsp
	stmxcsr	0(%rsp)
	movl	0(%rsp), %eax
	andl	$~0x3F, %eax
	orl	%edi, %eax
	movl	%eax, 0(%rsp)
	ldmxcsr	0(%rsp)
	addq	$8, %rsp
	ret
