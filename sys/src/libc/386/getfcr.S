/* GCC port: GAS translation of getfcr.s (the $4 frame as an explicit slot) */
.globl setfcr
setfcr:
	subl	$4, %esp
	movl	8(%esp), %eax		/* p */
	xorb	$0x3f, %al
	movw	%ax, 0(%esp)
	wait
	fldcw	0(%esp)
	movw	0(%esp), %ax
	addl	$4, %esp
	ret

.globl getfcr
getfcr:
	subl	$4, %esp
	movw	%ax, 0(%esp)
	wait
	fstcw	0(%esp)
	movw	0(%esp), %ax
	xorb	$0x3f, %al
	addl	$4, %esp
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
