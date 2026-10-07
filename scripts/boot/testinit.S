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

	/* port/initcode.c's startboot(): open(cons, OREAD) -> fd 0,
	 * open(cons, OWRITE) -> fd 1, open(cons, OWRITE) -> fd 2. fd 0 must
	 * be readable: rc reads commands from /fd/0 and only goes
	 * interactive when fd 0 is the console */
	lea	conspath(%rip), %rax
	mov	%rax, 8(%rsp)		/* arg0: name */
	movq	$0, 16(%rsp)		/* arg1: mode = OREAD */
	mov	$14, %rbp		/* OPEN */
	syscall
	lea	conspath(%rip), %rax
	mov	%rax, 8(%rsp)
	movq	$1, 16(%rsp)		/* OWRITE */
	mov	$14, %rbp		/* OPEN */
	syscall
	lea	conspath(%rip), %rax
	mov	%rax, 8(%rsp)
	movq	$1, 16(%rsp)		/* OWRITE */
	mov	$14, %rbp		/* OPEN */
	syscall

	/* pwrite(1, msg, msglen, ~0) */
	movq	$1, 8(%rsp)		/* arg0: fd */
	lea	msg(%rip), %rax
	mov	%rax, 16(%rsp)		/* arg1: buf */
	movq	$msglen, 24(%rsp)	/* arg2: len */
	movq	$-1, 32(%rsp)		/* arg3: off = ~0 (append/ignore) */
	mov	$51, %rbp		/* PWRITE */
	syscall

	/* Phase 2.5: rfork(RFPROC|RFFDG|RFREND) WITHOUT RFNAMEG a child that
	 * stages+execs netmount (configures the QEMU SLIRP static IP and
	 * dials+mounts the host's `ufs` 9P server at /mnt -- see
	 * scripts/boot/netmount.c). Omitting RFNAMEG is load-bearing: this
	 * kernel's rfork() shares the parent's namespace group by default,
	 * so netmount's mount() becomes visible to US (the parent) too once
	 * it exits -- that's the whole point, since mount() has no "mount
	 * into a specific other process" mode. */
	movq	$0x2014, 8(%rsp)	/* RFPROC|RFFDG|RFREND, same as fork() */
	mov	$19, %rbp		/* RFORK */
	syscall
	cmpq	$0, %rax
	jne	netmount_wait

	/* child: exec netmount */
	lea	netmountparams(%rip), %rdi
	call	docopyexec
	/* exec failed -- child just exits, parent's await() below unblocks */
	movq	$1, 8(%rsp)
	mov	$8, %rbp		/* EXITS */
	syscall

netmount_wait:
	/* parent: await(buf, n) -- block until the child above exits
	 * (success or failure), so /mnt is settled (or definitely not
	 * going to be) before we try to exec from it. Message content
	 * is irrelevant, just need the blocking wait. */
	leaq	360(%rsp), %rax
	mov	%rax, 8(%rsp)		/* arg0: buf */
	movq	$128, 16(%rsp)		/* arg1: n */
	mov	$47, %rbp		/* AWAIT */
	syscall

	/* Try the real rc shell binary, now from the REAL 9P-mounted path
	 * (/mnt/bin/rc, with the REAL /mnt/rc/lib/rcmain -- see
	 * scripts/boot/netmount.c and plan Phase 2.5) instead of the old #g-copy-
	 * of-our-own-embedded-binary + pipe-staged-fake-rcmain mechanism.
	 * argv is now fully static: the mount path is fixed at build time,
	 * unlike the old pipe-fd-based "-m #d/<N>" which had to be built at
	 * runtime (see the now-removed stagepipe, still below as a fallback
	 * reference/regression path). A successful exec() here replaces our
	 * process image and never returns, so rc runs with the CPU entirely
	 * to itself, reading interactively from/prompting on the console
	 * (fd 0/1/2, opened above) via its "-i" flag. */
	lea	realrcpath(%rip), %rax
	mov	%rax, 8(%rsp)		/* arg0: name */
	lea	realrcargv(%rip), %rax
	mov	%rax, 16(%rsp)		/* arg1: argv */
	mov	$7, %rbp		/* EXEC */
	syscall

	/* real rc exec failed (netmount/ufs mount likely didn't come up) --
	 * fall back to the old pipe-staged-fake-rcmain mechanism, so this
	 * harness still demonstrates a CLI even without working networking */
	jmp	stagepipe
stagepipe_done:
	leaq	290(%rsp), %rdi		/* dynrcparams, built by stagepipe */
	call	docopyexec

	/* rc's exec failed if we get here -- fall back to the real init
	 * binary (sys/src/cmd/init/build.json), same rationale as above */
	lea	initparams(%rip), %rdi
	call	docopyexec

	/* init's exec failed if we get here -- fall back to the original
	 * elf/go loader regression tests so those still get exercised */

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

/*
 * stagepipe: create a pipe, write rcmainscript into the write end, close
 * it (signaling EOF), and build "#d/<readfd>" + a patched argv + a
 * patched params block for rc's exec, all in stack scratch space.
 *
 * Originally this staged rcmainscript into a #g (devsegment) segment,
 * same as the ELF/Go/init test payloads. That broke rc's lexer: a
 * devsegment "data" file has no notion of length shorter than its
 * page-rounded reservation -- reading it always returns the FULL
 * reserved size (our ~50-byte script followed by thousands of zero
 * bytes), with no EOF until the whole segment is consumed. Feeding
 * that trailing run of NULs into rc's lexer as if it were more script
 * source corrupted its state (manifested as a NULL-pointer write deep
 * in lex.c's nextc(), long after the real content had been consumed).
 * A pipe has proper stream semantics instead: once we close the write
 * end, a read past our content returns 0 immediately.
 *
 * "#d/<N>" (the fd-namespace device's hardware name) is used directly
 * rather than "/fd/<N>", since "/fd" is only reachable if something
 * has bound "#d" onto it -- not the case in this raw, namespace-less
 * bootstrap environment, whereas "#d" itself is always reachable.
 *
 * Stack layout used here (all offsets below our existing readable/
 * writable scratch area, clear of docopyexec's own 0-112ish and
 * 200(%rsp) usage):
 *   220(%rsp) fdarr[2]       (pipe's read/write fds, int32 each)
 *   230(%rsp) pathbuf[8]     ("#d/NN\0")
 *   240(%rsp) dynrcargv[5]   (quads: "rc","-i","-m",&pathbuf,0)
 *   290(%rsp) dynrcparams[8] (copy of rcparams w/ argv -> dynrcargv)
 *
 * Reached via jmp and left via jmp back (to stagepipe_done), not
 * call/ret: entering via `call` would shift %rsp by -8 for everything
 * below, making these same fixed offsets land 8 bytes away from where
 * start's own code reads them back afterward -- exactly the kind of
 * caller/callee frame mismatch Bug 3 (see MEMORY.md) was about, just
 * self-inflicted in new test-harness code this time.
 */
stagepipe:
	/* pipe(fdarr) */
	leaq	220(%rsp), %rax
	mov	%rax, 8(%rsp)
	mov	$21, %rbp			/* PIPE */
	syscall

	/* pwrite(fdarr[1], rcmainscript, rcmainscriptlen, 0) */
	movl	224(%rsp), %eax
	mov	%rax, 8(%rsp)
	lea	rcmainscript(%rip), %rax
	mov	%rax, 16(%rsp)
	movq	$rcmainscriptlen, 24(%rsp)
	movq	$0, 32(%rsp)
	mov	$51, %rbp			/* PWRITE */
	syscall

	/* close(fdarr[1]) -- signal EOF to the read end */
	movl	224(%rsp), %eax
	mov	%rax, 8(%rsp)
	mov	$4, %rbp			/* CLOSE */
	syscall

	/* build "#d/<readfd>" into pathbuf, up to 2 digits (plenty: we're
	 * at most a handful of fds in at this point) */
	movb	$'#', 230(%rsp)
	movb	$'d', 231(%rsp)
	movb	$'/', 232(%rsp)
	movl	220(%rsp), %eax
	cmpl	$10, %eax
	jb	1f
	xorl	%edx, %edx
	movl	$10, %ecx
	divl	%ecx
	addb	$'0', %al
	movb	%al, 233(%rsp)
	movl	%edx, %eax
	addb	$'0', %al
	movb	%al, 234(%rsp)
	movb	$0, 235(%rsp)
	jmp	2f
1:
	addb	$'0', %al
	movb	%al, 233(%rsp)
	movb	$0, 234(%rsp)
2:
	/* dynrcargv = {"rc", "-i", "-m", &pathbuf, 0} */
	lea	rcargv0str(%rip), %rax
	movq	%rax, 240(%rsp)
	lea	rcargv1str(%rip), %rax
	movq	%rax, 248(%rsp)
	lea	rcargv2str(%rip), %rax
	movq	%rax, 256(%rsp)
	leaq	230(%rsp), %rax
	movq	%rax, 264(%rsp)
	movq	$0, 272(%rsp)

	/* dynrcparams = rcparams, but with argv (field 7) -> dynrcargv */
	lea	rcparams(%rip), %rsi
	movq	0(%rsi), %rax
	movq	%rax, 290(%rsp)
	movq	8(%rsi), %rax
	movq	%rax, 298(%rsp)
	movq	16(%rsi), %rax
	movq	%rax, 306(%rsp)
	movq	24(%rsi), %rax
	movq	%rax, 314(%rsp)
	movq	32(%rsi), %rax
	movq	%rax, 322(%rsp)
	movq	40(%rsi), %rax
	movq	%rax, 330(%rsp)
	movq	48(%rsi), %rax
	movq	%rax, 338(%rsp)
	leaq	240(%rsp), %rax
	movq	%rax, 346(%rsp)
	jmp	stagepipe_done

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

initgpath:
	.asciz	"#g/t3"
initctlpath:
	.asciz	"#g/t3/ctl"
initdatapath:
	.asciz	"#g/t3/data"
.align 8
initvacmd:
	/* generous upper bound, doesn't need to match TESTINITLEN exactly
	 * (see elfvacmd's comment above) -- init.c + libc/libauth/libbio
	 * currently links to ~2.3MB */
	.ascii	"va 0x1000000 0x400000"
initvacmdend:
initvacmdlen = initvacmdend - initvacmd

.align 8
initargv:
	.quad	initargv0str
	.quad	initargv1str
	.quad	0
initargv0str:
	.asciz	"init"
initargv1str:
	.asciz	"-t"

rcgpath:
	.asciz	"#g/t4"
rcctlpath:
	.asciz	"#g/t4/ctl"
rcdatapath:
	.asciz	"#g/t4/data"
.align 8
rcvacmd:
	/* rc is ~2.5MB; generous upper bound as elsewhere */
	.ascii	"va 0x1800000 0x400000"
rcvacmdend:
rcvacmdlen = rcvacmdend - rcvacmd

.align 8
rcargv:
	.quad	rcargv0str
	.quad	rcargv1str
	.quad	rcargv2str
	.quad	rcargv3str
	.quad	0
rcargv0str:
	.asciz	"rc"
rcargv1str:
	.asciz	"-i"
rcargv2str:
	.asciz	"-m"
rcargv3str:
	.asciz	"#g/t5/data"

.align 8
rcmainscript:
	/*
	 * Minimal stand-in for /rc/lib/rcmain (see stagepipe above):
	 * defines sigexit so rc's bootstrap Xexit runs this instead of
	 * calling exits(). Currently just an echo, to isolate the core
	 * Xexit->sigexit mechanism from the separate, still-open question
	 * of why ". -i #c/cons" crashes afterward (see MEMORY.md).
	 */
	.ascii	"fn sigexit {\n\techo hello-from-minimal-cli-rc-shell\n}\n"
rcmainscriptend:
rcmainscriptlen = rcmainscriptend - rcmainscript

netmountgpath:
	.asciz	"#g/t6"
netmountctlpath:
	.asciz	"#g/t6/ctl"
netmountdatapath:
	.asciz	"#g/t6/data"
.align 8
netmountvacmd:
	/* generous upper bound, same pattern as init/rc above --
	 * netmount currently links to ~2.1MB */
	.ascii	"va 0x2000000 0x400000"
netmountvacmdend:
netmountvacmdlen = netmountvacmdend - netmountvacmd

.align 8
netmountargv:
	.quad	netmountargv0str
	.quad	0
netmountargv0str:
	.asciz	"netmount"

realrcpath:
	.asciz	"/mnt/bin/rc"
realrcmpath:
	.asciz	"/mnt/rc/lib/rcmain"
.align 8
realrcargv:
	.quad	realrcargv0str
	.quad	realrcargv1str
	.quad	realrcargv2str
	.quad	realrcmpath
	.quad	0
realrcargv0str:
	.asciz	"rc"
realrcargv1str:
	.asciz	"-i"
realrcargv2str:
	.asciz	"-m"

.align 8
elfparams:
	.quad	elfgpath, elfctlpath, elfdatapath, elfvacmd
	.quad	elfvacmdlen, TESTELFVA, TESTELFLEN, elfargv
goparams:
	.quad	gogpath, goctlpath, godatapath, govacmd
	.quad	govacmdlen, TESTGOVA, TESTGOLEN, goargv
initparams:
	.quad	initgpath, initctlpath, initdatapath, initvacmd
	.quad	initvacmdlen, TESTINITVA, TESTINITLEN, initargv
rcparams:
	/* template only -- stagepipe copies fields 0-6 into dynrcparams
	 * (stack scratch), overriding field 7 (argv) with dynrcargv, since
	 * the real "-m" path is only known at runtime (see stagepipe) */
	.quad	rcgpath, rcctlpath, rcdatapath, rcvacmd
	.quad	rcvacmdlen, TESTRCVA, TESTRCLEN, rcargv
netmountparams:
	.quad	netmountgpath, netmountctlpath, netmountdatapath, netmountvacmd
	.quad	netmountvacmdlen, TESTNETMOUNTVA, TESTNETMOUNTLEN, netmountargv

.set TESTELFVA, 0x500000
.set TESTELFLEN, 9184
.set TESTGOVA, 0x700000
.set TESTGOLEN, 1163928
.set TESTINITVA, 0x1000000
.set TESTINITLEN, 2403192
.set TESTRCVA, 0x1800000
.set TESTRCLEN, 2521608
.set TESTNETMOUNTVA, 0x2000000
.set TESTNETMOUNTLEN, 2224272
