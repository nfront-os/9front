/*
 * GCC port: arg1 (v) arrives at 4(%esp) under cdecl, matching front's
 * original "v+0(FP)" (kencc's FP-relative addressing for a 386 asm
 * function's first stack argument). The "-4(v)" offset assumes the
 * caller's own frame places v exactly 4 bytes above its saved return
 * address -- true under Plan9 8c's fixed calling convention, same
 * caveat as sys/src/libc/amd64/getcallerpc.S.
 */
.globl getcallerpc
getcallerpc:
	movl	4(%esp), %eax
	movl	-4(%eax), %eax
	ret
