/* GCC port: GAS translation of cycles.s (APE: _cycles) */
.globl _cycles
_cycles:
	mrs	x1, cntvct_el0
	str	x1, [x0]
	ret
