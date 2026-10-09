/*
 * Far call, absolute indirect.
 * The argument is the offset.
 * We use a global structure for the jump params,
 * so this is *not* reentrant or thread safe.
 *
 * GCC port: GAS translation of apmjump.s. The BIOS returns values in
 * %ebx and %esi, and %edi holds the Ureg*: callee-saved under cdecl
 * (scratch for 8c), they are saved on entry, so the arguments are 12
 * bytes further up the stack.
 */

#include "mem.h"

.comm	apmjumpstruct, 8, 4

.globl apmfarcall
apmfarcall:
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	/*
	 * We call push and pop ourselves.
	 * As soon as we do the first push or pop,
	 * we can't use FP anymore.
	 */
	movl	20(%esp), %ebx		/* off */
	movl	16(%esp), %ecx		/* seg */
	movl	%ebx, apmjumpstruct+0
	movl	%ecx, apmjumpstruct+4

	/* load necessary registers from Ureg */
	movl	24(%esp), %edi		/* ureg */
	movl	28(%edi), %eax
	movl	16(%edi), %ebx
	movl	24(%edi), %ecx
	movl	20(%edi), %edx

	/* save registers, segments */
	pushl	%ds
	pushl	%es
	pushl	%fs
	pushl	%gs
	pushl	%ebp
	pushl	%edi

	/*
	 * paranoia: zero the segments, since it's the
	 * BIOS's responsibility to initialize them.
	 * (trick picked up from Linux driver).
	 */
	pushl	%edx
	xorl	%edx, %edx
	pushl	%edx
	popl	%ds
	pushl	%edx
	popl	%es
	pushl	%edx
	popl	%fs
	pushl	%edx
	popl	%gs
	popl	%edx

	/*
	 * The actual call.
	 */
	lcall	*%cs:apmjumpstruct

	/* restore segments, registers */
	popl	%edi
	popl	%ebp
	popl	%gs
	popl	%fs
	popl	%es
	popl	%ds

	pushfl
	popl	64(%edi)

	/* store interesting registers back in Ureg */
	movl	%eax, 28(%edi)
	movl	%ebx, 16(%edi)
	movl	%ecx, 24(%edi)
	movl	%edx, 20(%edi)
	movl	%esi, 4(%edi)

	pushfl
	popl	%eax
	andl	$1, %eax	/* carry flag */
	popl	%edi
	popl	%esi
	popl	%ebx
	ret
