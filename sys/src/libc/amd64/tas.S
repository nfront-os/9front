/*
 * GCC port: GAS/SysV translation. RARG (6c's first-argument register) ->
 * %rdi under gcc's SysV ABI (see setjmp.s). Logic unchanged: atomic
 * exchange of 0xdeaddead into *p, returning the old value in %eax.
 */

.globl _tas
_tas:
	movl	$0xdeaddead, %eax
	xchgl	%eax, (%rdi)
	ret
