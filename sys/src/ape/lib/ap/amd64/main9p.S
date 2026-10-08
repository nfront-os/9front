/*
 * GCC port: GAS/System V translation, see main9.s and libc/amd64/main9p.s.
 * _savearg/_saveret/_callpc serve plan9/profile.c's 6c -p
 * instrumentation, which gcc does not emit; _callpc keeps the original's
 * 6c stack-slot arithmetic.
 */
.comm	_tos, 8, 8

.globl _mainp
_mainp:
	movq	%rax, _tos
	movq	$_profmain, %rdi
	movq	0(%rsp), %rsi		/* argc */
	leaq	8(%rsp), %rdx		/* argv */
	andq	$~15, %rsp
	call	_callmain

.globl _savearg
_savearg:
	movq	%rdi, %rax
	ret

.globl _saveret
_saveret:
	ret				/* the return value is still in %rax */

.globl _callpc
_callpc:
	movq	8(%rdi), %rax
	ret
