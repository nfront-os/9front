/*
 * GCC port: own implementation. The original returns 0(SP), where
 * jc's callee keeps its return address. gcc code is compiled with
 * -fno-omit-frame-pointer and this leaf sets up no frame of its own,
 * so s0 is still the caller's frame pointer, the caller's return
 * address is at -8(s0). The argument is ignored.
 */
.globl getcallerpc
getcallerpc:
	ld	a0, -8(s0)
	ret
