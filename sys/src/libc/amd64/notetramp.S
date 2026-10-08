/*
 * GCC port: notify() needs a hand-written trampoline between the kernel
 * and the user's registered handler.
 *
 * sys/src/9/pc64/trap.c's notify() (unmodified stock kernel code) invokes
 * up->notify DIRECTLY -- not via a normal `call`, but by setting up the
 * resumed user context's registers/stack and letting the trap-return
 * path jump straight to it. It uses the OLD 6c amd64 calling convention
 * (stock kencc-compiled code always used this, so the kernel never
 * needed a bridge): the first/only logical argument (nureg, a Ureg*)
 * arrives in %rbp, ALSO redundantly placed on the stack at 8(%rsp), with
 * the note message's address at 16(%rsp) (0(%rsp) is always a fake
 * return pc, unused). A gcc-compiled handler expects ordinary SysV
 * argument passing (%rdi, %rsi) instead -- installing it as up->notify
 * directly means it gets called with garbage arguments.
 *
 * Fix: install _notetramp (below) as up->notify instead. It reads the
 * kernel's old-convention arguments and re-dispatches to the real,
 * user-registered handler via an ordinary `call` with correct SysV
 * arguments. notify(fn) itself just stashes fn and installs the
 * trampoline via the renamed raw syscall _notify (scripts/gen-9syscall.sh
 * names the stub for sys.h's NOTIFY _notify).
 */

.bss
.align 8
_notifyfn:
	.quad 0

.text
.globl	notify
notify:
	/* notify(void(*fn)(void*,char*)) -- SysV: fn arrives in %rdi */
	movq	%rdi, _notifyfn(%rip)
	testq	%rdi, %rdi			/* notify(nil): no handler, no trampoline */
	jz	1f
	leaq	_notetramp(%rip), %rdi
1:
	jmp	_notify				/* tail call: our caller's return address and _notify's return value both pass through untouched */

.globl	_notetramp
_notetramp:
	movq	16(%rsp), %rsi			/* note message pointer -> SysV arg2 */
	movq	%rbp, %rdi			/* nureg -> SysV arg1 (unused void* by convention) */
	call	*_notifyfn(%rip)

	/* The registered handler is expected to call noted() itself,
	 * which does not return here (matches stock Plan9 behavior). If
	 * it somehow falls through anyway, fall back to the kernel's
	 * default action rather than running off into undefined state.
	 * Raw inline syscall, not a call to the C noted(): this stack
	 * region was carved out by the kernel specifically for this
	 * trampoline (see trap.c's notify()), nothing else is relying on
	 * its contents surviving, so reusing it directly is safe without
	 * a dedicated sub/add frame of our own. */
	movq	$1, 8(%rsp)			/* NDFLT */
	movq	$29, %rbp			/* NOTED */
	syscall
hang:
	jmp	hang
