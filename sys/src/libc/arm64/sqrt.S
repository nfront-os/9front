/* GCC port: GAS/AAPCS64 translation of sqrt.s (the double in d0) */
.globl sqrt
sqrt:
	fsqrt	d0, d0
	ret
