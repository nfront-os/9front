/*
 * Minimal freestanding Plan9-syscall test program, built as a real,
 * properly-linked static ELF64 executable (gcc+ld, not flat-extracted)
 * -- to test the gcc port's own ELF64 exec loader (sysexecelf.c) with
 * genuine ELF program headers, not the hand-spliced flat-binary trick
 * used for the bootstrap initcode test.
 *
 * Syscall ABI (same as scripts/testinit.S): syscall number in %rbp,
 * `syscall` instruction; arguments read by the kernel from this
 * process's own stack starting at (user %rsp at trap time) + 8, one
 * 8-byte slot per argument.
 */
.code64
.section .text
.globl _start
_start:
	sub	$512, %rsp

	/* open("#c/cons", OWRITE) */
	lea	conspath(%rip), %rax
	mov	%rax, 8(%rsp)
	movq	$1, 16(%rsp)		/* OWRITE */
	mov	$14, %rbp		/* OPEN */
	syscall

	/* pwrite(0, msg, msglen, ~0) */
	movq	$0, 8(%rsp)
	lea	msg(%rip), %rax
	mov	%rax, 16(%rsp)
	movq	$msglen, 24(%rsp)
	movq	$-1, 32(%rsp)
	mov	$51, %rbp		/* PWRITE */
	syscall

	/* exits(nil) */
	movq	$0, 8(%rsp)
	mov	$8, %rbp		/* EXITS */
	syscall

hang:
	jmp	hang

.section .rodata
conspath:
	.asciz	"#c/cons"
msg:
	.ascii	"hello from gcc ELF64 exe via real exec!\n"
msglen = . - msg
