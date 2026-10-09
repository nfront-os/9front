/* GCC port: GAS/AAPCS64 translation of fabs.s (the double in d0) */
.globl fabs
fabs:
	fabs	d0, d0
	ret
