/*
 * GCC port: GAS translation. The argument is where 8c left it (cdecl,
 * 4(%esp)); the result is in %st(0), as 8c returns it.
 */
.globl tan
tan:
	fldl	4(%esp)
	fptan
	fstp	%st(0)		/* get rid of extra 1.0 */
	ret
