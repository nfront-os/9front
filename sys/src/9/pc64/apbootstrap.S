/*
 * Start an Application Processor. This must be placed on a 4KB boundary
 * somewhere in the 1st MB of conventional memory (APBOOTSTRAP). However,
 * due to some shortcuts below it's restricted further to within the 1st
 * 64KB. The AP starts in real-mode, with
 *   CS selector set to the startup memory address/16;
 *   CS base set to startup memory address;
 *   CS limit set to 64KB;
 *   CPL and IP set to 0.
 */
#include "mem.h"

#define NOP		.byte 0x90		/* NOP */

#define pFARJMP32(s, o)	.byte 0xea;		/* far jmp ptr32:16 */	\
			.long o; .word s

#define rFARJMP16(s, o)	.byte 0xea;		/* far jump ptr16:16 */	\
			.word o; .word s

#define rFARJMP32(s, o)	.byte 0x66;		/* far jump ptr32:16 */	\
			pFARJMP32(s, o)

#define rLGDT(gdtptr)	.byte 0x0f;		/* LGDT */		\
			.byte 0x01; .byte 0x16;			\
			.word gdtptr

#define rMOVAX(i)	.byte 0xb8;		/* i -> AX */		\
			.word i

#define	DELAY		.byte 0xEB;		/* JMP .+2 */		\
			.byte 0x00

.code16gcc

.globl apbootstrap
apbootstrap:
	rFARJMP16(0, _apbootstrap-KZERO)
	NOP; NOP; NOP;
.globl _apvector
_apvector:					/* address APBOOTSTRAP+0x08 */
	.quad 0
.globl _appml4
_appml4:					/* address APBOOTSTRAP+0x10 */
	.quad 0
.globl _apapic
_apapic:					/* address APBOOTSTRAP+0x18 */
	.quad 0
.globl _apmach
_apmach:					/* address APBOOTSTRAP+0x20 */
	.quad 0
.globl _apefer
_apefer:
	.quad 0x100				/* Long Mode Enable */

.globl _apbootstrap
_apbootstrap:
	movw	%cs, %ax
	movw	%ax, %ds			/* initialise DS */

	rLGDT(_gdtptr32p-KZERO)		/* load a basic gdt */

	movl	%cr0, %eax
	orl	$1, %eax
	movl	%eax, %cr0			/* turn on protected mode */
	DELAY					/* JMP .+2 */

	rFARJMP16(SELECTOR(3, SELGDT, 0), _ap32-KZERO)

/*
 * Enable and activate Long Mode. From the manual:
 * 	make sure Page Size Extentions are off, and Page Global
 *	Extensions and Physical Address Extensions are on in CR4;
 *	set Long Mode Enable in the Extended Feature Enable MSR;
 *	set Paging Enable in CR0;
 *	make an inter-segment jump to the Long Mode code.
 * It's all in 32-bit mode until the jump is made.
 */
.code32

.globl _ap32
_ap32:
	movw	$SELECTOR(2, SELGDT, 0), %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss

	movl	_appml4-KZERO, %eax	/* physical address of PML4 */
	movl	%eax, %cr3			/* load the mmu */
	DELAY

	movl	%cr4, %eax
	andl	$~0x00000010, %eax		/* Page Size */
	orl	$0x000000A0, %eax		/* Page Global, Phys. Address */
	movl	%eax, %cr4

	movl	$0xc0000080, %ecx		/* Extended Feature Enable */
	rdmsr
	orl	_apefer-KZERO, %eax
	wrmsr

	movl	%cr0, %edx
	andl	$~0x6000000a, %edx
	orl	$0x80010000, %edx		/* Paging Enable, Write Protect */
	movl	%edx, %cr0

	pFARJMP32(SELECTOR(KESEG, SELGDT, 0), _ap64-KZERO)

/*
 * Long mode. Welcome to 2003.
 * Jump out of the identity map space;
 * load a proper long mode GDT;
 * initialise the stack and call the
 * C startup code in m->splpc.
 */
.code64

.globl _ap64
_ap64:
	movq	$_gdtptr64v, %rax
	lgdt	(%rax)

	xorq	%rax, %rax
	movw	%ax, %ds			/* not used in long mode */
	movw	%ax, %es			/* not used in long mode */
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss			/* not used in long mode */

	lldt	%ax

	movq	_apmach, %rsp

	movq	%rax, RUSER			/* up = 0; */
	movq	%rsp, RMACH			/* m = apmach */

	addq	$MACHSIZE, %rsp

	pushq	%rax				/* clear flags */
	popfq

	movq	_apvector, %rax
	movq	_apapic, %rdi			/* GCC port: squidboy(apic) is a System V function, was RARG (%rbp, 6c) */
	pushq	%rdi

	call	*%rax

_halt:
	hlt
	jmp	_halt

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
