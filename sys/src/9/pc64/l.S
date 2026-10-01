#include "mem.h"

/*
 * GCC port register/ABI notes (see AGENTS.md / MEMORY.md):
 *
 * - RMACH (%r15) and RUSER (%r14) are unconditional kernel-internal
 *   aliases, defined in mem.h, unchanged in meaning here.
 *
 * - Plan9's amd64 6c passes a hand-written asm function's first
 *   argument in BP ("RARG"), remaining arguments on the stack at
 *   name+8(FP), name+16(FP), ... Under gcc/System V the caller instead
 *   passes up to 6 integer/pointer args in %rdi,%rsi,%rdx,%rcx,%r8,%r9
 *   and nothing on the stack. For every ordinary C-callable function
 *   below: arg1 (was RARG/BP) -> %rdi, arg2 (was +8(FP)) -> %rsi,
 *   arg3 (was +16(FP)) -> %rdx, etc. This is applied per function by
 *   reading its original FP offsets, not mechanically.
 *
 * - Exception: touser, syscallentry, forkret, _strayintr/_strayintrx/
 *   _intrcommon, noteret, vectortable are reached via SYSCALL/SYSRET/
 *   IRETQ or the IDT vector table, not an ordinary call; register
 *   uses there that are NOT "argument N" are translated literally
 *   (BP -> %rbp as a plain register). Where such a function turns
 *   around and calls a real C function (syscallentry -> syscall(),
 *   _intrcommon -> trap()), that specific call's argument setup does
 *   follow the System V mapping above -- marked inline below.
 *
 * - A few functions (rdmsr/wrmsr/peek's fallthrough labels, _fldcw,
 *   _ldmxcsr, vmxon/vmclear/vmptrld/invept/invvpid, vmlaunch) relied
 *   on a spare caller-provided stack slot (from the old ABI's "every
 *   argument gets a stack slot even if also passed in BP") as scratch
 *   memory. System V provides no such slot, so these explicitly
 *   allocate one (subq/addq around the memory-operand instruction),
 *   each marked "GCC port:".
 */

#define DELAY		.byte 0xEB; .byte 0x00	/* JMP .+2 */

#define pFARJMP32(s, o)	.byte 0xea;		/* far jump to ptr32:16 */ \
			.long o; .word s

.code32

/*
 * Enter here in 32-bit protected mode. Welcome to 1982.
 * Make sure the GDT is set as it should be:
 *	disable interrupts;
 *	load the GDT with the table in _gdt32p;
 *	load all the data segments
 *	load the code segment via a far jump.
 */
.globl _protected			/* GCC port: ENTRY(_protected) in kernel.ld needs this visible */
_protected:
	cli

	movl	$_gdtptr32p-KZERO, %eax
	lgdt	(%eax)

	movl	$SELECTOR(2, SELGDT, 0), %eax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss

	pFARJMP32(SELECTOR(3, SELGDT, 0), _warp64-KZERO)

/*
 * Must be 4-byte aligned (GCC port: GAS doesn't auto-align TEXT symbols
 * the way 6a did, so this needs an explicit directive rather than the
 * original single alignment byte).
 */
	.balign 4
_multibootheader:
	.long	0x1BADB002			/* magic */
	.long	0x00010007			/* flags */
	.long	-(0x1BADB002 + 0x00010007)	/* checksum */
	.long	_multibootheader-KZERO		/* header_addr */
	.long	_protected-KZERO		/* load_addr */
	.long	edata-KZERO			/* load_end_addr */
	.long	end-KZERO			/* bss_end_addr */
	.long	_multibootentry-KZERO		/* entry_addr */
	.long	0				/* mode_type */
	.long	0				/* width */
	.long	0				/* height */
	.long	32				/* depth */

/*
 * the kernel expects the data segment to be page-aligned.
 * 8l/6l pack .data immediately after .text with no gap, so the original
 * Plan 9 code here shifted that tightly-packed block up to a page boundary
 * at boot (see git history for the byte-shuffle this replaces). GCC port:
 * kernel.ld's linker script page-aligns .data directly (ADDR(.data) is
 * already page-aligned, matching where the old code used to move it), so
 * there's nothing left to relocate -- just stash the Multiboot info
 * pointer and fall into _protected.
 */
_multibootentry:
	movl	%ebx, multibootptr-KZERO
	movl	$_protected-KZERO, %eax
	jmp	*%eax

/* multiboot structure pointer (physical address) */
.globl multibootptr
multibootptr:
	.long	0

_gdt:
	/* null descriptor */
	.long	0
	.long	0

	/* (KESEG) 64 bit long mode exec segment */
	.long	(0xFFFF)
	.long	(SEGL|SEGG|SEGP|(0xF<<16)|SEGPL(0)|SEGEXEC|SEGR)

	/* 32 bit data segment descriptor for 4 gigabytes (PL 0) */
	.long	(0xFFFF)
	.long	(SEGG|SEGB|(0xF<<16)|SEGP|SEGPL(0)|SEGDATA|SEGW)

	/* 32 bit exec segment descriptor for 4 gigabytes (PL 0) */
	.long	(0xFFFF)
	.long	(SEGG|SEGD|(0xF<<16)|SEGP|SEGPL(0)|SEGEXEC|SEGR)


_gdtptr32p:
	.word	(4*8-1)
	.long	_gdt-KZERO

_gdtptr64p:
	.word	(4*8-1)
	.quad	_gdt-KZERO

_gdtptr64v:
	.word	(4*8-1)
	.quad	_gdt

/*
 * Macros for accessing page table entries; change the
 * C-style array-index macros into a page table byte offset
 */
#define PML4O(v)	((PTLX((v), 3))<<3)
#define PDPO(v)		((PTLX((v), 2))<<3)
#define PDO(v)		((PTLX((v), 1))<<3)
#define PTO(v)		((PTLX((v), 0))<<3)

_warp64:

	/* clear mach and page tables */
	movl	$((CPU0END-CPU0PML4)>>2), %ecx
	movl	$(CPU0PML4-KZERO), %esi
	movl	%esi, %edi
	xorl	%eax, %eax
	cld
	rep	stosl

	movl	%esi, %eax			/* PML4 */
	movl	%eax, %edx
	addl	$(PTEACCESSED|PTEDIRTY|PTSZ|PTEWRITE|PTEVALID), %edx	/* PDP at PML4 + PTSZ */
	movl	%edx, PML4O(0)(%eax)		/* PML4E for double-map */
	movl	%edx, PML4O(KZERO)(%eax)	/* PML4E for KZERO */

	addl	$PTSZ, %eax			/* PDP at PML4 + PTSZ */
	addl	$PTSZ, %edx			/* PD0 at PML4 + 2*PTSZ */
	movl	%edx, PDPO(0)(%eax)		/* PDPE for double-map */
	movl	%edx, PDPO(KZERO)(%eax)		/* PDPE for KZERO */

	/*
	 * add PDPE for KZERO+1GB early as Vmware
	 * hangs when modifying kernel PDP
	 */
	addl	$PTSZ, %edx			/* PD1 */
	movl	%edx, PDPO(KZERO+GiB)(%eax)

	addl	$PTSZ, %eax			/* PD0 at PML4 + 2*PTSZ */
	movl	$(PTEACCESSED|PTEDIRTY|PTESIZE|PTEGLOBAL|PTEWRITE|PTEVALID), %edx
	movl	%edx, PDO(0)(%eax)		/* PDE for double-map */

	/*
	 * map from KZERO to end using 2MB pages
	 */
	addl	$PDO(KZERO), %eax
	movl	$end-KZERO, %ecx

	addl	$(16*1024), %ecx		/* qemu puts multiboot data after the kernel */

	addl	$(PGLSZ(1)-1), %ecx
	andl	$~(PGLSZ(1)-1), %ecx
	movl	%ecx, MemMin-KZERO		/* see memory.c */
	shrl	$(1*PTSHIFT+PGSHIFT), %ecx
memloop:
	movl	%edx, (%eax)
	addl	$PGLSZ(1), %edx
	addl	$8, %eax
	loop	memloop

/*
 * Enable and activate Long Mode. From the manual:
 * 	make sure Page Size Extentions are off, and Page Global
 *	Extensions and Physical Address Extensions are on in CR4;
 *	set Long Mode Enable in the Extended Feature Enable MSR;
 *	set Paging Enable in CR0;
 *	make an inter-segment jump to the Long Mode code.
 * It's all in 32-bit mode until the jump is made.
 */
_lme:
	movl	%esi, %cr3			/* load the mmu */
	DELAY

	movl	%cr4, %eax
	andl	$~0x00000010, %eax		/* Page Size */
	orl	$0x000000A0, %eax		/* Page Global, Phys. Address */
	movl	%eax, %cr4

	movl	$0xc0000080, %ecx		/* Extended Feature Enable */
	rdmsr
	orl	$0x00000100, %eax		/* Long Mode Enable */
	wrmsr

	movl	%cr0, %edx
	andl	$~0x6000000a, %edx
	orl	$0x80010000, %edx		/* Paging Enable, Write Protect */
	movl	%edx, %cr0

	pFARJMP32(SELECTOR(KESEG, SELGDT, 0), _identity-KZERO)

/*
 * Long mode. Welcome to 2003.
 * Jump out of the identity map space;
 * load a proper long mode GDT.
 */
.code64

_identity:
	movq	$_start64v, %rax
	jmp	*%rax

_start64v:
	movq	$_gdtptr64v, %rax
	lgdt	(%rax)

	xorq	%rax, %rax
	movw	%ax, %ds			/* not used in long mode */
	movw	%ax, %es			/* not used in long mode */
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss			/* not used in long mode */

	lldt	%ax

	movq	$(CPU0MACH+MACHSIZE), %rsp
	movq	$(CPU0MACH), RMACH
	movq	%rax, RUSER			/* up = 0; */

_clearbss:
	movq	$edata, %rdi
	movq	$end, %rcx
	subq	%rdi, %rcx			/* end-edata bytes */
	shrq	$2, %rcx			/* end-edata doublewords */

	cld
	rep	stosl				/* clear BSS */

	/*
	 * GCC port: MemMin lives in BSS (between edata and end), so the
	 * clear above just wiped out the value _warp64 computed into it
	 * earlier (while still in 32-bit mode, before paging). The
	 * original source has this same ordering (BSS cleared after
	 * MemMin is set) and presumably gets away with it only because
	 * 6l/8l happen to place MemMin outside [edata,end) under their
	 * own layout; GNU ld's doesn't, so it's provably zero here by
	 * the time meminit0() reads it (confirmed: panics "kernel too
	 * big" since MemMin(0) < PADDR(PGROUND(end))). Recompute the
	 * exact same value _warp64 used -- see the "map from KZERO to
	 * end" block above -- now in 64-bit/virtual-addressing form
	 * since paging is already active here.
	 */
	movq	$end-KZERO, %rdx		/* %rax must stay 0 -- used below to clear flags */
	addq	$(16*1024), %rdx		/* qemu puts multiboot data after the kernel */
	addq	$(PGLSZ(1)-1), %rdx
	andq	$~(PGLSZ(1)-1), %rdx
	movl	%edx, MemMin

	pushq	%rax				/* clear flags */
	popfq

	call	main

/*
 * Park a processor. Should never fall through a return from main to here,
 * should only be called by application processors when shutting down.
 */
.globl idle
idle:
_idle:
	sti
	hlt
	jmp	_idle

/*
 * The CPUID instruction is always supported on the amd64.
 */
.globl cpuid
cpuid:
	pushq	%rbp				/* GCC port: %rbp is callee-saved under
						 * System V (unlike Plan9's convention) --
						 * save/restore the caller's value around
						 * using it as scratch below. */
	pushq	%rbx				/* GCC port: %rbx is also callee-saved --
						 * the CPUID instruction itself clobbers it
						 * (its result lands in %ebx), so shield the
						 * caller's value the same way. */
	movq	%rdx, %rbp			/* info (arg3); save before CPUID clobbers %rdx */
	movl	%edi, %eax			/* function (arg1) */
	movl	%esi, %ecx			/* sub-level (arg2) */
	cpuid

	movl	%eax, 0(%rbp)
	movl	%ebx, 4(%rbp)
	movl	%ecx, 8(%rbp)
	movl	%edx, 12(%rbp)
	popq	%rbx
	popq	%rbp
	ret

/*
 * Port I/O.
 */
.globl inb
inb:
	movl	%edi, %edx			/* port (arg1) */
	xorl	%eax, %eax
	inb	%dx, %al
	ret

.globl insb
insb:
	movl	%edx, %ecx			/* count (arg3), before %edx is overwritten */
	movl	%edi, %edx			/* port (arg1) */
	movq	%rsi, %rdi			/* address (arg2) */
	cld
	rep	insb
	ret

.globl ins
ins:
	movl	%edi, %edx			/* port (arg1) */
	xorl	%eax, %eax
	inw	%dx, %ax
	ret

.globl inss
inss:
	movl	%edx, %ecx			/* count (arg3), before %edx is overwritten */
	movl	%edi, %edx			/* port (arg1) */
	movq	%rsi, %rdi			/* address (arg2) */
	cld
	rep	insw
	ret

.globl inl
inl:
	movl	%edi, %edx			/* port (arg1) */
	inl	%dx, %eax
	ret

.globl insl
insl:
	movl	%edx, %ecx			/* count (arg3), before %edx is overwritten */
	movl	%edi, %edx			/* port (arg1) */
	movq	%rsi, %rdi			/* address (arg2) */
	cld
	rep	insl
	ret

.globl outb
outb:
	movl	%edi, %edx			/* port (arg1) */
	movl	%esi, %eax			/* byte (arg2) */
	outb	%al, %dx
	ret

.globl outsb
outsb:
	movl	%edx, %ecx			/* count (arg3), before %edx is overwritten */
	movl	%edi, %edx			/* port (arg1); address (arg2) already in %rsi for OUTSB */
	cld
	rep	outsb
	ret

.globl outs
outs:
	movl	%edi, %edx			/* port (arg1) */
	movl	%esi, %eax			/* short (arg2) */
	outw	%ax, %dx
	ret

.globl outss
outss:
	movl	%edx, %ecx			/* count (arg3), before %edx is overwritten */
	movl	%edi, %edx			/* port (arg1); address (arg2) already in %rsi for OUTSW */
	cld
	rep	outsw
	ret

.globl outl
outl:
	movl	%edi, %edx			/* port (arg1) */
	movl	%esi, %eax			/* long (arg2) */
	outl	%eax, %dx
	ret

.globl outsl
outsl:
	movl	%edx, %ecx			/* count (arg3), before %edx is overwritten */
	movl	%edi, %edx			/* port (arg1); address (arg2) already in %rsi for OUTSL */
	cld
	rep	outsl
	ret

.globl getgdt
getgdt:
	sgdt	(%rdi)				/* Note: 10 bytes returned */
	ret

.globl lgdt
lgdt:						/* GDTR - global descriptor table */
	lgdt	(%rdi)
	ret

.globl lidt
lidt:						/* IDTR - interrupt descriptor table */
	lidt	(%rdi)
	ret

.globl ltr
ltr:
	movw	%di, %ax
	ltr	%ax
	ret

/*
 * Read/write various system registers.
 */
.globl getcr0
getcr0:						/* Processor Control */
	movq	%cr0, %rax
	ret

.globl putcr0
putcr0:
	movq	%rdi, %cr0
	ret

.globl getcr2
getcr2:						/* #PF Linear Address */
	movq	%cr2, %rax
	ret

.globl putcr2
putcr2:
	movq	%rdi, %cr2
	ret

.globl getcr3
getcr3:						/* PML4 Base */
	movq	%cr3, %rax
	ret

.globl putcr3
putcr3:
	movq	%rdi, %cr3
	ret

.globl getcr4
getcr4:						/* Extensions */
	movq	%cr4, %rax
	ret

.globl putcr4
putcr4:
	movq	%rdi, %cr4
	ret

.globl getxcr0
getxcr0:					/* XCR0 - extended control */
	xorq	%rcx, %rcx
	.word 0x010f; .byte 0xd0	// XGETBV
	shlq	$32, %rdx
	orq	%rdx, %rax
	ret

.globl putxcr0
putxcr0:
	xorq	%rcx, %rcx
	movl	%edi, %edx		/* GCC port: preserves the original's MOVL-then-SHRQ, which nets %rdx=0 */
	shrq	$32, %rdx
	movl	%edi, %eax		/* value (arg1), low 32 bits */
	.word 0x010f; .byte 0xd1	// XSETBV
	ret

.globl mb386
mb386:						/* hack */
.globl mb586
mb586:
	xorl	%eax, %eax
	cpuid
	ret

/*
 * Basic timing loop.
 */
.globl delayloop
delayloop:
	/*
	 * GCC port: %rbx is callee-saved under System V -- gcc-compiled
	 * callers (e.g. hpetcpufreq()'s "loops") keep live values there
	 * across this call, trusting the ABI. Save/restore it instead of
	 * clobbering it as scratch.
	 */
	pushq	%rbx
	movl	%edi, %ecx			/* arg1 */
	movl	$1, %ebx
	movl	$0, %edx
	movl	$-1, %eax
_delayloop:
	idivq	%rbx
	idivq	%rbx
	idivq	%rbx
	idivq	%rbx
	idivq	%rbx
	idivq	%rbx
	idivq	%rbx
	idivq	%rbx
	loop	_delayloop
	popq	%rbx
	ret

.globl _cycles
_cycles:					/* time stamp counter */
	rdtsc
	movl	%eax, 0(%rdi)			/* lo */
	movl	%edx, 4(%rdi)			/* hi */
	ret

/*
 * GCC port: amd64 port of pc/l.s's lcycles -- low-order 32 bits of the
 * cycle counter, returned directly (not via an out-pointer like _cycles
 * above); rdtsc already leaves that in %eax, which is also the System V
 * 32-bit return register, so this is a straight RDTSC;RET like the 386
 * original.
 */
.globl lcycles
lcycles:
	rdtsc
	ret

.globl rdmsr
rdmsr:						/* Model-Specific Register */
	movl	%edi, %ecx			/* index (arg1) */
	pushq	%rbp				/* GCC port: %rbp is caller-saved under
						 * Plan9's convention but callee-saved under
						 * System V -- trap.c's _rdmsrinst/_wrmsrinst
						 * fault recovery writes -1 into the saved
						 * Ureg's bp field, which lands back in %rbp
						 * when the trap returns; save/restore the
						 * caller's %rbp around that so it isn't
						 * clobbered on every call. */
	movq	$0, %rbp
.globl _rdmsrinst
_rdmsrinst:
	rdmsr
	movl	%eax, 0(%rsi)			/* lo; vlong (arg2) already in %rsi */
	movl	%edx, 4(%rsi)			/* hi */
	movq	%rbp, %rax			/* %rbp set to -1 if trapped */
	popq	%rbp
	ret

.globl wrmsr
wrmsr:
	movl	%edi, %ecx			/* index (arg1) */
	movl	%esi, %eax			/* value (arg2), low 32 bits */
	movq	%rsi, %rdx
	shrq	$32, %rdx			/* value (arg2), high 32 bits */
	pushq	%rbp				/* GCC port: see rdmsr above */
	movq	$0, %rbp
.globl _wrmsrinst
_wrmsrinst:
	wrmsr
	movq	%rbp, %rax			/* %rbp set to -1 if trapped */
	popq	%rbp
	ret

/* fault-proof memcpy */
.globl peek
peek:
	xchgq	%rdi, %rsi			/* arg1 (src) -> %rsi, arg2 (dst) -> %rdi, for MOVSB's SI=src/DI=dst */
	movl	%edx, %ecx			/* cnt (arg3) */
	cld
.globl _peekinst
_peekinst:
	rep	movsb
	movl	%ecx, %eax
	ret

.globl invlpg
invlpg:
	invlpg	(%rdi)
	ret

.globl wbinvd
wbinvd:
	wbinvd
	ret

/*
 * Serialisation.
 */
.globl lfence
lfence:
	lfence
	ret

.globl mfence
mfence:
	mfence
	ret

.globl sfence
sfence:
	sfence
	ret

/*
 * Note: CLI and STI are not serialising instructions.
 * Is that assumed anywhere?
 */
.globl splhi
splhi:
_splhi:
	pushq	%rbx				/* GCC port: %rbx is callee-saved under
						 * System V -- shield the caller's value
						 * while using it as scratch below (this
						 * runs on essentially every lock/ilock). */
	pushfq
	popq	%rax
	testq	$0x200, %rax			/* 0x200 - Interrupt Flag */
	jz	_alreadyhi

	movq	8(%rsp), %rbx			/* return PC (now past the rbx push) */
	movq	%rbx, 8(RMACH)			/* save PC in m->splpc */
	cli
_alreadyhi:
	popq	%rbx
	ret

.globl spllo
spllo:
_spllo:
	pushfq
	popq	%rax
	testq	$0x200, %rax			/* 0x200 - Interrupt Flag */
	jne	_alreadylo

	movq	$0, 8(RMACH)			/* clear m->splpc */
	sti
_alreadylo:
	ret

.globl splx
splx:
	testq	$0x200, %rdi			/* 0x200 - Interrupt Flag */
	jne	_spllo
	jmp	_splhi

.globl spldone
spldone:
	ret

.globl islo
islo:
	pushfq
	popq	%rax
	andq	$0x200, %rax			/* 0x200 - Interrupt Flag */
	ret

/*
 * Synchronisation
 */
.globl tas
tas:
.globl _tas
_tas:
	movl	$0xdeaddead, %eax
	xchgl	%eax, (%rdi)
	ret

.globl cmpswap486
cmpswap486:
.globl cas
cas:
	/*
	 * GCC port: stage "new" in %r10d, not %ebx -- %rbx is callee-saved
	 * under System V and this hot path (called from essentially every
	 * refcount/lock) must not clobber it; %r10 is caller-saved and free.
	 */
	movl	%esi, %eax			/* exp (arg2) */
	movl	%edx, %r10d			/* new (arg3) */
	lock; cmpxchgl %r10d, (%rdi)
	movl	$1, %eax
	jne	_cas32r0
_cas32r1:
	ret
_cas32r0:
	decl	%eax
	ret

/*
 * Label consists of a stack pointer and a programme counter
 */
.globl gotolabel
gotolabel:
	/*
	 * GCC port: restore %rbp (Label.bp) too -- see the comment on
	 * struct Label (pc64/dat.h) for why: we build with
	 * -fno-omit-frame-pointer, so gcc-compiled functions address
	 * their locals %rbp-relative, and this jump bypasses every
	 * intervening function's normal epilogue that would otherwise
	 * have restored it.
	 */
	movq	16(%rdi), %rbp			/* restore BP */
	movq	0(%rdi), %rsp			/* restore SP */
	movq	8(%rdi), %rax			/* put return PC on the stack */
	movq	%rax, 0(%rsp)
	movl	$1, %eax			/* return 1 */
	ret

.globl setlabel
setlabel:
	/*
	 * GCC port: stage the return PC in %rax, not %rbx -- %rbx is
	 * callee-saved under System V; %rax is about to be overwritten
	 * with the return value anyway, so it's free scratch here.
	 * Also save %rbp (Label.bp) -- see gotolabel().
	 */
	movq	%rsp, 0(%rdi)			/* store SP */
	movq	0(%rsp), %rax			/* store return PC */
	movq	%rax, 8(%rdi)
	movq	%rbp, 16(%rdi)			/* store BP */
	movl	$0, %eax			/* return 0 */
	ret

.globl halt
halt:
	cli
	cmpl	$0, nrdy
	je	_nothingready
	sti
	ret
_nothingready:
	sti
	hlt
	ret

.globl mwait
mwait:
	movq	%rdi, %rax
	movl	(%rax), %ecx
	orl	%ecx, %ecx
	jne	_mwaitdone
	xorl	%edx, %edx
	.byte 0x0f; .byte 0x01; .byte 0xc8	/* MONITOR */
	movl	(%rax), %ecx
	orl	%ecx, %ecx
	jne	_mwaitdone
	xorl	%eax, %eax
	.byte 0x0f; .byte 0x01; .byte 0xc9	/* MWAIT */
_mwaitdone:
	ret

/*
 * SIMD Floating Point.
 */
.globl _clts
_clts:
	clts
	ret

.globl _fldcw
_fldcw:
	/* GCC port: no caller-provided stack slot under System V; use one of our own */
	subq	$8, %rsp
	movw	%di, (%rsp)
	fldcw	(%rsp)
	addq	$8, %rsp
	ret

.globl _fnclex
_fnclex:
	fnclex
	ret

.globl _fninit
_fninit:
	fninit					/* no WAIT */
	ret

.globl _fxrstor
_fxrstor:
	fxrstor64 (%rdi)
	ret

.globl _fxsave
_fxsave:
	fxsave64 (%rdi)
	ret

.globl _xrstor
_xrstor:
	movl	$7, %eax
	xorl	%edx, %edx
	xrstor64 (%rdi)
	ret

.globl _xrstors
_xrstors:
	movl	$7, %eax
	xorl	%edx, %edx
	xrstors64 (%rdi)
	ret

.globl _xsave
_xsave:
	movl	$7, %eax
	xorl	%edx, %edx
	xsave64 (%rdi)
	ret

.globl _xsaveopt
_xsaveopt:
	movl	$7, %eax
	xorl	%edx, %edx
	xsaveopt64 (%rdi)
	ret

.globl _xsaves
_xsaves:
	movl	$7, %eax
	xorl	%edx, %edx
	xsaves64 (%rdi)
	ret

.globl _fwait
_fwait:
	fwait
	ret

.globl _ldmxcsr
_ldmxcsr:					/* Load MXCSR */
	/* GCC port: no caller-provided stack slot under System V; use one of our own */
	subq	$8, %rsp
	movl	%edi, (%rsp)
	ldmxcsr	(%rsp)
	addq	$8, %rsp
	ret

.globl _stts
_stts:
	movq	%cr0, %rax
	orq	$8, %rax			/* Ts */
	movq	%rax, %cr0
	ret

.globl mul64fract
mul64fract:
	movq	%rsi, %rax			/* a (arg2) */
	mulq	%rdx				/* a*b; b (arg3) */
	shrdq	$32, %rdx, %rax
	movq	%rax, (%rdi)			/* r (arg1) */
	ret

.globl rdrand32
rdrand32:
loop32:
	.byte 0x0f; .byte 0xc7; .byte 0xf0
	jnc	loop32
	ret

.globl rdrand64
rdrand64:
loop64:
	.byte 0x48; .byte 0x0f; .byte 0xc7; .byte 0xf0
	jnc	loop64
	ret

.globl rdrandbuf
rdrandbuf:
	movq	%rdi, %rdx			/* buf (arg1) */

	movl	%esi, %ecx			/* cnt (arg2), zero-extends to 64 bits */
	shrq	$3, %rcx
eights:
	cmpl	$0, %ecx
	jle	f1
	call	rdrand64
	movq	%rax, 0(%rdx)
	addq	$8, %rdx
	subl	$1, %ecx
	jmp	eights

f1:
	movl	%esi, %ecx
	andl	$7, %ecx
	shrq	$2, %rcx
fours:
	cmpl	$0, %ecx
	jle	f2
	call	rdrand32
	movl	%eax, 0(%rdx)
	addq	$4, %rdx
	subl	$1, %ecx
	jmp	fours

f2:
	movl	%esi, %ecx
	andl	$3, %ecx
ones:
	cmpl	$0, %ecx
	jle	f3
	call	rdrand32
	movb	%al, 0(%rdx)
	addq	$1, %rdx
	subl	$1, %ecx
	jmp	ones

f3:
	ret

/* debug register access */

.globl putdr
putdr:
	movq	56(%rdi), %rax
	movq	%rax, %dr7
	/* wet floor */
.globl putdr01236
putdr01236:
	movq	0(%rdi), %rax
	movq	%rax, %dr0
	movq	8(%rdi), %rax
	movq	%rax, %dr1
	movq	16(%rdi), %rax
	movq	%rax, %dr2
	movq	24(%rdi), %rax
	movq	%rax, %dr3
	movq	48(%rdi), %rax
	movq	%rax, %dr6
	ret

.globl getdr6
getdr6:
	movq	%dr6, %rax
	ret

.globl putdr6
putdr6:
	movq	%rdi, %dr6
	ret

.globl putdr7
putdr7:
	movq	%rdi, %dr7
	ret

/* VMX instructions */
.globl vmxon
vmxon:
	/* GCC port: no caller-provided stack slot under System V; use one of our own */
	subq	$8, %rsp
	movq	%rdi, (%rsp)
	vmxon	(%rsp)
	addq	$8, %rsp
	jmp	_vmout

.globl vmxoff
vmxoff:
	vmxoff
	jmp	_vmout

.globl vmclear
vmclear:
	/* GCC port: no caller-provided stack slot under System V; use one of our own */
	subq	$8, %rsp
	movq	%rdi, (%rsp)
	vmclear	(%rsp)
	addq	$8, %rsp
	jmp	_vmout

.globl vmlaunch
vmlaunch:
	movq	%rdi, %rbp			/* ureg (arg1); kept live like the original's BP throughout */
	/* GCC port: scratch slot to smuggle the ureg ptr across a VM-exit
	 * via HOST_RSP; vmrestore below reads it back at entry. */
	subq	$8, %rsp

	movl	$0x6C14, %edi			/* HOST_RSP */
	movq	%rsp, %rdx
	vmwrite	%rdx, %rdi
	jbe	_vmlaunchfail

	movl	$0x6C16, %edi			/* HOST_RIP */
	movq	$vmrestore, %rdx
	vmwrite	%rdx, %rdi
	jbe	_vmlaunchfail

	movq	%rbp, (%rsp)			/* plant ureg ptr at the HOST_RSP slot */
	testl	%esi, %esi			/* resume (arg2) */
	movq	0x00(%rbp), %rax
	movq	0x08(%rbp), %rbx
	movq	0x10(%rbp), %rcx
	movq	0x18(%rbp), %rdx
	movq	0x20(%rbp), %rsi
	movq	0x28(%rbp), %rdi
	movq	0x38(%rbp), %r8
	movq	0x40(%rbp), %r9
	movq	0x48(%rbp), %r10
	movq	0x50(%rbp), %r11
	movq	0x58(%rbp), %r12
	movq	0x60(%rbp), %r13
	movq	0x68(%rbp), %r14
	movq	0x70(%rbp), %r15
	movq	0x30(%rbp), %rbp
	jne	_vmresume
	vmlaunch
	jmp	_vmlaunchfail
_vmresume:
	vmresume
	/* fall through: vmlaunch/vmresume return here only on failure */
_vmlaunchfail:
	addq	$8, %rsp			/* GCC port: undo the scratch-slot alloc above */
	jmp	_vmout

.globl vmrestore
vmrestore:
	pushq	%rbp
	movq	8(%rsp), %rbp			/* ureg ptr, smuggled via HOST_RSP from vmlaunch */
	movq	%rax, 0x00(%rbp)
	movq	%rbx, 0x08(%rbp)
	movq	%rcx, 0x10(%rbp)
	movq	%rdx, 0x18(%rbp)
	movq	%rsi, 0x20(%rbp)
	movq	%rdi, 0x28(%rbp)
	popq	0x30(%rbp)
	movq	%r8, 0x38(%rbp)
	movq	%r9, 0x40(%rbp)
	movq	%r10, 0x48(%rbp)
	movq	%r11, 0x50(%rbp)
	movq	%r12, 0x58(%rbp)
	movq	%r13, 0x60(%rbp)
	movq	%r14, 0x68(%rbp)
	movq	%r15, 0x70(%rbp)

	movq	%gs:0, RMACH			/* m-> */
	movq	16(RMACH), RUSER		/* up-> */
	xorl	%eax, %eax
	ret

.globl vmptrld
vmptrld:
	/* GCC port: no caller-provided stack slot under System V; use one of our own */
	subq	$8, %rsp
	movq	%rdi, (%rsp)
	vmptrld	(%rsp)
	addq	$8, %rsp
	jmp	_vmout

.globl vmwrite
vmwrite:
	vmwrite	%rsi, %rdi			/* value (arg2), field (arg1) */
	jmp	_vmout

.globl vmread
vmread:
	vmread	%rdi, (%rsi)			/* field (arg1), *valp (arg2) */
	jmp	_vmout

.globl invept
invept:
	/* GCC port: desc (16-byte struct, arg2) assumed passed as two
	 * eightbytes in %rsi:%rdx per System V classification; verify
	 * once the real C caller (vmx.c) is ported. No caller-provided
	 * stack slot under System V either way, so use one of our own. */
	subq	$16, %rsp
	movq	%rsi, (%rsp)
	movq	%rdx, 8(%rsp)
	invept	(%rsp), %rdi			/* type (arg1) */
	addq	$16, %rsp
	jmp	_vmout

.globl invvpid
invvpid:
	/* GCC port: see invept above */
	subq	$16, %rsp
	movq	%rsi, (%rsp)
	movq	%rdx, 8(%rsp)
	invvpid	(%rsp), %rdi			/* type (arg1) */
	addq	$16, %rsp
	jmp	_vmout

_vmout:
	jc	_vmout1
	jz	_vmout2
	xorl	%eax, %eax
	ret
_vmout1:
	movq	$-1, %rax
	ret
_vmout2:
	movq	$-2, %rax
	ret

/*
 */
.globl touser
touser:
	swapgs

	movq	$0, RMACH
	movq	$0, RUSER

	movq	$(UTZERO+0x28), %rcx		/* ip */
	movl	$0x200, %r11d			/* flags */
	movq	%rdi, %rsp			/* sp (arg1) */

	sysretq

/*
 */
.globl syscallentry
syscallentry:
	swapgs
	movq	%gs:0, %rax			/* m-> */
	movq	16(%rax), %rbx			/* up-> */
	movq	%rsp, %r13
	movq	%rbx, %rsp

	pushq	$UDSEL				/* old stack segment */
	pushq	%r13				/* old sp */
	pushq	%r11				/* old flags */
	pushq	$UESEL				/* old code segment */
	pushq	%rcx				/* old ip */
	pushq	$0				/* error code */
	pushq	$64				/* trap number (VectorSYSCALL) */

	subq	$(8 + 23*8-7*8), %rsp		/* arg + sizeof(Ureg)-pushed */

	movq	RMACH, (15*8)(%rsp)		/* old r15 */
	movq	RUSER, (14*8)(%rsp)		/* old r14 */

	movq	%rbp, (7*8)(%rsp)		/* system call number: arrives in %rbp per the userspace syscall-stub/kernel private ABI, not a C argument */

	movq	%rax, RMACH			/* m */
	movq	%rbx, RUSER			/* up */

	leaq	8(%rsp), %rdi			/* Ureg* arg for CALL syscall below, via System V %rdi */

	call	syscall

.globl forkret
forkret:
	swapgs

	movq	8(%rsp), %rax			/* return value */

	movq	(15*8)(%rsp), RMACH		/* r15 */
	movq	(14*8)(%rsp), RUSER		/* r14 */

	movq	(19*8)(%rsp), %rcx		/* ip */
	movq	(21*8)(%rsp), %r11		/* flags */
	movq	(22*8)(%rsp), %rsp		/* sp */

	movq	$0, %rbx
	movq	$0, %rdx
	movq	$0, %rsi
	movq	$0, %rdi
	movq	$0, %r8
	movq	$0, %r9
	movq	$0, %r10
	movq	$0, %r12
	movq	$0, %r13
	movq	$0, %rbp

	sysretq

/*
 * Interrupt/exception handling.
 */

.globl _strayintr
_strayintr:					/* no error code pushed */
	pushq	%rax				/* save AX */
	movq	8(%rsp), %rax			/* vectortable(SB) PC */
	jmp	_intrcommon

.globl _strayintrx
_strayintrx:					/* error code pushed */
	xchgq	%rax, (%rsp)
_intrcommon:
	movzbq	(%rax), %rax
	xchgq	%rax, (%rsp)

	subq	$24, %rsp			/* R1[45], [DEFG]S */
	cmpw	$KESEL, 48(%rsp)		/* old CS */
	je	_intrnested

	movq	RUSER, 0(%rsp)
	movq	RMACH, 8(%rsp)

	swapgs
	movq	%gs:0, RMACH			/* m-> */
	movq	16(RMACH), RUSER		/* up */

_intrnested:
	pushq	%r13
	pushq	%r12
	pushq	%r11
	pushq	%r10
	pushq	%r9
	pushq	%r8
	pushq	%rbp
	pushq	%rdi
	pushq	%rsi
	pushq	%rdx
	pushq	%rcx
	pushq	%rbx
	pushq	%rax

	movq	%rsp, %rdi			/* Ureg* arg for CALL trap below, via System V %rdi */
	pushq	%rsp
	call	trap

.globl noteret
noteret:
_intrestore:
	popq	%rax

	popq	%rax
	popq	%rbx
	popq	%rcx
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	popq	%r8
	popq	%r9
	popq	%r10
	popq	%r11
	popq	%r12
	popq	%r13

	cmpq	$KESEL, 48(%rsp)
	je	_iretnested

	swapgs

	movq	8(%rsp), RMACH
	movq	0(%rsp), RUSER

_iretnested:
	addq	$40, %rsp
	iretq

.globl vectortable
vectortable:
	call _strayintr; .byte 0x00			/* divide error */
	call _strayintr; .byte 0x01			/* debug exception */
	call _strayintr; .byte 0x02			/* NMI interrupt */
	call _strayintr; .byte 0x03			/* breakpoint */
	call _strayintr; .byte 0x04			/* overflow */
	call _strayintr; .byte 0x05			/* bound */
	call _strayintr; .byte 0x06			/* invalid opcode */
	call _strayintr; .byte 0x07			/* no coprocessor available */
	call _strayintrx; .byte 0x08			/* double fault */
	call _strayintr; .byte 0x09			/* coprocessor segment overflow */
	call _strayintrx; .byte 0x0A			/* invalid TSS */
	call _strayintrx; .byte 0x0B			/* segment not available */
	call _strayintrx; .byte 0x0C			/* stack exception */
	call _strayintrx; .byte 0x0D			/* general protection error */
	call _strayintrx; .byte 0x0E			/* page fault */
	call _strayintr; .byte 0x0F
	call _strayintr; .byte 0x10			/* coprocessor error */
	call _strayintrx; .byte 0x11			/* alignment check */
	call _strayintr; .byte 0x12			/* machine check */
	call _strayintr; .byte 0x13			/* simd error */
	call _strayintr; .byte 0x14
	call _strayintr; .byte 0x15
	call _strayintr; .byte 0x16
	call _strayintr; .byte 0x17
	call _strayintr; .byte 0x18
	call _strayintr; .byte 0x19
	call _strayintr; .byte 0x1A
	call _strayintr; .byte 0x1B
	call _strayintr; .byte 0x1C
	call _strayintr; .byte 0x1D
	call _strayintr; .byte 0x1E
	call _strayintr; .byte 0x1F
	call _strayintr; .byte 0x20			/* VectorLAPIC */
	call _strayintr; .byte 0x21
	call _strayintr; .byte 0x22
	call _strayintr; .byte 0x23
	call _strayintr; .byte 0x24
	call _strayintr; .byte 0x25
	call _strayintr; .byte 0x26
	call _strayintr; .byte 0x27
	call _strayintr; .byte 0x28
	call _strayintr; .byte 0x29
	call _strayintr; .byte 0x2A
	call _strayintr; .byte 0x2B
	call _strayintr; .byte 0x2C
	call _strayintr; .byte 0x2D
	call _strayintr; .byte 0x2E
	call _strayintr; .byte 0x2F
	call _strayintr; .byte 0x30
	call _strayintr; .byte 0x31
	call _strayintr; .byte 0x32
	call _strayintr; .byte 0x33
	call _strayintr; .byte 0x34
	call _strayintr; .byte 0x35
	call _strayintr; .byte 0x36
	call _strayintr; .byte 0x37
	call _strayintr; .byte 0x38
	call _strayintr; .byte 0x39
	call _strayintr; .byte 0x3A
	call _strayintr; .byte 0x3B
	call _strayintr; .byte 0x3C
	call _strayintr; .byte 0x3D
	call _strayintr; .byte 0x3E
	call _strayintr; .byte 0x3F
	call _strayintr; .byte 0x40			/* was VectorSYSCALL */
	call _strayintr; .byte 0x41
	call _strayintr; .byte 0x42
	call _strayintr; .byte 0x43
	call _strayintr; .byte 0x44
	call _strayintr; .byte 0x45
	call _strayintr; .byte 0x46
	call _strayintr; .byte 0x47
	call _strayintr; .byte 0x48
	call _strayintr; .byte 0x49
	call _strayintr; .byte 0x4A
	call _strayintr; .byte 0x4B
	call _strayintr; .byte 0x4C
	call _strayintr; .byte 0x4D
	call _strayintr; .byte 0x4E
	call _strayintr; .byte 0x4F
	call _strayintr; .byte 0x50
	call _strayintr; .byte 0x51
	call _strayintr; .byte 0x52
	call _strayintr; .byte 0x53
	call _strayintr; .byte 0x54
	call _strayintr; .byte 0x55
	call _strayintr; .byte 0x56
	call _strayintr; .byte 0x57
	call _strayintr; .byte 0x58
	call _strayintr; .byte 0x59
	call _strayintr; .byte 0x5A
	call _strayintr; .byte 0x5B
	call _strayintr; .byte 0x5C
	call _strayintr; .byte 0x5D
	call _strayintr; .byte 0x5E
	call _strayintr; .byte 0x5F
	call _strayintr; .byte 0x60
	call _strayintr; .byte 0x61
	call _strayintr; .byte 0x62
	call _strayintr; .byte 0x63
	call _strayintr; .byte 0x64
	call _strayintr; .byte 0x65
	call _strayintr; .byte 0x66
	call _strayintr; .byte 0x67
	call _strayintr; .byte 0x68
	call _strayintr; .byte 0x69
	call _strayintr; .byte 0x6A
	call _strayintr; .byte 0x6B
	call _strayintr; .byte 0x6C
	call _strayintr; .byte 0x6D
	call _strayintr; .byte 0x6E
	call _strayintr; .byte 0x6F
	call _strayintr; .byte 0x70
	call _strayintr; .byte 0x71
	call _strayintr; .byte 0x72
	call _strayintr; .byte 0x73
	call _strayintr; .byte 0x74
	call _strayintr; .byte 0x75
	call _strayintr; .byte 0x76
	call _strayintr; .byte 0x77
	call _strayintr; .byte 0x78
	call _strayintr; .byte 0x79
	call _strayintr; .byte 0x7A
	call _strayintr; .byte 0x7B
	call _strayintr; .byte 0x7C
	call _strayintr; .byte 0x7D
	call _strayintr; .byte 0x7E
	call _strayintr; .byte 0x7F
	call _strayintr; .byte 0x80			/* Vector[A]PIC */
	call _strayintr; .byte 0x81
	call _strayintr; .byte 0x82
	call _strayintr; .byte 0x83
	call _strayintr; .byte 0x84
	call _strayintr; .byte 0x85
	call _strayintr; .byte 0x86
	call _strayintr; .byte 0x87
	call _strayintr; .byte 0x88
	call _strayintr; .byte 0x89
	call _strayintr; .byte 0x8A
	call _strayintr; .byte 0x8B
	call _strayintr; .byte 0x8C
	call _strayintr; .byte 0x8D
	call _strayintr; .byte 0x8E
	call _strayintr; .byte 0x8F
	call _strayintr; .byte 0x90
	call _strayintr; .byte 0x91
	call _strayintr; .byte 0x92
	call _strayintr; .byte 0x93
	call _strayintr; .byte 0x94
	call _strayintr; .byte 0x95
	call _strayintr; .byte 0x96
	call _strayintr; .byte 0x97
	call _strayintr; .byte 0x98
	call _strayintr; .byte 0x99
	call _strayintr; .byte 0x9A
	call _strayintr; .byte 0x9B
	call _strayintr; .byte 0x9C
	call _strayintr; .byte 0x9D
	call _strayintr; .byte 0x9E
	call _strayintr; .byte 0x9F
	call _strayintr; .byte 0xA0
	call _strayintr; .byte 0xA1
	call _strayintr; .byte 0xA2
	call _strayintr; .byte 0xA3
	call _strayintr; .byte 0xA4
	call _strayintr; .byte 0xA5
	call _strayintr; .byte 0xA6
	call _strayintr; .byte 0xA7
	call _strayintr; .byte 0xA8
	call _strayintr; .byte 0xA9
	call _strayintr; .byte 0xAA
	call _strayintr; .byte 0xAB
	call _strayintr; .byte 0xAC
	call _strayintr; .byte 0xAD
	call _strayintr; .byte 0xAE
	call _strayintr; .byte 0xAF
	call _strayintr; .byte 0xB0
	call _strayintr; .byte 0xB1
	call _strayintr; .byte 0xB2
	call _strayintr; .byte 0xB3
	call _strayintr; .byte 0xB4
	call _strayintr; .byte 0xB5
	call _strayintr; .byte 0xB6
	call _strayintr; .byte 0xB7
	call _strayintr; .byte 0xB8
	call _strayintr; .byte 0xB9
	call _strayintr; .byte 0xBA
	call _strayintr; .byte 0xBB
	call _strayintr; .byte 0xBC
	call _strayintr; .byte 0xBD
	call _strayintr; .byte 0xBE
	call _strayintr; .byte 0xBF
	call _strayintr; .byte 0xC0
	call _strayintr; .byte 0xC1
	call _strayintr; .byte 0xC2
	call _strayintr; .byte 0xC3
	call _strayintr; .byte 0xC4
	call _strayintr; .byte 0xC5
	call _strayintr; .byte 0xC6
	call _strayintr; .byte 0xC7
	call _strayintr; .byte 0xC8
	call _strayintr; .byte 0xC9
	call _strayintr; .byte 0xCA
	call _strayintr; .byte 0xCB
	call _strayintr; .byte 0xCC
	call _strayintr; .byte 0xCD
	call _strayintr; .byte 0xCE
	call _strayintr; .byte 0xCF
	call _strayintr; .byte 0xD0
	call _strayintr; .byte 0xD1
	call _strayintr; .byte 0xD2
	call _strayintr; .byte 0xD3
	call _strayintr; .byte 0xD4
	call _strayintr; .byte 0xD5
	call _strayintr; .byte 0xD6
	call _strayintr; .byte 0xD7
	call _strayintr; .byte 0xD8
	call _strayintr; .byte 0xD9
	call _strayintr; .byte 0xDA
	call _strayintr; .byte 0xDB
	call _strayintr; .byte 0xDC
	call _strayintr; .byte 0xDD
	call _strayintr; .byte 0xDE
	call _strayintr; .byte 0xDF
	call _strayintr; .byte 0xE0
	call _strayintr; .byte 0xE1
	call _strayintr; .byte 0xE2
	call _strayintr; .byte 0xE3
	call _strayintr; .byte 0xE4
	call _strayintr; .byte 0xE5
	call _strayintr; .byte 0xE6
	call _strayintr; .byte 0xE7
	call _strayintr; .byte 0xE8
	call _strayintr; .byte 0xE9
	call _strayintr; .byte 0xEA
	call _strayintr; .byte 0xEB
	call _strayintr; .byte 0xEC
	call _strayintr; .byte 0xED
	call _strayintr; .byte 0xEE
	call _strayintr; .byte 0xEF
	call _strayintr; .byte 0xF0
	call _strayintr; .byte 0xF1
	call _strayintr; .byte 0xF2
	call _strayintr; .byte 0xF3
	call _strayintr; .byte 0xF4
	call _strayintr; .byte 0xF5
	call _strayintr; .byte 0xF6
	call _strayintr; .byte 0xF7
	call _strayintr; .byte 0xF8
	call _strayintr; .byte 0xF9
	call _strayintr; .byte 0xFA
	call _strayintr; .byte 0xFB
	call _strayintr; .byte 0xFC
	call _strayintr; .byte 0xFD
	call _strayintr; .byte 0xFE
	call _strayintr; .byte 0xFF
