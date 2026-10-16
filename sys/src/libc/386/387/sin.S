/*
 * GCC port: GAS translation. The argument is where 8c left it (cdecl,
 * 4(%esp)); the result is in %st(0), as 8c returns it.
 */
.globl sin
sin:
	fldl	4(%esp)
	fsin
	ret

.globl cos
cos:
	fldl	4(%esp)
	fcos
	ret
