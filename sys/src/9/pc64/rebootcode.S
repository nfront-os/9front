#include "mem.h"

/*
 * Turn off MMU, then memmove the new kernel to its correct location
 * in physical memory.  Then jumps the to start of the kernel.
 */

.code64

/*
 * GCC port: called as a real C function pointer
 *	void (*)(void *entry, void *code, ulong n)
 * so args arrive via System V registers, not BP/(FP):
 *	%edi = destination (entry point), %esi = source, %edx = byte count.
 * %edi/%esi already hold what the original code moved into DI/SI.
 */
.globl main
main:
	movl	%edx, %ebx		/* byte count */

	/* load zero length idt */
	movl	$_idtptr64p, %eax
	lidt	(%rax)

	/* load temporary gdt */
	movl	$_gdtptr64p, %eax
	lgdt	(%rax)

	/* load CS with 32bit code segment */
	pushq	$SELECTOR(3, SELGDT, 0)
	pushq	$_warp32
	lretq

.code32

_warp32:

	/* load 32bit data segments */
	movl	$SELECTOR(2, SELGDT, 0), %eax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss

	/* turn off paging */
	movl	%cr0, %eax
	andl	$0x7fffffff, %eax	/* ~(PG) */
	movl	%eax, %cr0

	movl	$0, %eax
	movl	%eax, %cr3

	/* disable long mode */
	movl	$0xc0000080, %ecx	/* Extended Feature Enable */
	rdmsr
	andl	$0xfffffeff, %eax	/* Long Mode Disable */
	wrmsr

	/* diable pae */
	movl	%cr4, %eax
	andl	$0xffffff5f, %eax	/* ~(PAE|PGE) */
	movl	%eax, %cr4

	movl	%ebx, %ecx		/* byte count */
	movl	%edi, %eax		/* save entry point */
	movl	%eax, %esp		/* move stack below entry */

	/* park cpu for zero entry point */
	orl	%eax, %eax
	jz	_idle


/*
 * the source and destination may overlap.
 * determine whether to copy forward or backwards
 * GCC port: Plan9 CMP operands are Intel-order (a,b computes a-b);
 * AT&T CMP is written reversed (b,a) for the same a-b result.
 */
	cmpl	%edi, %esi
	jg	_forward
	movl	%esi, %edx
	addl	%ecx, %edx
	cmpl	%edi, %edx
	jg	_back

_forward:
	cld
	rep	movsb

_startkernel:
	/* jump to entry point */
	jmp	*%eax

_back:
	addl	%ecx, %edi
	addl	%ecx, %esi
	subl	$1, %edi
	subl	$1, %esi
	std
	rep	movsb
	jmp	_startkernel

_idle:
	hlt
	jmp	_idle

_gdt:
	/* null descriptor */
	.long	0
	.long	0

	/* (KESEG) 64 bit long mode exec segment */
	.long	0xFFFF
	.long	(SEGL|SEGG|SEGP|(0xF<<16)|SEGPL(0)|SEGEXEC|SEGR)

	/* 32 bit data segment descriptor for 4 gigabytes (PL 0) */
	.long	0xFFFF
	.long	(SEGG|SEGB|(0xF<<16)|SEGP|SEGPL(0)|SEGDATA|SEGW)

	/* 32 bit exec segment descriptor for 4 gigabytes (PL 0) */
	.long	0xFFFF
	.long	(SEGG|SEGD|(0xF<<16)|SEGP|SEGPL(0)|SEGEXEC|SEGR)

_gdtptr64p:
	.word	(4*8-1)
	.quad	_gdt

_idtptr64p:
	.word	0
	.quad	0
