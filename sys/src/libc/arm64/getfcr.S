/* GCC port: GAS translation of getfcr.s */
.globl setfcr
setfcr:
	msr	fpcr, x0
	ret

.globl getfcr
getfcr:
	mrs	x0, fpcr
	ret

.globl getfsr
getfsr:
	mrs	x0, fpsr
	ret

.globl setfsr
setfsr:
	msr	fpsr, x0
	ret
