#define NPRIVATES 16
.code64

.text

// %rdi and %rsi are set up as argc, argv for main.
// %rax points to top of stack (this is what the kernel's exec() return
// path actually hands the new process -- sysproc.c's sysexec()/
// execregs() return the Tos pointer as their C return value, which
// trap.c's syscall() dispatcher stores into ureg->ax; matches the
// original stock main9.s's "MOVQ AX, _tos(SB)", NOT %rdx)
.globl	_main
_main:

	movq	0(%rsp), %rdi
	leaq	8(%rsp), %rsi
	movq	%rax, _tos
	movq 	$-128, _privates
	addq	%rsp, _privates
	movl 	$NPRIVATES, _nprivates
	leaq	-160(%rsp), %rsp
	call	main

loop:
	movq	$_exits, %rdi
	call	exits
	jmp	loop

.data
_exits:
	.ascii	"main"


.globl gettls0
gettls0:
	movq %fs:0, %rax
	ret
