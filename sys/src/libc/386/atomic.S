/* GCC port: GAS translation of atomic.s; %ecx/%edx instead of %ebx (callee-saved) */

/* get variants */
.globl ageti
.globl agetl
.globl agetp
ageti:
agetl:
agetp:
	movl	4(%esp), %eax		/* p */
	movl	0(%eax), %eax
	ret

/* set variants */
.globl aseti
.globl aswapl
.globl aswapp
aseti:
aswapl:
aswapp:
	movl	4(%esp), %ecx		/* p */
	movl	8(%esp), %eax		/* v */
	lock; xchgl (%ecx), %eax
	ret

/* inc variants */
.globl aincl
aincl:
	movl	4(%esp), %edx		/* p */
	movl	8(%esp), %ecx		/* v */
	movl	%ecx, %eax
	lock; xaddl %eax, (%edx)
	addl	%ecx, %eax
	ret

/* cas variants */
.globl acasl
.globl acasp
acasl:
acasp:
	movl	4(%esp), %ecx		/* p */
	movl	8(%esp), %eax		/* ov */
	movl	12(%esp), %edx		/* nv */
	lock; cmpxchgl %edx, (%ecx)
	jne	fail32
	movl	$1, %eax
	ret
fail32:
	movl	$0, %eax
	ret

/* barriers */
.globl coherence
coherence:
	/* this is essentially mfence but that requires sse2 */
	xorl	%eax, %eax
	lock; xaddl %eax, (%esp)
	ret
