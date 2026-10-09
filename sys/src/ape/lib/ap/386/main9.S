/*
 * GCC port: GAS translation of main9.s, as libc/386/main9.S. The kernel
 * starts the process with the Tos pointer in %eax and argc, argv[0], ...
 * on the stack; _callmain(_apemain, argc, argv) (plan9/callmain.c, gcc)
 * gets argv itself, from a 16-byte aligned stack.
 */
.comm	_tos, 4, 4

.globl _main
_main:
	movl	%eax, _tos
	movl	0(%esp), %ecx		/* argc */
	leal	4(%esp), %edx		/* argv */
	andl	$~15, %esp
	subl	$4, %esp
	pushl	%edx
	pushl	%ecx
	pushl	$_apemain
	call	_callmain
