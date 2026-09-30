/*
 * GCC port: argp (arg1) arrives in %rdi under System V, not RARG/BP plus
 * an FP-relative stack slot. The "-8(argp)" offset assumes the caller's
 * own stack frame places argp exactly 8 bytes above its saved return
 * address -- true under Plan9 6c's fixed calling convention, unverified
 * for a gcc/clang-compiled caller's frame layout. Preserved as a faithful,
 * literal port of the original; flag for review once real callers are
 * exercised under the new ABI.
 */
.globl getcallerpc
getcallerpc:
	movq	-8(%rdi), %rax
	ret
