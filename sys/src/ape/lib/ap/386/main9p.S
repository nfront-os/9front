.comm	_tos, 4, 4

/* GCC port: GAS translation of main9p.s, _mainp as main9.S's _main */
.globl _mainp
_mainp:
	movl	%eax, _tos
	movl	0(%esp), %ecx		/* argc */
	leal	4(%esp), %edx		/* argv */
	andl	$~15, %esp
	subl	$4, %esp
	pushl	%edx
	pushl	%ecx
	pushl	$_profmain
	call	_callmain

.globl _saveret
.globl _savearg
_saveret:
_savearg:
	ret

.globl _callpc
_callpc:
	movl	4(%esp), %eax		/* argp */
	movl	4(%eax), %eax
	ret
