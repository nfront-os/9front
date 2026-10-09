/* GCC port: GAS translation of cycles.s */
.globl cycles
cycles:
	mrs	x1, cntvct_el0
	str	x1, [x0]
	ret
