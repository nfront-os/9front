#include "mem.h"

/*
 * Turn off MMU, then memmove the new kernel to its correct location
 * in physical memory.  Then jumps the to start of the kernel.
 *
 * GCC port: called as a real C function pointer
 *	void (*)(void *p1, void *p2, ulong n)
 * -- cdecl, so args stay on the stack exactly where front's FP-relative
 * offsets already put them (p1+0(FP) -> 4(%esp), etc).
 */
.globl main
main:
	movl	4(%esp), %edi		/* destination */
	movl	%edi, %eax		/* entry point */
	movl	8(%esp), %esi		/* source */
	movl	12(%esp), %ecx		/* byte count */

/*
 * disable paging
 */
	movl	%cr0, %edx
	andl	$~0x80000000, %edx	/* ~(PG) */
	movl	%edx, %cr0
	movl	$0, %edx
	movl	%edx, %cr3

	/* stack below entry point */
	movl	%eax, %esp

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
	rep; movsb
	jmp	_startkernel

_back:
	addl	%ecx, %edi
	addl	%ecx, %esi
	subl	$1, %edi
	subl	$1, %esi
	std
	rep; movsb
	jmp	_startkernel

/*
 * JMP to kernel entry point. Note the true kernel entry point is the
 * virtual address KZERO|AX, but this must wait until the MMU is
 * enabled by the kernel in l.S.
 */
_startkernel:
	orl	%eax, %eax		/* NOP: avoid link bug */
	jmp	*%eax

_idle:
	hlt
	jmp	_idle
