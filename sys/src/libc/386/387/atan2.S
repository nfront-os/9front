/*
 * GCC port: GAS translation. The argument is where 8c left it (cdecl,
 * 4(%esp)); the result is in %st(0), as 8c returns it.
 */
.globl atan2
atan2:
	fldl	4(%esp)
	fldl	12(%esp)
	fpatan
	ret
