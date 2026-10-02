#include "mem.h"

/*
 * GCC port register/ABI notes (see sys/src/9/pc64/l.S for the amd64
 * equivalent of this file, and AGENTS.md / MEMORY.md for the broader
 * gcc-port conventions):
 *
 * - This is a 32-bit (cdecl) port, not amd64 -- there is no register-based
 *   argument-passing ABI to translate around like pc64/l.S needed. Every
 *   ordinary C-callable function below keeps Plan9's own convention
 *   (arguments on the stack, name+N(FP) -> N+4(%esp) measured from a
 *   fresh function entry, adjusted for any PUSHes already done). AX/BX/
 *   CX/DX/SI/DI/BP/SP map literally to %eax/%ebx/%ecx/%edx/%esi/%edi/
 *   %ebp/%esp.
 *
 * - Plan9's 386 kernel keeps its Mach* in a plain global C variable `m`
 *   (sys/src/9/pc/main.c: "Mach *m;"), not a pinned register the way
 *   pc64 pins %r15 -- no GCC-port register-pinning concerns here at all.
 *
 * - %ebp is used as scratch in a few hand-written leaf sequences below
 *   (rdmsr/wrmsr) exactly as front's original does, but gcc-compiled C
 *   code expects %ebp preserved across any call (callee-saved under both
 *   cdecl and Plan9 convention) -- each such use saves/restores it
 *   explicitly around the scratch use, same fix as pc64/l.S's rdmsr/
 *   wrmsr (trap.c's _rdmsrinst/_wrmsrinst fault recovery writes -1 into
 *   the pushed Ureg.bp slot, which lands back in %ebp on return).
 *
 * - We build with -fno-omit-frame-pointer, so gcc-compiled functions
 *   address their locals %ebp-relative; gotolabel()'s jump bypasses
 *   every intervening function's normal epilogue (which would otherwise
 *   have restored %ebp), so setlabel/gotolabel save/restore %ebp too via
 *   a third Label field -- same fix as pc64/dat.h's struct Label gained
 *   (uintptr bp), mirrored in sys/src/9/pc/dat.h.
 */

#define DELAY		.byte 0xEB; .byte 0x00	/* JMP .+2 */

#define VectorSYSCALL	0x40

#define PADDR(a)	((a) & ~KZERO)
#define KADDR(a)	(KZERO|(a))

#define PDO(a)		(((((a))>>22) & 0x03FF)<<2)
#define PTO(a)		(((((a))>>12) & 0x03FF)<<2)

/*
 * For backwards compatibility with 9load -- see front's l.s; 9load
 * identity-maps the first 16MB, so behave as if the mmu was not set up.
 */
.globl _startKADDR
_startKADDR:
	movl	$_startPADDR, %eax
	andl	$~KZERO, %eax
	jmp	*%eax

/*
 * Must be 4-byte aligned. GCC port: GAS doesn't auto-align TEXT symbols
 * the way 8a did, so this needs an explicit directive.
 */
.balign 4
.globl _multibootheader
_multibootheader:
	.long	0x1BADB002				/* magic */
	.long	0x00010007				/* flags */
	.long	-(0x1BADB002 + 0x00010007)		/* checksum */
	.long	_multibootheader - KZERO		/* header_addr */
	.long	_startKADDR - KZERO			/* load_addr */
	.long	edata - KZERO				/* load_end_addr */
	.long	end - KZERO				/* bss_end_addr */
	.long	_multibootentry - KZERO		/* entry_addr */
	.long	0					/* mode_type */
	.long	0					/* width */
	.long	0					/* height */
	.long	32					/* depth */

/*
 * The kernel expects the data segment to be page-aligned. Front's
 * original code here shifted the tightly-packed .text/.data block (8l/
 * 6l pack .data immediately after .text with no gap) up to a page
 * boundary at boot. GCC port: kernel.ld page-aligns .data directly
 * instead (see that file), so there's nothing left to relocate -- just
 * stash the Multiboot info pointer and fall into _startPADDR. Same fix
 * as pc64/l.S's _multibootentry.
 */
.globl _multibootentry
_multibootentry:
	movl	%ebx, (multibootptr - KZERO)
	movl	$(_startPADDR - KZERO), %eax
	jmp	*%eax

/* multiboot structure pointer (physical address) */
.globl multibootptr
multibootptr:
	.long	0

/*
 * In protected mode with paging turned off and segment registers setup
 * to linear map all memory. Entered via a jump to PADDR(entry), the
 * physical address of the virtual kernel entry point of KADDR(entry).
 * Make the basic page tables for processor 0 -- see front's l.s for the
 * full page count/layout rationale, unchanged here.
 */
.globl _startPADDR
_startPADDR:
	cli						/* make sure interrupts are off */

	/* set up the gdt so we have sane plan 9 style gdts */
	movl	$(tgdtptr - KZERO), %eax
	lgdt	(%eax)
	movw	$1, %ax
	lmsw	%ax

	DELAY						/* clear prefetch queue */

	/* set segs to something sane (avoid traps later) */
	movw	$(1<<3), %ax
	movw	%ax, %ds
	movw	%ax, %ss
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs

	/* JMP $(2<<3):$mode32bit -- far jump to reload %cs */
	.byte	0xEA
	.long	mode32bit - KZERO
	.word	(2<<3)

/*
 * gdt to get us to 32-bit/segmented/unpaged mode
 */
.globl tgdt
tgdt:
	/* null descriptor */
	.long	0
	.long	0

	/* data segment descriptor for 4 gigabytes (PL 0) */
	.long	0xFFFF
	.long	(SEGG|SEGB|(0xF<<16)|SEGP|SEGPL(0)|SEGDATA|SEGW)

	/* exec segment descriptor for 4 gigabytes (PL 0) */
	.long	0xFFFF
	.long	(SEGG|SEGD|(0xF<<16)|SEGP|SEGPL(0)|SEGEXEC|SEGR)

/*
 * Pointer to initial gdt. Note the -KZERO which puts the physical
 * address in the gdtptr -- needed as we start executing in physical
 * addresses.
 */
.globl tgdtptr
tgdtptr:
	.word	(3*8)
	.long	tgdt - KZERO

.globl vtgdtptr
vtgdtptr:
	.word	(3*8)
	.long	tgdt

.globl mode32bit
mode32bit:
	movl	$((CPU0END-CPU0PDB)>>2), %ecx
	movl	$PADDR(CPU0PDB), %edi
	xorl	%eax, %eax

	cld
	rep; stosl

	movl	$PADDR(CPU0PTE), %edx
	movl	$(PTEWRITE|PTEVALID), %ebx	/* page permissions */
	orl	%ebx, %edx

	movl	$PADDR(CPU0PDB), %eax
	addl	$PDO(KZERO), %eax		/* page directory offset for KZERO */

	movl	%edx, 0(%eax)			/* PTE's for KZERO */
	addl	$BY2PG, %edx
	movl	%edx, 4(%eax)			/* PTE's for KZERO+4MB */
	addl	$BY2PG, %edx
	movl	%edx, 8(%eax)			/* PTE's for KZERO+8MB */
	addl	$BY2PG, %edx
	movl	%edx, 12(%eax)			/* PTE's for KZERO+12MB */

	movl	$PADDR(CPU0PTE), %eax		/* first page of page table */
	movl	$(end - KZERO), %ecx

	addl	$(16*1024), %ecx		/* qemu puts multiboot data after the kernel */

	addl	$(BY2XPG-1), %ecx
	andl	$~(BY2XPG-1), %ecx		/* round to 4MB */
	movl	%ecx, (MemMin - KZERO)		/* see memory.c */
	shrl	$PGSHIFT, %ecx
	movl	%ebx, %edx
_setpte:
	movl	%edx, (%eax)
	addl	$BY2PG, %edx
	addl	$4, %eax
	loop	_setpte

	movl	$PADDR(CPU0PTE), %eax
	addl	$PTO(MACHADDR), %eax		/* page table entry offset for MACHADDR */
	orl	$PADDR(CPU0MACH), %ebx
	movl	%ebx, (%eax)			/* PTE for Mach */

/*
 * Now ready to use the new map. Make sure the processor options are what
 * is wanted. A JMP immediately after a mode switch clears the prefetch
 * queue on some processors.
 */
	movl	$PADDR(CPU0PDB), %ecx		/* load address of page directory */
	movl	PDO(KZERO)(%ecx), %edx		/* double-map KZERO at 0 */
	movl	%edx, PDO(0)(%ecx)
	movl	%ecx, %cr3
	DELAY

	movl	%cr0, %edx
	orl	$0x80010000, %edx		/* PG|WP */
	andl	$~0x6000000A, %edx		/* ~(CD|NW|TS|MP) */

	movl	$_startpg, %eax		/* this is a virtual address */
	movl	%edx, %cr0			/* turn on paging */
	jmp	*%eax				/* jump to the virtual nirvana */

/*
 * Basic machine environment set, can clear BSS and create a stack. The
 * stack starts at the top of the page containing the Mach structure.
 */
.globl _startpg
_startpg:
	movl	$vtgdtptr, %eax
	lgdt	(%eax)

	movl	$0, PDO(0)(%ecx)		/* undo double-map of KZERO at 0 */
	movl	%ecx, %cr3			/* load and flush the mmu */

_clearbss:
	movl	$edata, %edi
	xorl	%eax, %eax
	movl	$end, %ecx
	subl	%edi, %ecx			/* end-edata bytes */
	shrl	$2, %ecx			/* end-edata doublewords */

	cld
	rep; stosl				/* clear BSS */

	/*
	 * GCC port: MemMin lives in BSS (between edata and end), so the
	 * clear above just wiped out the value mode32bit computed into it
	 * earlier (while still in physical-addressing mode, before
	 * paging). Front's original source has this same ordering (BSS
	 * cleared after MemMin is set) and presumably gets away with it
	 * only because 8l happens to place MemMin outside [edata,end)
	 * under its own layout; GNU ld's doesn't, so it's provably zero
	 * here by the time meminit0() reads it (confirmed: panics "kernel
	 * too big" since MemMin(0) < PADDR(PGROUND(end))). Same fix as
	 * pc64/l.S: recompute the exact same value mode32bit used, now in
	 * virtual-addressing form since paging is already active here.
	 */
	movl	$(end-KZERO), %edx
	addl	$(16*1024), %edx		/* qemu puts multiboot data after the kernel */
	addl	$(BY2XPG-1), %edx
	andl	$~(BY2XPG-1), %edx
	movl	%edx, MemMin

	movl	$MACHADDR, %esp
	movl	%esp, m				/* initialise global Mach pointer */
	movl	$0, 0(%esp)			/* initialise m->machno */

	addl	$(MACHSIZE-4), %esp		/* initialise stack */

/*
 * Need to do one final thing to ensure a clean machine environment,
 * clear the EFLAGS register, which can only be done once there is a stack.
 */
	movl	$0, %eax
	pushl	%eax
	popfl

	call	main

/*
 * Park a processor. Should never fall through a return from main to
 * here -- should only be called by application processors when
 * shutting down.
 */
.globl idle
idle:
_idle:
	sti
	hlt
	jmp	_idle

.globl load_fs
load_fs:
	movw	4(%esp), %ax
	movw	%ax, %fs
	ret

.globl load_gs
load_gs:
	movw	4(%esp), %ax
	movw	%ax, %gs
	ret

/*
 * BIOS32.
 */
.globl bios32call
bios32call:
	/*
	 * GCC port: %ebp/%ebx/%esi/%edi are all callee-saved under cdecl;
	 * this function legitimately uses all of them as scratch to
	 * marshal to/from the "ci" register-block struct for the actual
	 * BIOS32 far call (which itself both reads and clobbers them per
	 * the BIOS32 calling convention) -- but that's no reason to
	 * destroy whatever the CALLER had live in them. Save/restore all
	 * four, shift every %esp-relative offset below (both the arg
	 * reads and this function's own, already-present internal
	 * pushl %ebp) by the 16 bytes this adds.
	 */
	pushl	%ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	movl	20(%esp), %ebp			/* ci (arg1) */
	movl	0(%ebp), %eax
	movl	4(%ebp), %ebx
	movl	8(%ebp), %ecx
	movl	12(%ebp), %edx
	movl	16(%ebp), %esi
	movl	20(%ebp), %edi
	pushl	%ebp

	movl	28(%esp), %ebp			/* ptr */
	.byte	0xFF; .byte	0x5D; .byte 0x00	/* CALL FAR 0(BP) */

	popl	%ebp
	movl	%edi, 20(%ebp)
	movl	%esi, 16(%ebp)
	movl	%edx, 12(%ebp)
	movl	%ecx, 8(%ebp)
	movl	%ebx, 4(%ebp)
	movl	%eax, 0(%ebp)

	xorl	%eax, %eax
	jnc	_bios32xxret
	incl	%eax

_bios32xxret:
	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret

/*
 * Port I/O.
 *	in[bsl]		input a byte|short|long
 *	ins[bsl]	input a string of bytes|shorts|longs
 *	out[bsl]	output a byte|short|long
 *	outs[bsl]	output a string of bytes|shorts|longs
 */
.globl inb
inb:
	movl	4(%esp), %edx			/* port */
	xorl	%eax, %eax
	inb	(%dx), %al
	ret

.globl insb
insb:
	/* GCC port: %edi is callee-saved under cdecl; save/restore it */
	pushl	%edi
	movl	8(%esp), %edx			/* port */
	movl	12(%esp), %edi			/* address */
	movl	16(%esp), %ecx			/* count */
	cld
	rep; insb
	popl	%edi
	ret

.globl ins
ins:
	movl	4(%esp), %edx			/* port */
	xorl	%eax, %eax
	inw	(%dx), %ax
	ret

.globl inss
inss:
	/* GCC port: %edi is callee-saved under cdecl; save/restore it */
	pushl	%edi
	movl	8(%esp), %edx			/* port */
	movl	12(%esp), %edi			/* address */
	movl	16(%esp), %ecx			/* count */
	cld
	rep; insw
	popl	%edi
	ret

.globl inl
inl:
	movl	4(%esp), %edx			/* port */
	inl	(%dx), %eax
	ret

.globl insl
insl:
	/* GCC port: %edi is callee-saved under cdecl; save/restore it */
	pushl	%edi
	movl	8(%esp), %edx			/* port */
	movl	12(%esp), %edi			/* address */
	movl	16(%esp), %ecx			/* count */
	cld
	rep; insl
	popl	%edi
	ret

.globl outb
outb:
	movl	4(%esp), %edx			/* port */
	movl	8(%esp), %eax			/* byte */
	outb	%al, (%dx)
	ret

.globl outsb
outsb:
	/* GCC port: %esi is callee-saved under cdecl; save/restore it */
	pushl	%esi
	movl	8(%esp), %edx			/* port */
	movl	12(%esp), %esi			/* address */
	movl	16(%esp), %ecx			/* count */
	cld
	rep; outsb
	popl	%esi
	ret

.globl outs
outs:
	movl	4(%esp), %edx			/* port */
	movl	8(%esp), %eax			/* short */
	outw	%ax, (%dx)
	ret

.globl outss
outss:
	/* GCC port: %esi is callee-saved under cdecl; save/restore it */
	pushl	%esi
	movl	8(%esp), %edx			/* port */
	movl	12(%esp), %esi			/* address */
	movl	16(%esp), %ecx			/* count */
	cld
	rep; outsw
	popl	%esi
	ret

.globl outl
outl:
	movl	4(%esp), %edx			/* port */
	movl	8(%esp), %eax			/* long */
	outl	%eax, (%dx)
	ret

.globl outsl
outsl:
	/* GCC port: %esi is callee-saved under cdecl; save/restore it */
	pushl	%esi
	movl	8(%esp), %edx			/* port */
	movl	12(%esp), %esi			/* address */
	movl	16(%esp), %ecx			/* count */
	cld
	rep; outsl
	popl	%esi
	ret

/*
 * Read/write various system registers.
 * CR4 and the 'model specific registers' should only be read/written
 * after it has been determined the processor supports them.
 */
.globl lgdt
lgdt:
	movl	4(%esp), %eax			/* gdtptr */
	lgdt	(%eax)
	ret

.globl lldt
lldt:
	movl	4(%esp), %eax			/* sel */
	lldt	%ax
	ret

.globl lidt
lidt:
	movl	4(%esp), %eax			/* idtptr */
	lidt	(%eax)
	ret

.globl ltr
ltr:
	movl	4(%esp), %eax			/* tptr */
	ltr	%ax
	ret

.globl getcr0
getcr0:
	movl	%cr0, %eax
	ret

.globl getcr2
getcr2:
	movl	%cr2, %eax
	ret

.globl putcr2
putcr2:
	movl	4(%esp), %eax			/* cr2 */
	movl	%eax, %cr2
	ret

.globl getcr3
getcr3:
	movl	%cr3, %eax
	ret

.globl putcr0
putcr0:
	movl	4(%esp), %eax			/* cr0 */
	movl	%eax, %cr0
	ret

.globl putcr3
putcr3:
	movl	4(%esp), %eax			/* cr3 */
	movl	%eax, %cr3
	ret

.globl getcr4
getcr4:
	movl	%cr4, %eax
	ret

.globl putcr4
putcr4:
	movl	4(%esp), %eax			/* cr4 */
	movl	%eax, %cr4
	ret

.globl invlpg
invlpg:
	/* 486+ only */
	movl	4(%esp), %ecx			/* va */
	invlpg	(%ecx)
	ret

.globl wbinvd
wbinvd:
	wbinvd
	ret

.globl _cycles
_cycles:					/* time stamp counter */
	rdtsc
	movl	4(%esp), %ecx			/* &vlong */
	movl	%eax, 0(%ecx)			/* lo */
	movl	%edx, 4(%ecx)			/* hi */
	ret

/*
 * stub for:
 * time stamp counter; low-order 32 bits of 64-bit cycle counter
 * Runs at fasthz/4 cycles per second (m->clkin>>3)
 */
.globl lcycles
lcycles:
	rdtsc
	ret

.globl rdmsr
rdmsr:						/* model-specific register */
	movl	4(%esp), %ecx			/* index */
	pushl	%ebp				/* GCC port: see header comment -- save
						 * the caller's %ebp around the scratch
						 * use below */
	movl	$0, %ebp
.globl _rdmsrinst
_rdmsrinst:
	rdmsr
	movl	12(%esp), %ecx			/* &vlong (note: +8 for the pushl above) */
	movl	%eax, 0(%ecx)			/* lo */
	movl	%edx, 4(%ecx)			/* hi */
	movl	%ebp, %eax			/* %ebp set to -1 if trapped */
	popl	%ebp
	ret

.globl wrmsr
wrmsr:
	movl	4(%esp), %ecx			/* index */
	movl	8(%esp), %eax			/* lo */
	movl	12(%esp), %edx			/* hi */
	pushl	%ebp				/* GCC port: see rdmsr above */
	movl	$0, %ebp
.globl _wrmsrinst
_wrmsrinst:
	wrmsr
	movl	%ebp, %eax			/* %ebp set to -1 if trapped */
	popl	%ebp
	ret

/* fault-proof memcpy */
.globl peek
peek:
	/*
	 * GCC port: %esi/%edi are callee-saved under cdecl; save/restore
	 * them. Safe for the _peekinst fault-recovery path too (trap.c
	 * just does ureg->pc += 2 past the 2-byte "rep movsb" and falls
	 * through to here same as a normal return, no mid-function state
	 * needed from esi/edi beyond that point).
	 */
	pushl	%esi
	pushl	%edi
	movl	12(%esp), %esi			/* src */
	movl	16(%esp), %edi			/* dst */
	movl	20(%esp), %ecx			/* cnt */
	cld
.globl _peekinst
_peekinst:
	rep; movsb
	movl	%ecx, %eax
	popl	%edi
	popl	%esi
	ret

/*
 * Try to determine the CPU type which requires fiddling with EFLAGS.
 * If the Id bit can be toggled then the CPUID instruction can be used
 * to determine CPU identity and features. First have to check if it's
 * a 386 (Ac bit can't be set). If it's not a 386 and the Id bit can't be
 * toggled then it's an older 486 of some kind.
 *
 *	cpuid(fn, sublvl, regs[4]);
 */
.globl cpuid
cpuid:
	/*
	 * GCC port: %ebp AND %ebx are both callee-saved under cdecl (and
	 * we build with -fno-omit-frame-pointer, so the caller is
	 * actively relying on %ebp as its own frame pointer) -- front's
	 * original uses both as free scratch (%ebp to hold "regs" at the
	 * end, %ebx throughout for the Id/Ac EFLAGS-toggle probe and to
	 * carry CPUID's own %ebx output), same bug class already fixed in
	 * rdmsr/wrmsr above. Save/restore both here, and shift every
	 * %esp-relative argument offset by the 8 bytes the two pushes add.
	 */
	pushl	%ebp
	pushl	%ebx
	movl	$0x240000, %eax
	pushl	%eax
	popfl					/* set Id|Ac */
	pushfl
	popl	%ebx				/* retrieve value */
	movl	$0, %eax
	pushl	%eax
	popfl					/* clear Id|Ac, EFLAGS initialised */
	pushfl
	popl	%eax				/* retrieve value */
	xorl	%ebx, %eax
	testl	$0x040000, %eax			/* Ac */
	jz	_cpu386				/* can't set this bit on 386 */
	testl	$0x200000, %eax			/* Id */
	jz	_cpu486				/* can't toggle this bit on some 486 */
	movl	12(%esp), %eax			/* fn */
	movl	16(%esp), %ecx			/* sublvl */
	cpuid
	jmp	_cpuid
_cpu486:
	movl	$0x400, %eax
	jmp	_maybezapax
_cpu386:
	movl	$0x300, %eax
_maybezapax:
	cmpl	$1, 12(%esp)			/* fn */
	je	_zaprest
	xorl	%eax, %eax
_zaprest:
	xorl	%ebx, %ebx
	xorl	%ecx, %ecx
	xorl	%edx, %edx
_cpuid:
	movl	20(%esp), %ebp			/* regs */
	movl	%eax, 0(%ebp)
	movl	%ebx, 4(%ebp)
	movl	%ecx, 8(%ebp)
	movl	%edx, 12(%ebp)
	popl	%ebx
	popl	%ebp
	ret

/*
 * Basic timing loop to determine CPU frequency.
 */
.globl delayloop
delayloop:
	movl	4(%esp), %ecx			/* count */
_aamloop:
	aam
	loop	_aamloop
	ret

/*
 * Floating point.
 * Note: the encodings for the FCLEX, FINIT, FSAVE, FSTCW, FSENV and
 * FSTSW instructions do NOT have the WAIT prefix byte (i.e. they act
 * like their FNxxx variations) so WAIT instructions must be explicitly
 * placed in the code as necessary.
 */
.globl fpoff
fpoff:						/* disable */
	movl	%cr0, %eax
	orl	$0x28, %eax			/* NE=1, TS=1 */
	movl	%eax, %cr0
	ret

.globl fpinit
fpinit:						/* enable and init */
	movl	%cr0, %eax
	andl	$~0xC, %eax			/* EM=0, TS=0 */
	movl	%eax, %cr0
	fninit
	fwait
	/* setfcr(FPPDBL|FPRNR|FPINVAL|FPZDIV|FPOVFL) */
	/* note that low 6 bits are masks, not enables, on this chip */
	pushw	$0x0232
	fldcw	0(%esp)
	popw	%ax
	fwait
	ret

.globl fpx87save0
fpx87save0:					/* save state and disable */
	movl	4(%esp), %eax			/* p */
	fnsave	0(%eax)				/* no WAIT */
	movl	%cr0, %eax
	orl	$0x28, %eax
	movl	%eax, %cr0
	ret

.globl fpx87restore0
fpx87restore0:					/* enable and restore state */
	movl	%cr0, %eax
	andl	$~0xC, %eax
	movl	%eax, %cr0
	movl	4(%esp), %eax			/* p */
	frstor	0(%eax)
	fwait
	ret

.globl fpclear
fpclear:					/* clear pending exceptions */
	movl	%cr0, %eax
	andl	$~0xC, %eax
	movl	%eax, %cr0
	fnclex					/* no WAIT */
	orl	$0x28, %eax
	movl	%eax, %cr0
	ret

.globl fpssesave
fpssesave:					/* save state and disable */
	movl	4(%esp), %eax			/* p */
	fxsave	0(%eax)				/* no WAIT */
	movl	%cr0, %eax
	orl	$0x28, %eax
	movl	%eax, %cr0
	ret

.globl fpsserestore
fpsserestore:					/* enable and restore state */
	movl	%cr0, %eax
	andl	$~0xC, %eax
	movl	%eax, %cr0
	movl	4(%esp), %eax			/* p */
	fxrstor	0(%eax)
	fwait
	ret

.globl ldmxcsr
ldmxcsr:					/* load MXCSR */
	ldmxcsr	4(%esp)				/* mxcsr */
	ret

.globl splhi
splhi:
shi:
	pushfl
	popl	%eax
	testl	$0x200, %eax
	jz	alreadyhi
	movl	$(MACHADDR+0x04), %ecx		/* save PC in m->splpc */
	/*
	 * GCC port: %ebx is callee-saved under cdecl -- this is one of
	 * the hottest paths in the kernel (every lock acquisition), so an
	 * unprotected clobber here would be both severe and very hard to
	 * localize. %eax's value (the eflags test result) is dead by this
	 * point, so reuse it instead of introducing a push/pop.
	 */
	movl	(%esp), %eax
	movl	%eax, (%ecx)
	cli
alreadyhi:
	ret

.globl spllo
spllo:
slo:
	pushfl
	popl	%eax
	testl	$0x200, %eax
	jnz	alreadylo
	movl	$(MACHADDR+0x04), %ecx		/* clear m->splpc */
	movl	$0, (%ecx)
	sti
alreadylo:
	ret

.globl splx
splx:
	movl	4(%esp), %eax			/* s */
	testl	$0x200, %eax
	jnz	slo
	jmp	shi

.globl spldone
spldone:
	ret

.globl islo
islo:
	pushfl
	popl	%eax
	andl	$0x200, %eax			/* interrupt enable flag */
	ret

/*
 * Test-And-Set
 */
.globl tas
tas:
.globl _tas
_tas:
	/*
	 * GCC port: %ebx is callee-saved under cdecl -- another extremely
	 * hot path (every lock attempt). Use %ecx instead (caller-saved,
	 * otherwise unused here) rather than pay for a push/pop.
	 */
	movl	$0xDEADDEAD, %eax
	movl	4(%esp), %ecx			/* lock */
	xchgl	%eax, (%ecx)			/* lock->key */
	ret

.globl mb386
mb386:
	popl	%eax				/* return PC */
	pushfl
	pushl	%cs
	pushl	%eax
	iretl

.globl mb586
mb586:
	xorl	%eax, %eax
	cpuid
	ret

.globl sfence
sfence:
	sfence
	ret

.globl lfence
lfence:
	lfence
	ret

.globl mfence
mfence:
	mfence
	ret

.globl xchgw
xchgw:
	/* GCC port: %ebx is callee-saved under cdecl; save/restore it */
	pushl	%ebx
	movl	12(%esp), %eax			/* v */
	movl	8(%esp), %ebx			/* p */
	xchgw	%ax, (%ebx)
	popl	%ebx
	ret

.globl cmpswap486
cmpswap486:
	/* GCC port: %ebx is callee-saved under cdecl; save/restore it */
	pushl	%ebx
	movl	8(%esp), %ebx			/* addr */
	movl	12(%esp), %eax			/* old */
	movl	16(%esp), %ecx			/* new */
	lock; cmpxchgl %ecx, (%ebx)
	jnz	didnt
	movl	$1, %eax
	popl	%ebx
	ret
didnt:
	xorl	%eax, %eax
	popl	%ebx
	ret

/*
 * Multiply two 64-bit numbers and keep the middle 64 bits from the
 * 128-bit result. See ../port/tod.c for motivation.
 */
.globl mul64fract
mul64fract:
	/* GCC port: %ebx is callee-saved under cdecl; save/restore it */
	pushl	%ebx
	movl	8(%esp), %ecx			/* r */
	xorl	%ebx, %ebx			/* ebx = 0 */

	movl	16(%esp), %eax			/* a1 */
	mull	24(%esp)			/* a1*b1 */
	movl	%eax, 4(%ecx)			/* r2 = lo(a1*b1) */

	movl	16(%esp), %eax			/* a1 */
	mull	20(%esp)			/* a1*b0 */
	movl	%eax, 0(%ecx)			/* r1 = lo(a1*b0) */
	addl	%edx, 4(%ecx)			/* r2 += hi(a1*b0) */

	movl	12(%esp), %eax			/* a0 */
	mull	24(%esp)			/* a0*b1 */
	addl	%eax, 0(%ecx)			/* r1 += lo(a0*b1) */
	adcl	%edx, 4(%ecx)			/* r2 += hi(a0*b1) + carry */

	movl	12(%esp), %eax			/* a0 */
	mull	20(%esp)			/* a0*b0 */
	addl	%edx, 0(%ecx)			/* r1 += hi(a0*b0) */
	adcl	%ebx, 4(%ecx)			/* r2 += carry */
	popl	%ebx
	ret

/*
 * Label consists of a stack pointer and a programme counter -- plus, in
 * this gcc port, a frame pointer (see header comment: -fno-omit-frame-
 * pointer means gotolabel's jump must restore %ebp too, mirrored in
 * sys/src/9/pc/dat.h's struct Label), and the rest of the cdecl
 * callee-saved set (%ebx/%esi/%edi), which gcc may keep live across a
 * procswitch() in the frames above it.
 */
.globl gotolabel
gotolabel:
	movl	4(%esp), %eax			/* label */
	movl	8(%eax), %ebp			/* restore BP */
	movl	12(%eax), %ebx			/* restore BX */
	movl	16(%eax), %esi			/* restore SI */
	movl	20(%eax), %edi			/* restore DI */
	movl	0(%eax), %esp			/* restore SP */
	movl	4(%eax), %eax			/* put return pc on the stack */
	movl	%eax, 0(%esp)
	movl	$1, %eax			/* return 1 */
	ret

.globl setlabel
setlabel:
	/*
	 * GCC port: %ebx is callee-saved under cdecl -- setlabel() backs
	 * waserror(), called throughout the kernel. Use %ecx instead
	 * (caller-saved, otherwise unused here) rather than pay for a
	 * push/pop.
	 */
	movl	4(%esp), %eax			/* label */
	movl	%esp, 0(%eax)			/* store sp */
	movl	0(%esp), %ecx			/* store return pc */
	movl	%ecx, 4(%eax)
	movl	%ebp, 8(%eax)			/* store bp */
	movl	%ebx, 12(%eax)			/* store bx */
	movl	%esi, 16(%eax)			/* store si */
	movl	%edi, 20(%eax)			/* store di */
	movl	$0, %eax			/* return 0 */
	ret

/*
 * Attempt at power saving. -rsc
 */
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
	movl	4(%esp), %eax			/* addr */
	movl	(%eax), %ecx
	orl	%ecx, %ecx
	jnz	_mwaitdone
	xorl	%edx, %edx
	.byte	0x0f; .byte 0x01; .byte 0xc8	/* MONITOR */
	movl	(%eax), %ecx
	orl	%ecx, %ecx
	jnz	_mwaitdone
	xorl	%eax, %eax
	.byte	0x0f; .byte 0x01; .byte 0xc9	/* MWAIT */
_mwaitdone:
	ret

.globl rdrand32
rdrand32:
_rloop32:
	.byte	0x0f; .byte 0xc7; .byte 0xf0	/* RDRAND AX */
	jnc	_rloop32
	ret

.globl rdrandbuf
rdrandbuf:
	/* GCC port: %edi is callee-saved under cdecl; save/restore it */
	pushl	%edi
	movl	8(%esp), %edi			/* buf */
	movl	12(%esp), %ecx			/* cnt */
	cld
	movl	%ecx, %edx
	shrl	$2, %ecx
	cmpl	$0, %ecx
	je	_rndleft
_rnddwords:
	call	rdrand32
	stosl
	loop	_rnddwords
_rndleft:
	movl	%edx, %ecx
	andl	$3, %ecx
	cmpl	$0, %ecx
	je	_rnddone
_rndbytes:
	call	rdrand32
	stosb
	loop	_rndbytes
_rnddone:
	popl	%edi
	ret

/* debug register access */

.globl putdr
putdr:
	/* GCC port: %esi is callee-saved under cdecl; save/restore it */
	pushl	%esi
	movl	8(%esp), %esi			/* p */
	movl	28(%esi), %eax
	movl	%eax, %dr7
_putdr01236:
	movl	0(%esi), %eax
	movl	%eax, %dr0
	movl	4(%esi), %eax
	movl	%eax, %dr1
	movl	8(%esi), %eax
	movl	%eax, %dr2
	movl	12(%esi), %eax
	movl	%eax, %dr3
	movl	24(%esi), %eax
	movl	%eax, %dr6
	popl	%esi
	ret

.globl putdr01236
putdr01236:
	pushl	%esi
	movl	8(%esp), %esi			/* p */
	jmp	_putdr01236

.globl getdr6
getdr6:
	movl	%dr6, %eax
	ret

.globl putdr6
putdr6:
	movl	4(%esp), %eax			/* p */
	movl	%eax, %dr6
	ret

.globl putdr7
putdr7:
	movl	4(%esp), %eax			/* p */
	movl	%eax, %dr7
	ret

/* VMX instructions */
.globl vmxon
vmxon:
	/* VMXON 4(SP) */
	.byte	0xf3; .byte 0x0f; .byte 0xc7; .byte 0x74; .byte 0x24; .byte 0x04
	jmp	_vmout

.globl vmxoff
vmxoff:
	.byte	0x0f; .byte 0x01; .byte 0xc4
	jmp	_vmout

.globl vmclear
vmclear:
	/* VMCLEAR 4(SP) */
	.byte	0x66; .byte 0x0f; .byte 0xc7; .byte 0x74; .byte 0x24; .byte 0x04
	jmp	_vmout

.globl vmlaunch
vmlaunch:
	movl	$0x6C14, %edi
	movl	%esp, %edx
	.byte	0x0f; .byte 0x79; .byte 0xfa	/* VMWRITE DX, DI */
	jbe	_vmout
	movl	$0x6C16, %edi
	movl	$vmrestore, %edx
	.byte	0x0f; .byte 0x79; .byte 0xfa	/* VMWRITE DX, DI */
	jbe	_vmout

	movl	8(%esp), %eax			/* resume */
	testl	%eax, %eax
	movl	4(%esp), %edi			/* ureg */
	movl	4(%edi), %esi
	movl	8(%edi), %ebp
	movl	16(%edi), %ebx
	movl	20(%edi), %edx
	movl	24(%edi), %ecx
	movl	28(%edi), %eax
	movl	0(%edi), %edi
	jne	_vmresume
	.byte	0x0f; .byte 0x01; .byte 0xc2	/* VMLAUNCH */
	jmp	_vmout
_vmresume:
	.byte	0x0f; .byte 0x01; .byte 0xc3	/* VMRESUME */
	jmp	_vmout

.globl vmrestore
vmrestore:
	pushl	%edi
	movl	4(%esp), %edi			/* ureg (note: +4 for the pushl above, matches front) */
	popl	0(%edi)
	movl	%esi, 4(%edi)
	movl	%ebp, 8(%edi)
	movl	%ebx, 16(%edi)
	movl	%edx, 20(%edi)
	movl	%ecx, 24(%edi)
	movl	%eax, 28(%edi)
	xorl	%eax, %eax
	ret

.globl vmptrld
vmptrld:
	/* VMPTRLD 4(SP) */
	.byte	0x0f; .byte 0xc7; .byte 0x74; .byte 0x24; .byte 0x04
	jmp	_vmout

.globl vmwrite
vmwrite:
	/*
	 * GCC port: %edi is callee-saved under cdecl, but the raw VMWRITE
	 * encoding below has the register numbers baked into its ModRM
	 * byte (0xfa = edi/edx), so %edi can't just be swapped for a
	 * caller-saved register -- save/restore it instead, with a local
	 * exit sequence (the shared _vmout tail below is also reached
	 * directly, with nothing pushed, by vmxon/vmxoff/vmclear/vmptrld/
	 * invept/invvpid, so it can't be the one popping this).
	 */
	pushl	%edi
	movl	8(%esp), %edi			/* addr */
	movl	12(%esp), %edx			/* val */
	/* VMWRITE DX, DI */
	.byte	0x0f; .byte 0x79; .byte 0xfa
	popl	%edi
	jc	_vmout1
	jz	_vmout2
	xorl	%eax, %eax
	ret

.globl vmread
vmread:
	/* GCC port: see vmwrite above -- same fix, %edi and %esi both */
	pushl	%edi
	pushl	%esi
	movl	12(%esp), %edi			/* addr */
	movl	16(%esp), %esi			/* valp */
	/* VMREAD (SI), DI */
	.byte	0x0f; .byte 0x78; .byte 0x3e
	popl	%esi
	popl	%edi
	jc	_vmout1
	jz	_vmout2
	xorl	%eax, %eax
	ret

.globl invept
invept:
	movl	4(%esp), %eax			/* type */
	/* INVEPT AX, 8(SP) */
	.byte	0x66; .byte 0x0f; .byte 0x38; .byte 0x80; .byte 0x44; .byte 0x24; .byte 0x08
	jmp	_vmout

.globl invvpid
invvpid:
	movl	4(%esp), %eax			/* type */
	/* INVVPID AX, 8(SP) */
	.byte	0x66; .byte 0x0f; .byte 0x38; .byte 0x81; .byte 0x44; .byte 0x24; .byte 0x08
	jmp	_vmout

_vmout:
	jc	_vmout1
	jz	_vmout2
	xorl	%eax, %eax
	ret
_vmout1:
	movl	$-1, %eax
	ret
_vmout2:
	movl	$-2, %eax
	ret

/*
 *  Used to get to the first process:
 * 	set up an interrupt return frame and IRET to user level.
 */
.globl touser
touser:
	pushl	$UDSEL				/* old ss */
	movl	4(%esp), %eax			/* old sp (note: +4 for the pushl above) */
	pushl	%eax
	movl	$0x200, %eax			/* interrupt enable flag */
	pushl	%eax				/* old flags */
	pushl	$UESEL				/* old cs */
	pushl	$(UTZERO+32)			/* old pc */
	movl	$UDSEL, %eax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %gs
	movw	%ax, %fs
	iretl

/*
 * Interrupt/exception handling.
 * Each entry in the vector table calls either _strayintr or _strayintrx
 * depending on whether an error code has been automatically pushed onto
 * the stack (_strayintrx) or not, in which case a dummy entry must be
 * pushed before retrieving the trap type from the vector table entry
 * and placing it on the stack as part of the Ureg structure. The size
 * of each entry in the vector table (6 bytes) is known in trapinit().
 */
.globl _strayintr
_strayintr:
	pushl	%eax			/* save AX */
	movl	4(%esp), %eax		/* return PC from vectortable */
	jmp	intrcommon

.globl _strayintrx
_strayintrx:
	xchgl	%eax, (%esp)		/* swap AX with vectortable CALL PC */
intrcommon:
	pushl	%ds			/* save DS */
	pushl	$KDSEL
	popl	%ds			/* fix up DS */
	movzbl	(%eax), %eax		/* trap type -> AX */
	xchgl	%eax, 4(%esp)		/* exchange trap type with saved AX */

	pushl	%es			/* save ES */
	pushl	$KDSEL
	popl	%es			/* fix up ES */

	pushl	%fs			/* save the rest of the Ureg struct */
	pushl	%gs
	pushal

	pushl	%esp			/* Ureg* argument to trap */
	call	trap

.globl forkret
forkret:
	/* has to be done before POPAL so we don't clobber the flags */
	xorl	%ecx, %ecx
	xorl	%ebx, %ebx
	xorl	%edx, %edx
	xorl	%esi, %esi
	xorl	%edi, %edi
	xorl	%ebp, %ebp

	popl	%eax
	popal
.globl _forkretpopgs
_forkretpopgs:
	popl	%gs
.globl _forkretpopfs
_forkretpopfs:
	popl	%fs
.globl _forkretpopes
_forkretpopes:
	popl	%es
.globl _forkretpopds
_forkretpopds:
	popl	%ds
	addl	$8, %esp		/* pop error code and trap type */
.globl _forkretiret
_forkretiret:
	iretl

/*
 * This is merely _strayintr optimised to vector to syscall() without
 * going through trap().
 */
.globl _syscallintr
_syscallintr:
	pushl	$VectorSYSCALL		/* trap type */

	pushl	%ds
	pushl	%es
	pushl	%fs
	pushl	%gs
	pushal
	movl	$KDSEL, %eax
	movw	%ax, %ds
	movw	%ax, %es

	pushl	%esp			/* Ureg* argument to syscall */
	pushl	$forkret		/* return pc */
	jmp	syscall

.globl vectortable
vectortable:
	call _strayintr; .byte 0x00		/* divide error */
	call _strayintr; .byte 0x01		/* debug exception */
	call _strayintr; .byte 0x02		/* NMI interrupt */
	call _strayintr; .byte 0x03		/* breakpoint */
	call _strayintr; .byte 0x04		/* overflow */
	call _strayintr; .byte 0x05		/* bound */
	call _strayintr; .byte 0x06		/* invalid opcode */
	call _strayintr; .byte 0x07		/* no coprocessor available */
	call _strayintrx; .byte 0x08		/* double fault */
	call _strayintr; .byte 0x09		/* coprocessor segment overflow */
	call _strayintrx; .byte 0x0A		/* invalid TSS */
	call _strayintrx; .byte 0x0B		/* segment not available */
	call _strayintrx; .byte 0x0C		/* stack exception */
	call _strayintrx; .byte 0x0D		/* general protection error */
	call _strayintrx; .byte 0x0E		/* page fault */
	call _strayintr; .byte 0x0F
	call _strayintr; .byte 0x10		/* coprocessor error */
	call _strayintrx; .byte 0x11		/* alignment check */
	call _strayintr; .byte 0x12		/* machine check */
	call _strayintr; .byte 0x13		/* simd error */
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
	call _strayintr; .byte 0x20		/* VectorLAPIC */
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
	call _syscallintr; .byte 0x40		/* VectorSYSCALL */
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
	call _strayintr; .byte 0x80		/* Vector[A]PIC */
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
