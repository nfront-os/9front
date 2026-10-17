/*
 * GCC port: notify() needs a trampoline between the kernel and the
 * user's registered handler, as on amd64 and arm64.
 *
 * sys/src/9/riscv64/trap.c's notify() enters up->notify the jc way:
 * nureg (the Ureg*) in R8 (s0) and on the stack at 8(sp), the note
 * message's address at 16(sp), 0(sp) a fake return pc. A gcc-compiled
 * handler expects them in a0 and a1. notify(fn) keeps fn and installs
 * _notetramp through the raw system call _notify
 * (scripts/build/gen-9syscall.sh); _notetramp calls fn(nureg, msg)
 * from a 16-byte aligned stack (the kernel's is 8-byte aligned).
 */

.bss
.align 3
_notifyfn:
	.quad 0

.text
.globl	notify
notify:
	/* notify(void(*fn)(void*,char*)): fn arrives in a0 */
	lla	t0, _notifyfn
	sd	a0, 0(t0)
	beqz	a0, 1f				/* notify(nil): no handler, no trampoline */
	lla	a0, _notetramp
1:
	tail	_notify

.globl	_notetramp
_notetramp:
	ld	a0, 8(sp)			/* nureg */
	ld	a1, 16(sp)			/* note message */
	andi	sp, sp, -16
	li	s0, 0
	lla	t0, _notifyfn
	ld	t0, 0(t0)
	jalr	t0

	/* The handler calls noted(), which does not return here; should it
	 * fall through, take the default action (noted(NDFLT)). */
	li	a0, 1				/* NDFLT */
	call	noted
1:
	j	1b
