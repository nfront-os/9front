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
 * exists yet in this tree to build a conventional test binary. Also
 * exercises both of sysexec()'s loaders with real compiler output
 * (not hand-assembled code): a gcc-compiled static ELF64 binary
 * (hello_elf.S/testelf.i, tests sysexecelf.c) and a real
 * `GOOS=plan9 GOARCH=amd64 go build` a.out binary (hello_go.go/
 * testgo.i, tests the a.out loader). There is no real filesystem to
 * exec() either from, so this rfork()s: the child copies the
 * kernel-embedded ELF bytes (mapped read-only at TESTELFVA by
 * proc0(), see userinit.c) into a #g (devsegment) shared segment --
 * the one device that gives us a seekable Chan backed by arbitrary
 * memory -- and exec()s that; the parent does the same for the Go
 * binary (TESTGOVA), under a different #g name since that namespace
 * is global, not per-process.
 *
 * Syscall ABI (see sys/src/9/pc64/l.S's syscallentry and
 * sys/src/9/pc64/trap.c's syscall()): syscall number in %rbp,
 * `syscall` instruction; arguments read by the kernel from the calling
 * process's OWN stack starting at (user %rsp at trap time) + 8 -- one
 * 8-byte slot per argument regardless of its C type (struct Sargs).
 * Also: this kernel's syscall-return path (forkret, l.S) zeroes
 * %rbx/%rdx/%rsi/%rdi/%r8-%r10/%r12/%r13/%rbp on every syscall return
 * -- only %r14/%r15/%rsp survive a `syscall`. (Ordinary `call`/`ret`
 * within this file, as used for the docopyexec subroutine below, are
 * NOT affected -- that's plain x86, not this kernel's syscall
 * convention -- so normal register-preservation reasoning applies
 * there.)
 */
.code64
.section .text
.globl start

.skip 0x28, 0x90
start:
	sub	$512, %rsp		/* scratch space below whatever touser() set up */

	/* open("#c/cons", OWRITE) */
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

	/* rfork(RFPROC|RFFDG|RFREND) -- same flags as fork() */
	movq	$0x2014, 8(%rsp)
	mov	$19, %rbp		/* RFORK */
	syscall
	cmpq	$0, %rax
	je	child

	/* parent: test the real-toolchain a.out (Go plan9/amd64) binary */
	lea	goparams(%rip), %rdi
	call	docopyexec
	jmp	done

child:
	/* child: test the real-toolchain ELF64 (gcc) binary */
	lea	elfparams(%rip), %rdi
	call	docopyexec

done:
	/* if docopyexec returns, exec() failed -- fall through and exit */
	movq	$0, 8(%rsp)		/* arg0: status */
	mov	$8, %rbp		/* EXITS */
	syscall

hang:
	jmp	hang

/*
 * docopyexec(params): copy params->srclen bytes from params->srcva
 * (one of our own already-mapped read-only segments) into a freshly
 * created #g shared segment, then exec() it. Ordinary subroutine --
 * plain x86 call/ret register rules apply, NOT the syscall-zeroing
 * ones above -- but every *syscall* this makes still needs its own
 * live state reloaded from memory afterward, same reasoning as a
 * single inline copy loop would need.
 *
 * params layout (8 bytes each, see elfparams/goparams below):
 *   0 gpath  8 ctlpath  16 datapath  24 vacmd  32 vacmdlen
 *   40 srcva  48 srclen  56 argv
 */
docopyexec:
	movq	%rdi, 200(%rsp)			/* stash params ptr (survives: plain call/ret) */

	/* create(gpath, OREAD, DMDIR|0700) */
	movq	0(%rdi), %rax
	mov	%rax, 8(%rsp)
	movq	$0, 16(%rsp)			/* OREAD */
	movl	$0x800001c0, 24(%rsp)		/* DMDIR|0700; low 4 bytes only matter
						 * (ARG() reads perm as plain int) */
	mov	$22, %rbp			/* CREATE */
	syscall

	movq	200(%rsp), %rdi
	/* open(ctlpath, ORDWR) */
	movq	8(%rdi), %rax
	mov	%rax, 8(%rsp)
	movq	$2, 16(%rsp)			/* ORDWR */
	mov	$14, %rbp			/* OPEN */
	syscall
	movq	%rax, 72(%rsp)			/* ctlfd */

	movq	200(%rsp), %rdi
	/* pwrite(ctlfd, vacmd, vacmdlen, 0) */
	movq	72(%rsp), %rax
	mov	%rax, 8(%rsp)
	movq	24(%rdi), %rax
	mov	%rax, 16(%rsp)
	movq	32(%rdi), %rax
	mov	%rax, 24(%rsp)
	movq	$0, 32(%rsp)
	mov	$51, %rbp			/* PWRITE */
	syscall

	movq	200(%rsp), %rdi
	/* open(datapath, ORDWR) */
	movq	16(%rdi), %rax
	mov	%rax, 8(%rsp)
	movq	$2, 16(%rsp)			/* ORDWR */
	mov	$14, %rbp			/* OPEN */
	syscall
	movq	%rax, 64(%rsp)			/* datafd */

	movq	200(%rsp), %rdi
	movq	40(%rdi), %rax
	movq	%rax, 80(%rsp)			/* srcva (running) */
	movq	48(%rdi), %rax
	movq	%rax, 88(%rsp)			/* remaining */
	movq	$0, 96(%rsp)			/* curoff */

copyloop:
	movq	88(%rsp), %rax
	cmpq	$0, %rax
	je	copydone
	cmpq	$4096, %rax
	jbe	1f
	movq	$4096, %rax
1:
	movq	%rax, 104(%rsp)			/* chunklen */

	movq	64(%rsp), %rcx
	movq	%rcx, 8(%rsp)			/* fd */
	movq	80(%rsp), %rcx
	movq	%rcx, 16(%rsp)			/* buf */
	movq	%rax, 24(%rsp)			/* len */
	movq	96(%rsp), %rcx
	movq	%rcx, 32(%rsp)			/* off */
	mov	$51, %rbp			/* PWRITE */
	syscall

	movq	104(%rsp), %rcx			/* assume full write, not %rax */
	addq	%rcx, 80(%rsp)
	addq	%rcx, 96(%rsp)
	subq	%rcx, 88(%rsp)
	jmp	copyloop
copydone:

	movq	200(%rsp), %rdi
	/* exec(datapath, argv) */
	movq	16(%rdi), %rax
	mov	%rax, 8(%rsp)
	movq	56(%rdi), %rax
	mov	%rax, 16(%rsp)
	mov	$7, %rbp			/* EXEC */
	syscall

	/* exec failed if we're still here */
	ret

conspath:
	.asciz	"#c/cons"
msg:
	.ascii	"hello from initcode via real exec!\n"
msglen = . - msg

elfgpath:
	.asciz	"#g/t1"
elfctlpath:
	.asciz	"#g/t1/ctl"
elfdatapath:
	.asciz	"#g/t1/data"
.align 8
elfvacmd:
	/*
	 * "va <base> <len>" for the #g ctl file -- base must match
	 * TESTELFVA below, len just needs to cover TESTELFLEN bytes
	 * (rounded up to pages by the kernel). Both are literal text
	 * here, not derived from the .set values below -- keep in sync
	 * by hand if testelf.S / gen-exectest-payloads.sh changes size.
	 */
	.ascii	"va 0x10000000 9184"
elfvacmdend:
elfvacmdlen = elfvacmdend - elfvacmd

gogpath:
	.asciz	"#g/t2"
goctlpath:
	.asciz	"#g/t2/ctl"
godatapath:
	.asciz	"#g/t2/data"
.align 8
govacmd:
	/* keep in sync with TESTGOVA/TESTGOLEN and
	 * gen-exectest-payloads.sh's actual Go binary size */
	.ascii	"va 0x20000000 0x200000"
govacmdend:
govacmdlen = govacmdend - govacmd

.align 8
elfargv:
	.quad	elfargv0str
	.quad	0
elfargv0str:
	.asciz	"t1"

goargv:
	.quad	goargv0str
	.quad	0
goargv0str:
	.asciz	"t2"

.align 8
elfparams:
	.quad	elfgpath, elfctlpath, elfdatapath, elfvacmd
	.quad	elfvacmdlen, TESTELFVA, TESTELFLEN, elfargv
goparams:
	.quad	gogpath, goctlpath, godatapath, govacmd
	.quad	govacmdlen, TESTGOVA, TESTGOLEN, goargv

.set TESTELFVA, 0x500000
.set TESTELFLEN, 9184
.set TESTGOVA, 0x700000
.set TESTGOLEN, 1163928
