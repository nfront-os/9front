/*
 * GCC port: GAS translation of vlop.s, 8c's vlong runtime (gcc code
 * uses libgcc instead): same hidden-pointer convention as 8c, which is
 * the only caller; %ebx saved (callee-saved under cdecl).
 */
.globl _mulv
_mulv:
	pushl	%ebx
	movl	8(%esp), %ecx		/* r */
	movl	12(%esp), %eax		/* a */
	mull	20(%esp)		/* b */
	movl	%eax, 0(%ecx)
	movl	%edx, %ebx
	movl	12(%esp), %eax
	mull	24(%esp)
	addl	%eax, %ebx
	movl	16(%esp), %eax
	mull	20(%esp)
	addl	%eax, %ebx
	movl	%ebx, 4(%ecx)
	popl	%ebx
	ret

.globl _mul64by32
_mul64by32:
	pushl	%ebx
	movl	8(%esp), %ecx		/* r */
	movl	12(%esp), %eax		/* a */
	mull	20(%esp)		/* b */
	movl	%eax, 0(%ecx)
	movl	%edx, %ebx
	movl	16(%esp), %eax
	mull	20(%esp)
	addl	%eax, %ebx
	movl	%ebx, 4(%ecx)
	popl	%ebx
	ret

.globl _div64by32
_div64by32:
	movl	16(%esp), %ecx		/* r */
	movl	4(%esp), %eax		/* a */
	movl	8(%esp), %edx
	divl	12(%esp)		/* b */
	movl	%edx, 0(%ecx)
	ret

.globl _addv
_addv:					/* used in profiler, can't be profiled */
	pushl	%ebx
	movl	8(%esp), %ecx		/* r */
	movl	12(%esp), %eax		/* a */
	movl	16(%esp), %ebx
	addl	20(%esp), %eax		/* b */
	adcl	24(%esp), %ebx
	movl	%eax, 0(%ecx)
	movl	%ebx, 4(%ecx)
	popl	%ebx
	ret

.globl _subv
_subv:					/* used in profiler, can't be profiled */
	pushl	%ebx
	movl	8(%esp), %ecx		/* r */
	movl	12(%esp), %eax		/* a */
	movl	16(%esp), %ebx
	subl	20(%esp), %eax		/* b */
	sbbl	24(%esp), %ebx
	movl	%eax, 0(%ecx)
	movl	%ebx, 4(%ecx)
	popl	%ebx
	ret
