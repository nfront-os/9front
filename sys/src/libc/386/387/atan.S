/*
 * GCC port: GAS translation. The argument is where 8c left it (cdecl,
 * 4(%esp)); the result is in %st(0), as 8c returns it.
 */
.globl atan
atan:
	fldl	4(%esp)
	fld1
	fpatan
	ret
