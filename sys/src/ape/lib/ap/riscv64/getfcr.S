/*
 * GCC port: GAS translation of getfcr.s: the fcsr; also in the
 * kernel (fpu.c), which is built without the F extension
 */
.option arch, +f
.globl getfcr
.globl getfsr
getfcr:
getfsr:
	frcsr	a0
	ret

.globl setfcr
.globl setfsr
setfcr:
setfsr:
	fscsr	a0
	ret
