/*
 * GCC port: own implementation. The original returns -8(firstarg):
 * under 6c the caller's first argument sits on the stack right above
 * its return address. Under System V arguments arrive in registers and
 * &firstarg is a spill slot somewhere in the caller's frame, so that
 * relation does not hold. Everything is compiled with
 * -fno-omit-frame-pointer and this leaf sets up no frame of its own, so
 * %rbp is still the caller's frame pointer and 8(%rbp) is the caller's
 * return address -- the value the original computes. The argument is
 * ignored.
 */
.globl getcallerpc
getcallerpc:
	movq	8(%rbp), %rax
	ret
