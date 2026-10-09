/*
 * GCC port: GAS/AAPCS64 translation of l.s (7a), instruction for
 * instruction. The differences, each needed for gcc's ABI:
 *
 *  - 7c passes the first argument in R0 and the others on the stack
 *    (x+8(FP), ...); AAPCS64 passes them in x0-x7.
 *  - 7c treats every register as caller-saved; under AAPCS64 x19-x29
 *    are callee-saved. setlabel/gotolabel save and restore them too
 *    (Label in dat.h), and no routine here keeps scratch in them.
 *  - m and up stay in x27 and x26 (dat.h: register variables, the
 *    kernel and its libraries are compiled with -ffixed-x26 -ffixed-x27).
 *    gcc has no static base: 7l's R28 = setSB is not set, x28 is an
 *    ordinary callee-saved register (saved in the Ureg like before).
 *  - Before the MMU is on, $sym-KZERO(SB) is adrp/add, which yields the
 *    address the code runs at, the physical one.
 *  - cmpswap(long*) compares and swaps a long, 64 bits with gcc (Ref,
 *    Proc pids); cas(int*) stays 32-bit (semcas in fns.h).
 *  - returnto() rewrites the saved link of its caller, which gcc keeps
 *    in the caller's frame record at 8(x29) (7c: 0(SP)).
 */
#include "mem.h"

#define ADR(r, s)	adrp	r, s; add r, r, :lo12:s

.text

.globl _start
_start:
	mov	x26, x0		/* save */

	bl	svcmode

	/* use dedicated stack pointer per exception level */
	mov	w1, #1
	msr	spsel, x1

	bl	mmudisable

	/* invalidate local caches */
	bl	cachedwbinv
	bl	l2cacheuwbinv
	bl	cacheiinv

	ldr	x27, =(MACHADDR(0)-KZERO)
	mrs	x1, mpidr_el1
	and	w1, w1, #(MAXMACH-1)
	mov	w2, #MACHSIZE
	mul	w2, w1, w2
	sub	x27, x27, x2

	ldr	x2, =(MACHSIZE-16)
	add	x2, x27, x2
	mov	sp, x2

	cbnz	x1, _startup

	/* clear page table and machs */
	ldr	x1, =(L1BOT-KZERO)
	ldr	x2, =(MACHADDR(-1)-KZERO)
_zerol1:
	str	xzr, [x1], #8
	cmp	x2, x1
	b.ne	_zerol1

	/* clear BSS */
	ADR(x1, edata)
	ADR(x2, end)
_zerobss:
	str	xzr, [x1], #8
	cmp	x2, x1
	b.ne	_zerobss

	/* setup page tables */
	ldr	x0, =(L1BOT-KZERO)
	bl	mmuidmap

	ldr	x0, =(L1-KZERO)
	bl	mmu0init

	sevl
_startup:
	wfe
	bl	mmuenable

	mov	x0, x26
	mov	x26, #0
	orr	x27, x27, #KZERO
	msr	tpidr_el1, x27
	mov	x29, #0		/* end of the frame chain */

	bl	main

stop:
_stop:
	wfe
	b	_stop

aaa:
xxx:
	ldr	x1, =(0x860040+VIRTIO)
	mov	w2, #'A'
	str	w2, [x1]
	b	xxx

.globl sev
sev:
	sev
	wfe
	ret

svcmode:
	msr	daifset, #0xF
	mrs	x0, currentel
	and	w0, w0, #(3<<2)
	cmp	w0, #(1<<2)
	b.eq	el1
	cmp	w0, #(2<<2)
	b.eq	el2
	b	stop
el2:
	mov	x0, #0
	msr	mdcr_el2, x0
	isb	sy

	/* set virtual timer offset to zero */
	mov	x0, #0
	msr	cntvoff_el2, x0

	/* HCR = RW, HCD, SWIO, BSU, FB */
	ldr	w0, =(1<<31 | 1<<29 | 1<<2 | 0<<10 | 0<<9)
	msr	hcr_el2, x0
	isb	sy

	/* SCTLR = RES1 */
	ldr	w0, =(3<<4 | 1<<11 | 1<<16 | 1<<18 | 3<<22 | 3<<28)
	isb	sy
	msr	sctlr_el2, x0
	isb	sy

	/* set VMID to zero */
	mov	x0, #0
	msr	vttbr_el2, x0
	isb	sy

	mov	w0, #(0xF<<6 | 4)
	msr	spsr_el2, x0
	msr	elr_el2, x30
	eret
el1:
	ret

mmudisable:
#define SCTLRCLR \
	/* RES0 */	( 3<<30 \
	/* RES0 */	| 1<<27 \
	/* UCI */	| 1<<26 \
	/* EE */	| 1<<25 \
	/* RES0 */	| 1<<21 \
	/* E0E */	| 1<<24 \
	/* WXN */	| 1<<19 \
	/* nTWE */	| 1<<18 \
	/* RES0 */	| 1<<17 \
	/* nTWI */	| 1<<16 \
	/* UCT */	| 1<<15 \
	/* DZE */	| 1<<14 \
	/* RES0 */	| 1<<13 \
	/* RES0 */	| 1<<10 \
	/* UMA */	| 1<<9 \
	/* SA0 */	| 1<<4 \
	/* SA */	| 1<<3 \
	/* A */		| 1<<1 )
#define SCTLRSET \
	/* RES1 */	( 3<<28 \
	/* RES1 */	| 3<<22 \
	/* RES1 */	| 1<<20 \
	/* RES1 */	| 1<<11 )
#define SCTLRMMU \
	/* I */		( 1<<12 \
	/* C */		| 1<<2 \
	/* M */		| 1<<0 )

	/* initialise SCTLR, MMU and caches off */
	isb	sy
	mrs	x0, sctlr_el1
	ldr	x9, =(SCTLRCLR | SCTLRMMU)
	bic	x0, x0, x9
	ldr	x9, =SCTLRSET
	orr	x0, x0, x9
	isb	sy
	msr	sctlr_el1, x0
	isb	sy

	b	flushlocaltlb

mmuenable:
	/* return to virtual */
	orr	x30, x30, #KZERO
	str	x30, [sp, #-16]!

	bl	flushlocaltlb

	/* memory attributes */
#define MAIRINIT \
	( 0xFF << MA_MEM_WB*8 \
	| 0x33 << MA_MEM_WT*8 \
	| 0x44 << MA_MEM_UC*8 \
	| 0x00 << MA_DEV_nGnRnE*8 \
	| 0x04 << MA_DEV_nGnRE*8 \
	| 0x08 << MA_DEV_nGRE*8 \
	| 0x0C << MA_DEV_GRE*8 )
	ldr	x1, =MAIRINIT
	msr	mair_el1, x1
	isb	sy

	/* translation control */
#define TCRINIT \
	/* TBI1 */	( 0<<38 \
	/* TBI0 */	| 0<<37 \
	/* AS */	| 0<<36 \
	/* TG1 */	| (((3<<16|1<<14|2<<12)>>PGSHIFT)&3)<<30 \
	/* SH1 */	| SHARE_INNER<<28 \
	/* ORGN1 */	| CACHE_WB<<26 \
	/* IRGN1 */	| CACHE_WB<<24 \
	/* EPD1 */	| 0<<23 \
	/* A1 */	| 0<<22 \
	/* T1SZ */	| (64-EVASHIFT)<<16 \
	/* TG0 */	| (((1<<16|2<<14|0<<12)>>PGSHIFT)&3)<<14 \
	/* SH0 */	| SHARE_INNER<<12 \
	/* ORGN0 */	| CACHE_WB<<10 \
	/* IRGN0 */	| CACHE_WB<<8 \
	/* EPD0 */	| 0<<7 \
	/* T0SZ */	| (64-EVASHIFT)<<0 )
	ldr	x1, =TCRINIT
	mrs	x2, id_aa64mmfr0_el1
	and	w2, w2, #0x7	// PARange
	add	x1, x1, x2, lsl #32	// IPS
	msr	tcr_el1, x1
	isb	sy

	/* load the page tables */
	ldr	x0, =(L1BOT-KZERO)
	ldr	x1, =(L1TOP-KZERO)
	isb	sy
	msr	ttbr0_el1, x0
	msr	ttbr1_el1, x1
	isb	sy

	/* enable MMU and caches */
	mrs	x1, sctlr_el1
	ldr	x9, =SCTLRMMU
	orr	x1, x1, x9
	isb	sy
	msr	sctlr_el1, x1
	isb	sy

	mov	x1, sp
	orr	x1, x1, #KZERO
	mov	sp, x1
	ldr	x30, [sp], #16
	b	cacheiinv

.globl touser
touser:
	ldr	w1, =0x10028	// entry: UTZERO+0x28 behind 7l's header (mem.h's 0ULL is not readable for GAS)
	mov	w2, #0		// psr
	msr	sp_el0, x0	// sp
	msr	elr_el1, x1
	msr	spsr_el1, x2
	eret

/* int cas(int *p, int ov, int nv): 32 bits (semcas) */
.globl cas
cas:
	mov	w1, w1		// MOVWU ov
	mov	w2, w2		// MOVWU nv
_cas1:
	ldxr	w3, [x0]
	cmp	x1, x3
	b.ne	_cas0
	stxr	w4, w2, [x0]
	cbnz	w4, _cas1
	mov	w0, #1
	dmb	ish
	ret
_cas0:
	clrex
	mov	w0, #0
	ret

/* int cmpswap(long *p, long ov, long nv): gcc's long is 64 bits */
.globl cmpswap
cmpswap:
_cmpswap1:
	ldxr	x3, [x0]
	cmp	x1, x3
	b.ne	_cas0
	stxr	w4, x2, [x0]
	cbnz	w4, _cmpswap1
	mov	w0, #1
	dmb	ish
	ret

.globl tas
.globl _tas
tas:
_tas:
	ldr	w2, =0xdeaddead
_tas1:
	ldxr	w1, [x0]
	stxr	w3, w2, [x0]
	cbnz	w3, _tas1
	mov	w0, w1

.globl coherence
coherence:
	dmb	ish
	ret

.globl islo
islo:
	mrs	x0, daif
	and	x0, x0, #(0x2<<6)
	eor	x0, x0, #(0x2<<6)
	ret

.globl splhi
splhi:
	mrs	x0, daif
	msr	daifset, #0x2
	ret

.globl splfhi
splfhi:
	mrs	x0, daif
	msr	daifset, #0x3
	ret

.globl spllo
spllo:
	msr	daifclr, #0x3
	ret

.globl splflo
splflo:
	msr	daifclr, #0x1
	ret

.globl splx
splx:
	msr	daif, x0
	ret

.globl idlehands
idlehands:
	dmb	ish
	ADR(x0, nrdy)
	ldr	w0, [x0]
	cbnz	w0, _ready
	wfi
_ready:
	ret

.globl vcycles
vcycles:
	mrs	x0, cntvct_el0
	ret

.globl lcycles
lcycles:
	mrs	x0, pmccntr_el0
	ret

/*
 * Label: sp, pc, then the callee-saved x19-x28 and x29 (dat.h)
 */
.globl setlabel
setlabel:
	mov	x1, sp
	stp	x1, x30, [x0, #0]
	stp	x19, x20, [x0, #16]
	stp	x21, x22, [x0, #32]
	stp	x23, x24, [x0, #48]
	stp	x25, x26, [x0, #64]
	stp	x27, x28, [x0, #80]
	str	x29, [x0, #96]
	mov	w0, #0
	ret

.globl gotolabel
gotolabel:
	ldp	x1, x30, [x0, #0]	/* sp, link */
	ldp	x19, x20, [x0, #16]
	ldp	x21, x22, [x0, #32]
	ldp	x23, x24, [x0, #48]
	ldr	x25, [x0, #64]		/* not x26, x27: up, m (fixed) */
	ldr	x28, [x0, #88]
	ldr	x29, [x0, #96]
	mov	sp, x1
	mov	w0, #1
	ret

.globl returnto
returnto:
	str	x0, [x29, #8]
	ret

.globl getfar
getfar:
	mrs	x0, far_el1
	ret

.globl setttbr
setttbr:
	dsb	ishst
	msr	ttbr0_el1, x0
	dsb	ish
	isb	sy
	ret

/*
 * TLB maintenance operations.
 * these broadcast to all cpu's in the cluser
 * (inner sharable domain).
 */
.globl flushasidva
.globl tlbivae1is
flushasidva:
tlbivae1is:
	dsb	ishst
	tlbi	vae1is, x0
	dsb	ish
	isb	sy
	ret

.globl flushasidvall
.globl tlbivale1is
flushasidvall:
tlbivale1is:
	dsb	ishst
	tlbi	vale1is, x0
	dsb	ish
	isb	sy
	ret

.globl flushasid
.globl tlbiaside1is
flushasid:
tlbiaside1is:
	dsb	ishst
	tlbi	aside1is, x0
	dsb	ish
	isb	sy
	ret

.globl flushtlb
.globl tlbivmalle1is
flushtlb:
tlbivmalle1is:
	dsb	ishst
	tlbi	vmalle1is
	dsb	ish
	isb	sy
	ret

/*
 * flush the tlb of this cpu. no broadcast.
 */
.globl flushlocaltlb
.globl tlbivmalle1
flushlocaltlb:
tlbivmalle1:
	dsb	nshst
	tlbi	vmalle1
	dsb	nsh
	isb	sy
	ret

/*
 * floating-point support.
 */
.globl fpon
fpon:
	mov	w0, #(3<<20)
	msr	cpacr_el1, x0
	isb	sy
	ret

.globl fpoff
fpoff:
	mov	w0, #(0<<20)
	msr	cpacr_el1, x0
	isb	sy
	ret

.globl fpsaveregs
fpsaveregs:
	.word	(1<<30 | 3 << 26 | 2<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 0)  /* MOV { V0, V1, V2, V3  }, (R0)64! */
	.word	(1<<30 | 3 << 26 | 2<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 4)  /* MOV { V4, V5, V6, V7  }, (R0)64! */
	.word	(1<<30 | 3 << 26 | 2<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 8)  /* MOV { V8, V9, V10,V11 }, (R0)64! */
	.word	(1<<30 | 3 << 26 | 2<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 12) /* MOV { V12,V13,V14,V15 }, (R0)64! */
	.word	(1<<30 | 3 << 26 | 2<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 16) /* MOV { V16,V17,V18,V19 }, (R0)64! */
	.word	(1<<30 | 3 << 26 | 2<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 20) /* MOV { V20,V21,V22,V23 }, (R0)64! */
	.word	(1<<30 | 3 << 26 | 2<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 24) /* MOV { V24,V25,V26,V27 }, (R0)64! */
	.word	(1<<30 | 3 << 26 | 2<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 28) /* MOV { V28,V29,V30,V31 }, (R0)64! */
	ret

.globl fploadregs
fploadregs:
	.word	(1<<30 | 3 << 26 | 3<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 0)  /* MOV (R0)64!, { V0, V1, V2, V3  } */
	.word	(1<<30 | 3 << 26 | 3<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 4)  /* MOV (R0)64!, { V4, V5, V6, V7  } */
	.word	(1<<30 | 3 << 26 | 3<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 8)  /* MOV (R0)64!, { V8, V9, V10,V11 } */
	.word	(1<<30 | 3 << 26 | 3<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 12) /* MOV (R0)64!, { V12,V13,V14,V15 } */
	.word	(1<<30 | 3 << 26 | 3<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 16) /* MOV (R0)64!, { V16,V17,V18,V19 } */
	.word	(1<<30 | 3 << 26 | 3<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 20) /* MOV (R0)64!, { V20,V21,V22,V23 } */
	.word	(1<<30 | 3 << 26 | 3<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 24) /* MOV (R0)64!, { V24,V25,V26,V27 } */
	.word	(1<<30 | 3 << 26 | 3<<22 | 0x1F<<16 | 3<<10 | 0<<5 | 28) /* MOV (R0)64!, { V28,V29,V30,V31 } */
	ret

// syscall or trap from EL0
.globl vsys0
vsys0:
	lsr	w17, w0, #26	// ec
	cmp	w17, #0x15	// SVC trap?
	b.ne	_itsatrap	// nope.

	stp	x26, x27, [sp, #224]
	stp	x28, x29, [sp, #240]

	mrs	x1, sp_el0
	mrs	x2, elr_el1
	mrs	x3, spsr_el1

	str	x0, [sp, #288]	// type
	str	x1, [sp, #264]	// sp
	str	x2, [sp, #272]	// pc
	str	x3, [sp, #280]	// psr

	mrs	x27, tpidr_el1
	ldr	x26, [x27, #16]
	mov	x29, #0		// end of the frame chain

	add	x0, sp, #16	// ureg
	bl	syscall

.globl forkret
forkret:
	msr	daifset, #0x3	// interrupts off

	add	x0, sp, #16	// ureg

	ldr	x0, [sp, #16]	// ret
	ldr	x1, [sp, #264]	// sp
	ldr	x2, [sp, #272]	// pc
	ldr	x3, [sp, #280]	// psr

	msr	sp_el0, x1
	msr	elr_el1, x2
	msr	spsr_el1, x3

	ldp	x26, x27, [sp, #224]
	ldp	x28, x29, [sp, #240]

	ldr	x30, [sp, #256]	// link

	add	sp, sp, #TRAPFRAMESIZE
	eret

_itsatrap:
	stp	x1, x2, [sp, #24]
	stp	x3, x4, [sp, #40]
	stp	x5, x6, [sp, #56]
	stp	x7, x8, [sp, #72]
	stp	x9, x10, [sp, #88]
	stp	x11, x12, [sp, #104]
	stp	x13, x14, [sp, #120]
	stp	x15, x16, [sp, #136]

	stp	x18, x19, [sp, #160]
	stp	x20, x21, [sp, #176]
	stp	x22, x23, [sp, #192]
	stp	x24, x25, [sp, #208]

// trap/irq/fiq/serr from EL0
.globl vtrap0
vtrap0:
	stp	x26, x27, [sp, #224]
	stp	x28, x29, [sp, #240]

	mrs	x1, sp_el0
	mrs	x2, elr_el1
	mrs	x3, spsr_el1

	str	x0, [sp, #288]	// type
	str	x1, [sp, #264]	// sp
	str	x2, [sp, #272]	// pc
	str	x3, [sp, #280]	// psr

	mrs	x27, tpidr_el1
	ldr	x26, [x27, #16]
	mov	x29, #0		// end of the frame chain

	add	x0, sp, #16	// ureg
	bl	trap

.globl noteret
noteret:
	msr	daifset, #0x3	// interrupts off

	add	x0, sp, #16	// ureg

	ldr	x1, [sp, #264]	// sp
	ldr	x2, [sp, #272]	// pc
	ldr	x3, [sp, #280]	// psr

	msr	sp_el0, x1
	msr	elr_el1, x2
	msr	spsr_el1, x3

	ldp	x26, x27, [sp, #224]
	ldp	x28, x29, [sp, #240]

_intrreturn:
	ldp	x0, x1, [sp, #16]
	ldp	x2, x3, [sp, #32]
	ldp	x4, x5, [sp, #48]
	ldp	x6, x7, [sp, #64]
	ldp	x8, x9, [sp, #80]
	ldp	x10, x11, [sp, #96]
	ldp	x12, x13, [sp, #112]
	ldp	x14, x15, [sp, #128]
	ldp	x16, x17, [sp, #144]
	ldp	x18, x19, [sp, #160]
	ldp	x20, x21, [sp, #176]
	ldp	x22, x23, [sp, #192]
	ldp	x24, x25, [sp, #208]

	ldr	x30, [sp, #256]	// link

	add	sp, sp, #TRAPFRAMESIZE
	eret

// irq/fiq/trap/serr from EL1
.globl vtrap1
vtrap1:
	str	x29, [sp, #248]	// special

	add	x1, sp, #TRAPFRAMESIZE
	mrs	x2, elr_el1
	mrs	x3, spsr_el1

	str	x0, [sp, #288]	// type
	str	x1, [sp, #264]	// sp
	str	x2, [sp, #272]	// pc
	str	x3, [sp, #280]	// psr

	add	x0, sp, #16	// ureg
	bl	trap

	msr	daifset, #0x3	// interrupts off

	ldr	x2, [sp, #272]	// pc
	ldr	x3, [sp, #280]	// psr

	msr	elr_el1, x2
	msr	spsr_el1, x3

	ldr	x29, [sp, #248]	// special
	b	_intrreturn

// vector tables
.globl vsys
vsys:
	sub	sp, sp, #TRAPFRAMESIZE

	str	x0, [sp, #16]
	str	x30, [sp, #256]	// link

	str	x17, [sp, #152]	// temp

	mrs	x0, esr_el1	// type

.globl _vsyspatch
_vsyspatch:
	b	_vsyspatch	// branch to vsys0() patched in

.globl vtrap
vtrap:
	sub	sp, sp, #TRAPFRAMESIZE

	stp	x0, x1, [sp, #16]
	stp	x2, x3, [sp, #32]
	stp	x4, x5, [sp, #48]
	stp	x6, x7, [sp, #64]
	stp	x8, x9, [sp, #80]
	stp	x10, x11, [sp, #96]
	stp	x12, x13, [sp, #112]
	stp	x14, x15, [sp, #128]
	stp	x16, x17, [sp, #144]
	stp	x18, x19, [sp, #160]
	stp	x20, x21, [sp, #176]
	stp	x22, x23, [sp, #192]
	stp	x24, x25, [sp, #208]

	str	x30, [sp, #256]	// link

	mrs	x0, esr_el1	// type

.globl _vtrappatch
_vtrappatch:
	b	_vtrappatch	// branch to vtrapX() patched in

.globl virq
virq:
	sub	sp, sp, #TRAPFRAMESIZE

	stp	x0, x1, [sp, #16]
	stp	x2, x3, [sp, #32]
	stp	x4, x5, [sp, #48]
	stp	x6, x7, [sp, #64]
	stp	x8, x9, [sp, #80]
	stp	x10, x11, [sp, #96]
	stp	x12, x13, [sp, #112]
	stp	x14, x15, [sp, #128]
	stp	x16, x17, [sp, #144]
	stp	x18, x19, [sp, #160]
	stp	x20, x21, [sp, #176]
	stp	x22, x23, [sp, #192]
	stp	x24, x25, [sp, #208]

	str	x30, [sp, #256]	// link

	mov	x0, #(1<<32)	// type irq

.globl _virqpatch
_virqpatch:
	b	_virqpatch	// branch to vtrapX() patched in

.globl vfiq
vfiq:
	sub	sp, sp, #TRAPFRAMESIZE

	stp	x0, x1, [sp, #16]
	stp	x2, x3, [sp, #32]
	stp	x4, x5, [sp, #48]
	stp	x6, x7, [sp, #64]
	stp	x8, x9, [sp, #80]
	stp	x10, x11, [sp, #96]
	stp	x12, x13, [sp, #112]
	stp	x14, x15, [sp, #128]
	stp	x16, x17, [sp, #144]
	stp	x18, x19, [sp, #160]
	stp	x20, x21, [sp, #176]
	stp	x22, x23, [sp, #192]
	stp	x24, x25, [sp, #208]

	str	x30, [sp, #256]	// link
	mov	x0, #(2<<32)	// type fiq

.globl _vfiqpatch
_vfiqpatch:
	b	_vfiqpatch	// branch to vtrapX() patched in

.globl vserr
vserr:
	sub	sp, sp, #TRAPFRAMESIZE

	stp	x0, x1, [sp, #16]
	stp	x2, x3, [sp, #32]
	stp	x4, x5, [sp, #48]
	stp	x6, x7, [sp, #64]
	stp	x8, x9, [sp, #80]
	stp	x10, x11, [sp, #96]
	stp	x12, x13, [sp, #112]
	stp	x14, x15, [sp, #128]
	stp	x16, x17, [sp, #144]
	stp	x18, x19, [sp, #160]
	stp	x20, x21, [sp, #176]
	stp	x22, x23, [sp, #192]
	stp	x24, x25, [sp, #208]

	str	x30, [sp, #256]	// link

	mrs	x0, esr_el1
	orr	x0, x0, #(3<<32)	// type
.globl _vserrpatch
_vserrpatch:
	b	_vserrpatch	// branch to vtrapX() patched in

/* fault-proof memcpy: peek(src, dst, len) */
.globl peek
peek:
	mov	x9, x1		// dst (7c: dst+8(FP))
	mov	x1, x0		// src
	mov	w0, w2		// MOVWU len+16(FP)
	mov	x2, x9
.globl _peekinst
_peekinst:
_peekloop:
	ldrb	w3, [x1], #1
	strb	w3, [x2], #1
	subs	x0, x0, #1
	b.ne	_peekloop
	ret

/*
 * hvccall(Ureg*): the original's $32 frame holds R26, R27 and the
 * Ureg pointer; the frame record (x29, x30) comes first here.
 */
.globl hvccall
hvccall:
	stp	x29, x30, [sp, #-48]!
	mov	x29, sp
	/* save extern registers */
	stp	x26, x27, [sp, #16]

	/* R0 = Ureg */
	mov	x8, x0

	/* save ureg pointer */
	str	x8, [sp, #32]

	ldp	x0, x1, [x8, #0]
	ldp	x2, x3, [x8, #16]
	ldp	x4, x5, [x8, #32]
	ldp	x6, x7, [x8, #48]

	hvc	#0

	/* restore ureg pointer */
	ldr	x8, [sp, #32]

	stp	x0, x1, [x8, #0]
	stp	x2, x3, [x8, #16]

	/* restore extern registers */
	ldp	x26, x27, [sp, #16]

	ldp	x29, x30, [sp], #48
	ret
