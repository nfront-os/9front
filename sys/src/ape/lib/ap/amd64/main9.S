/*
 * GCC port: GAS/System V translation, as libc/amd64/main9.s. The kernel
 * starts the process with the Tos pointer in %rax and argc, argv[0], ...
 * on the stack; the original pushes _apemain and a dummy return pc and
 * jumps to _callmain, whose 6c frame then holds f, argc and arg0. Under
 * System V the arguments travel in registers: _callmain(_apemain, argc,
 * argv) (plan9/callmain.c) with argv pointing into the stack and %rsp
 * 16-byte aligned for the call.
 */
.comm	_tos, 8, 8

.globl _main
_main:
	movq	%rax, _tos
	movq	$_apemain, %rdi
	movq	0(%rsp), %rsi		/* argc */
	leaq	8(%rsp), %rdx		/* argv */
	andq	$~15, %rsp
	call	_callmain
