/* GCC port: GAS translation of getfcr.s */
.globl setfcr
setfcr:
	movl	4(%esp), %eax		/* p */
	xorb	$0x3f, %al
	pushw	%ax
	wait
	fldcw	0(%esp)
	popw	%ax
	ret

.globl getfcr
getfcr:
	pushw	%ax
	wait
	fstcw	0(%esp)
	popw	%ax
	xorb	$0x3f, %al
	ret

.globl getfsr
getfsr:
	wait
	fstsw	%ax
	ret

.globl setfsr
setfsr:
	wait
	fclex
	ret
