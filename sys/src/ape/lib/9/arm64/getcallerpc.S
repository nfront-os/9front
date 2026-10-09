/*
 * GCC port: own implementation (as libc/amd64/getcallerpc.S). The
 * original returns 0(SP), where 7c's caller keeps its return address.
 * gcc code is compiled with -fno-omit-frame-pointer and this leaf sets
 * up no frame of its own, so x29 is still the caller's frame record
 * {previous x29, x30}: 8(x29) is the caller's return address. The
 * argument is ignored.
 */
.globl getcallerpc
getcallerpc:
	ldr	x0, [x29, #8]
	ret
