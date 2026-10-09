/*
 * GCC port: notify() needs a trampoline between the kernel and the
 * user's registered handler, as on amd64 (libc/amd64/notetramp.S).
 *
 * sys/src/9/arm64/trap.c's notify() enters up->notify the 7c way: nureg
 * (the Ureg*) in R0 and on the stack at 8(SP), the note message's
 * address at 16(SP), 0(SP) a fake return pc. A gcc-compiled handler
 * expects the message in x1. notify(fn) keeps fn and installs
 * _notetramp through the raw system call _notify
 * (scripts/gen-9syscall.sh), _notetramp calls fn(nureg, msg) from a
 * 16-byte aligned stack. The kernel's stack pointer is only 8-byte
 * aligned, so it is read through x9 (no SP-relative access).
 */

.bss
.align 3
_notifyfn:
	.quad 0

.text
.globl	notify
notify:
	/* notify(void(*fn)(void*,char*)): fn arrives in x0 */
	adrp	x9, _notifyfn
	str	x0, [x9, :lo12:_notifyfn]
	cbz	x0, 1f				/* notify(nil): no handler, no trampoline */
	adrp	x0, _notetramp
	add	x0, x0, :lo12:_notetramp
1:
	b	_notify				/* tail call */

.globl	_notetramp
_notetramp:
	mov	x9, sp
	ldr	x1, [x9, #16]			/* note message */
	and	x9, x9, #~15
	mov	sp, x9
	mov	x29, #0
	adrp	x9, _notifyfn
	ldr	x9, [x9, :lo12:_notifyfn]
	blr	x9

	/* The handler calls noted(), which does not return here; should it
	 * fall through, take the default action (noted(NDFLT)). */
	mov	x0, #1				/* NDFLT */
	bl	noted
1:
	b	1b
