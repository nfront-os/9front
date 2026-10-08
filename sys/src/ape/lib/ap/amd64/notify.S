/*
 * GCC port: the kernel enters note handlers the 6c way (pc64/trap.c
 * notify(), noted() NSAVE): the Ureg* in %rbp and at 8(%rsp), the note
 * message at 16(%rsp), 0(%rsp) a fake return pc. gcc-compiled handlers
 * take System V arguments (%rdi, %rsi), so -- as libc/amd64/notetramp.S
 * does for notify() -- trampolines translate:
 *
 * _NOTIFY(f) stashes f and registers _notifytramp with the raw system
 * call __NOTIFY (scripts/gen-9syscall.sh --ape); _NOTIFY(nil) clears the
 * handler. _notecont is the pc notetramp.c's _notetramp() resumes at
 * (NSAVE) and calls its notecont(Ureg*, char*). Neither handler
 * returns (they end in _NOTED); should one, the note gets the default
 * action. %rsp is 16-byte aligned for the calls.
 */
.bss
.align 8
_notifyfn:
	.quad 0

.text
.globl	_NOTIFY
_NOTIFY:
	movq	%rdi, _notifyfn(%rip)
	testq	%rdi, %rdi
	jz	1f
	leaq	_notifytramp(%rip), %rdi
1:
	jmp	__NOTIFY

_notifytramp:
	movq	%rbp, %rdi			/* Ureg* */
	movq	16(%rsp), %rsi			/* note message */
	andq	$~15, %rsp
	call	*_notifyfn(%rip)
	jmp	2f

.globl	_notecont
_notecont:
	movq	%rbp, %rdi
	movq	16(%rsp), %rsi
	andq	$~15, %rsp
	call	notecont

2:
	subq	$16, %rsp
	movq	$1, 8(%rsp)			/* NDFLT */
	movq	$29, %rbp			/* NOTED */
	syscall
3:
	jmp	3b
