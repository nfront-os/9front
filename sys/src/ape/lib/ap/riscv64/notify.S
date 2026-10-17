/*
 * GCC port: APE's notify trampolines for riscv64, as arm64/notify.S and
 * libc/riscv64/notetramp.S. The kernel enters a note handler the jc way
 * (Ureg* in R8 (s0) and at 8(sp), the message at 16(sp)); a gcc
 * function takes them in a0, a1 and wants a 16-byte aligned stack.
 *
 *  _NOTIFY(fn)	keeps fn, installs _notifytramp through the raw stub
 *		__NOTIFY (scripts/build/gen-9syscall.sh --ape)
 *  _notecont	where notetramp.c's _notetramp sends a note (u->pc):
 *		calls notecont(u, msg)
 */
.bss
.align 3
_notifyfn:
	.quad 0

.text
.globl	_NOTIFY
_NOTIFY:
	lla	t0, _notifyfn
	sd	a0, 0(t0)
	beqz	a0, 1f
	lla	a0, _notifytramp
1:
	tail	__NOTIFY

_notifytramp:
	ld	a0, 8(sp)
	ld	a1, 16(sp)
	andi	sp, sp, -16
	li	s0, 0
	lla	t0, _notifyfn
	ld	t0, 0(t0)
	jalr	t0
	j	2f

.globl	_notecont
_notecont:
	ld	a0, 8(sp)
	ld	a1, 16(sp)
	andi	sp, sp, -16
	li	s0, 0
	call	notecont

2:
	li	a0, 1				/* NDFLT */
	call	_NOTED
3:
	j	3b
