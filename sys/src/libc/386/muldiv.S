/* GCC port: GAS translation of muldiv.s */
.globl umuldiv
umuldiv:
	movl	4(%esp), %eax		/* a */
	mull	8(%esp)			/* b */
	divl	12(%esp)		/* c */
	ret

.globl muldiv
muldiv:
	movl	4(%esp), %eax		/* a */
	imull	8(%esp)			/* b */
	idivl	12(%esp)		/* c */
	ret
