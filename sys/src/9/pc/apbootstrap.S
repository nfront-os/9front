/*
 * Start an Application Processor. This must be placed on a 4KB boundary
 * somewhere in the 1st MB of conventional memory (APBOOTSTRAP). However,
 * due to some shortcuts below it's restricted further to within the 1st
 * 64KB. The AP starts in real-mode, with
 *   CS selector set to the startup memory address/16;
 *   CS base set to startup memory address;
 *   CS limit set to 64KB;
 *   CPL and IP set to 0.
 *
 * GCC port: GAS translation of apbootstrap.s (as pc64/apbootstrap.S).
 * 8l truncated the 16-bit operands (WORD $gdtptr) to the low 16 bits of
 * the address; linked at APBOOTSTRAP that is sym-KZERO (the first far
 * jump loads CS with 0, so these are linear), spelled LO16(sym): the
 * offset into this code plus APBOOTSTRAP-KZERO, which GAS can resolve
 * before the link.
 */
#include "mem.h"

#define NOP		.byte 0x90		/* NOP */
#define LGDT(gdtptr)	.byte 0x0F;		/* LGDT */			\
			.byte 0x01; .byte 0x16;					\
			.word gdtptr
#define FARJUMP16(s, o)	.byte 0xEA;		/* far jump to ptr16:16 */	\
			.word o; .word s;					\
			NOP; NOP; NOP
#define FARJUMP32(s, o)	.byte 0x66;		/* far jump to ptr32:16 */	\
			.byte 0xEA; .long o; .word s

#define LO16(s)		((s)-apbootstrap+(APBOOTSTRAP-KZERO))

#define	DELAY		.byte 0xEB;		/* JMP .+2 */			\
			.byte 0x00

/*
 * Macros for calculating offsets within the page directory base
 * and page tables.
 */
#define PDO(a)		(((((a))>>22) & 0x03FF)<<2)
#define PTO(a)		(((((a))>>12) & 0x03FF)<<2)

.code16

.globl apbootstrap
apbootstrap:
	FARJUMP16(0, LO16(_apbootstrap))
.globl _apvector
_apvector:					/* address APBOOTSTRAP+0x08 */
	.long 0
.globl _appdb
_appdb:						/* address APBOOTSTRAP+0x0C */
	.long 0
.globl _apapic
_apapic:					/* address APBOOTSTRAP+0x10 */
	.long 0
.globl _apbootstrap
_apbootstrap:					/* address APBOOTSTRAP+0x14 */
	movw	%cs, %ax
	movw	%ax, %ds			/* initialise DS */

	LGDT(LO16(gdtptr))			/* load a basic gdt */

	movl	%cr0, %eax
	orl	$1, %eax
	movl	%eax, %cr0			/* turn on protected mode */
	DELAY					/* JMP .+2 */

	.byte 0xB8; .word SELECTOR(1, SELGDT, 0)/* MOVW $SELECTOR(1, SELGDT, 0), AX */
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss

	FARJUMP32(SELECTOR(2, SELGDT, 0), _ap32-KZERO)

/*
 * For Pentiums and higher, the code that enables paging must come from
 * pages that are identity mapped. 
 * To this end double map KZERO at virtual 0 and undo the mapping once virtual
 * nirvana has been obtained.
 */
.code32

.globl _ap32
_ap32:
	movl	_appdb-KZERO, %ecx		/* physical address of PDB */
	movl	PDO(KZERO)(%ecx), %edx		/* double-map KZERO at 0 */
	movl	%edx, PDO(0)(%ecx)
	movl	%ecx, %cr3			/* load and flush the mmu */

	movl	%cr4, %eax
	andl	$~0x00000020, %eax		/* ~PAE */
	orl	$0x00000010, %eax		/* PSE */
	movl	%eax, %cr4

	movl	%cr0, %edx
	orl	$0x80010000, %edx		/* PG|WP */
	andl	$~0x6000000A, %edx		/* ~(CD|NW|TS|MP) */

	movl	$_appg, %eax
	movl	%edx, %cr0			/* turn on paging */
	jmp	*%eax

.globl _appg
_appg:
	movl	%ecx, %eax			/* physical address of PDB */
	orl	$KZERO, %eax
	movl	$0, PDO(0)(%eax)		/* undo double-map of KZERO at 0 */
	movl	%ecx, %cr3			/* load and flush the mmu */

	movl	$(MACHADDR+MACHSIZE-4), %esp

	movl	$0, %eax
	pushl	%eax
	popfl

	movl	_apapic, %eax
	movl	%eax, (%esp)
	movl	_apvector, %eax
	call	*%eax
_aphalt:
	hlt
	jmp	_aphalt

.globl gdt
gdt:
	.long 0x0000; .long 0
	.long 0xFFFF; .long (SEGG|SEGB|(0xF<<16)|SEGP|SEGPL(0)|SEGDATA|SEGW)
	.long 0xFFFF; .long (SEGG|SEGD|(0xF<<16)|SEGP|SEGPL(0)|SEGEXEC|SEGR)
.globl gdtptr
gdtptr:
	.word	(3*8-1)
	.long	gdt-KZERO
