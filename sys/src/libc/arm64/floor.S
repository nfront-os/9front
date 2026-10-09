/* GCC port: GAS/AAPCS64 translation of floor.s (the double in d0) */
.globl floor
floor:
	frintm	d0, d0
	ret

.globl ceil
ceil:
	frintp	d0, d0
	ret
