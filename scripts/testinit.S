/*
 * Minimal hand-written test replacement for initcode (see
 * sys/src/9/port/userinit.c's proc0()): proc0() copies this raw blob
 * verbatim to UTZERO (one read-only page, no data segment) and jumps to
 * UTZERO+0x28 via touser() -- the +0x28 matches the original a.out EXEC
 * header size for amd64 (32-byte base header + 8-byte expanded 64-bit
 * entry field, see sys/include/a.out.h's HDR_MAGIC), which upstream
 * code never strips before copying. We don't bother building a real
 * header; the leading 0x28 bytes are simply never executed.
 *
 * Purpose: exercise the real syscall path (ARG()/Sargs mechanism, see
 * MEMORY.md "sys/9: fix syscall argument extraction") end-to-end from
 * an actual exec'd/bootstrapped user process, since no userspace libc
 * exists yet in this tree to build a conventional test binary.
 *
 * Syscall ABI (see sys/src/9/pc64/l.S's syscallentry and
 * sys/src/9/pc64/trap.c's syscall()): syscall number in %rbp,
 * `syscall` instruction; arguments read by the kernel from the calling
 * process's OWN stack starting at (user %rsp at trap time) + 8 -- one
 * 8-byte slot per argument regardless of its C type (struct Sargs).
 */
.code64
.section .text
.globl start

.skip 0x28, 0x90
start:
	sub	$512, %rsp		/* scratch space below whatever touser() set up */

	/* open("/dev/cons", OWRITE) */
	lea	conspath(%rip), %rax
	mov	%rax, 8(%rsp)		/* arg0: name */
	movq	$1, 16(%rsp)		/* arg1: mode = OWRITE */
	mov	$14, %rbp		/* OPEN */
	syscall

	/* pwrite(0, msg, msglen, ~0) */
	movq	$0, 8(%rsp)		/* arg0: fd */
	lea	msg(%rip), %rax
	mov	%rax, 16(%rsp)		/* arg1: buf */
	movq	$msglen, 24(%rsp)	/* arg2: len */
	movq	$-1, 32(%rsp)		/* arg3: off = ~0 (append/ignore) */
	mov	$51, %rbp		/* PWRITE */
	syscall

	/* exits(nil) */
	movq	$0, 8(%rsp)		/* arg0: status */
	mov	$8, %rbp		/* EXITS */
	syscall

hang:
	jmp	hang

conspath:
	.asciz	"#c/cons"
msg:
	.ascii	"hello from initcode via real exec!\n"
msglen = . - msg
