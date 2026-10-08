/*
 * GCC port: GAS/SysV translation. The original read its double argument off
 * the stack (a+0(FP), 6c's convention for float/double args); under gcc's
 * SysV ABI the first double argument arrives in %xmm0 already, and the
 * result is returned there too, so this is now a pure register op.
 */

.globl sqrt
sqrt:
	sqrtsd	%xmm0, %xmm0
	ret
