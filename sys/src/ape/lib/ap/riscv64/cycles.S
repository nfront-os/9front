/* GCC port: GAS translation of cycles.s: void cycles(uvlong*) */
.globl cycles
cycles:
	rdcycle	t0
	sd	t0, 0(a0)
	ret
