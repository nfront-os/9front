/*
 * GCC port: GAS/System V translation. The kernel starts the process
 * with the Tos pointer in %rax and argc, argv[0], ... on the stack.
 * The original pushes main and a dummy return pc and jumps to
 * _callmain, whose 6c frame then holds main, argc and arg0 with argv
 * at &arg0. Under System V the arguments travel in registers instead,
 * so _callmain (9sys/callmain.c) is called as
 * _callmain(main, argc, argv) with argv pointing into the stack, and
 * %rsp is 16-byte aligned for the call.
 */
.globl _main
_main:
	movq	%rax, _tos
	movq	$main, %rdi
	movq	0(%rsp), %rsi		/* argc */
	leaq	8(%rsp), %rdx		/* argv */
	andq	$~15, %rsp
	call	_callmain
