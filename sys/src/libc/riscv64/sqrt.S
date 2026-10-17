/* GCC port: GAS translation of sqrt.s: double sqrt(double) */
.globl sqrt
sqrt:
	fsqrt.d	fa0, fa0
	ret
