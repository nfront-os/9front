/*
 * GCC port: GAS translation of main9.s. The kernel starts the process
 * with the Tos pointer in %eax and argc, argv[0], ... on the stack. The
 * original pushes main and a dummy return pc and jumps to _callmain,
 * whose 8c frame then holds main, argc and arg0 with argv at &arg0;
 * gcc's _callmain (9sys/callmain.c, __GNUC__) takes argv itself, so it
 * is passed explicitly, from a 16-byte aligned stack.
 */
.globl _main
_main:
	movl	%eax, _tos
	movl	0(%esp), %ecx		/* argc */
	leal	4(%esp), %edx		/* argv */
	andl	$~15, %esp
	subl	$4, %esp
	pushl	%edx
	pushl	%ecx
	pushl	$main
	call	_callmain
