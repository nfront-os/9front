/*
 * GCC port: GAS/System V translation: a, b, c arrive in %edi, %esi,
 * %edx. MULL/IMULL write their high half to %edx, so c is moved to
 * %ecx first (the original read b and c from the stack). long is 64
 * bits under gcc (ulong stays 32, amd64/include/u.h), so muldiv
 * sign-extends its 32-bit quotient.
 */
.globl umuldiv
umuldiv:
	movl	%edx, %ecx
	movl	%edi, %eax
	mull	%esi
	divl	%ecx
	ret

.globl muldiv
muldiv:
	movl	%edx, %ecx
	movl	%edi, %eax
	imull	%esi
	idivl	%ecx
	cltq
	ret
