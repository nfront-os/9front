/*
 * GCC port: APE's notify trampolines for arm64, as amd64/notify.S and
 * libc/arm64/notetramp.S. The kernel enters a note handler the 7c way
 * (Ureg* in R0, the message at 16(SP)); a gcc function takes the message
 * in x1 and wants a 16-byte aligned stack.
 *
 *  _NOTIFY(fn)	keeps fn, installs _notifytramp through the raw stub
 *		__NOTIFY (scripts/gen-9syscall.sh --ape)
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
	adrp	x9, _notifyfn
	str	x0, [x9, :lo12:_notifyfn]
	cbz	x0, 1f
	adrp	x0, _notifytramp
	add	x0, x0, :lo12:_notifytramp
1:
	b	__NOTIFY

_notifytramp:
	mov	x9, sp
	ldr	x1, [x9, #16]			/* note message */
	and	x9, x9, #~15
	mov	sp, x9
	mov	x29, #0
	adrp	x9, _notifyfn
	ldr	x9, [x9, :lo12:_notifyfn]
	blr	x9
	b	2f

.globl	_notecont
_notecont:
	mov	x9, sp
	ldr	x1, [x9, #16]
	and	x9, x9, #~15
	mov	sp, x9
	mov	x29, #0
	bl	notecont

2:
	mov	w0, #1				/* NDFLT */
	bl	_NOTED
3:
	b	3b
