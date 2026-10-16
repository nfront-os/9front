/*
 * GCC port: GAS/System V translation: data, len, state arrive in %rdi,
 * %esi, %rdx. state is kept in %r9 and edata in %r10 (the original
 * read them from the stack); %rbx and %rbp are callee-saved under
 * System V and pushed. The locals are those of the original's $384
 * frame, at their offsets from its top (6a's pseudo-SP) turned into
 * offsets from %rsp; LEAL's address needs E's 64-bit name: E64.
 */

/* x = (wp[off-f] ^ wp[off-8] ^ wp[off-14] ^ wp[off-16]) <<< 1;
 * wp[off] = x;
 * x += A <<< 5;
 * E += 0xca62c1d6 + x;
 * x = FN(B,C,D);
 * E += x;
 * B >>> 2
 */
#define BODY(off,FN,V,A,B,C,D,E,E64)\
	movl (off-64)(%rbp),%edi;\
	xorl (off-56)(%rbp),%edi;\
	xorl (off-32)(%rbp),%edi;\
	xorl (off-12)(%rbp),%edi;\
	roll $1,%edi;\
	movl %edi,off(%rbp);\
	leal V(%rdi,E64,1),E;\
	movl A,%edi;\
	roll $5,%edi;\
	addl %edi,E;\
	FN(B,C,D)\
	addl %edi,E;\
	rorl $2,B;\

#define BODY0(off,FN,V,A,B,C,D,E,E64)\
	movl off(%rbx),%edi;\
	bswap %edi;\
	movl %edi,off(%rbp);\
	leal V(%rdi,E64,1),E;\
	movl A,%edi;\
	roll $5,%edi;\
	addl %edi,E;\
	FN(B,C,D)\
	addl %edi,E;\
	rorl $2,B;\

/*
 * fn1 = (((C^D)&B)^D);
 */
#define FN1(B,C,D)\
	movl C,%edi;\
	xorl D,%edi;\
	andl B,%edi;\
	xorl D,%edi;\

/*
 * fn24 = B ^ C ^ D
 */
#define FN24(B,C,D)\
	movl B,%edi;\
	xorl C,%edi;\
	xorl D,%edi;\

/*
 * fn3 = ((B ^ C) & (D ^= B)) ^ B
 * D ^= B to restore D
 */
#define FN3(B,C,D)\
	movl B,%edi;\
	xorl C,%edi;\
	xorl B,D;\
	andl D,%edi;\
	xorl B,%edi;\
	xorl B,D;\

/*
 * locals, from %rsp (the original's offsets from its frame's top + 384)
 * ulong w[80];
 * ulong *w15, *w40, *w60, *w80;
 * register local
 * ulong *wp = BP
 * ulong a = eax, b = ebx, c = ecx, d = edx, e = esi
 * ulong tmp = edi
 */
#define WARRAY	(384-8-(80*4))
#define TMP1	(384-16-(80*4))
#define TMP2	(384-24-(80*4))
#define W15	(384-32-(80*4))
#define W40	(384-40-(80*4))
#define W60	(384-48-(80*4))
#define W80	(384-56-(80*4))

.globl _sha1block
_sha1block:
	pushq	%rbx
	pushq	%rbp
	subq	$384, %rsp
	movq	%rdi, %r8		/* Rpdata */
	movq	%rdx, %r9		/* state */
	movl	%esi, %ebx		/* len, zero-extended */
	leaq	(%rdi,%rbx), %r10	/* edata */

	leaq	(WARRAY+15*4)(%rsp), %rdi
	movq	%rdi, W15(%rsp)
	leaq	(WARRAY+40*4)(%rsp), %rdx
	movq	%rdx, W40(%rsp)
	leaq	(WARRAY+60*4)(%rsp), %rcx
	movq	%rcx, W60(%rsp)
	leaq	(WARRAY+80*4)(%rsp), %rdi
	movq	%rdi, W80(%rsp)

1:
	leaq	WARRAY(%rsp), %rbp

	movl	(%r9), %eax
	movl	4(%r9), %ebx
	movl	%ebx, TMP1(%rsp)
	movl	8(%r9), %ecx
	movl	12(%r9), %edx
	movl	16(%r9), %esi

	movq	%r8, %rbx

2:
	BODY0(0,FN1,0x5a827999,%eax,TMP1(%rsp),%ecx,%edx,%esi,%rsi)
	movl	%esi, TMP2(%rsp)
	BODY0(4,FN1,0x5a827999,%esi,%eax,TMP1(%rsp),%ecx,%edx,%rdx)
	movl	TMP1(%rsp), %esi
	BODY0(8,FN1,0x5a827999,%edx,TMP2(%rsp),%eax,%esi,%ecx,%rcx)
	BODY0(12,FN1,0x5a827999,%ecx,%edx,TMP2(%rsp),%eax,%esi,%rsi)
	movl	%esi, TMP1(%rsp)
	BODY0(16,FN1,0x5a827999,%esi,%ecx,%edx,TMP2(%rsp),%eax,%rax)
	movl	TMP2(%rsp), %esi

	addq	$20, %rbx
	addq	$20, %rbp
	cmpq	W15(%rsp), %rbp
	jb	2b

	BODY0(0,FN1,0x5a827999,%eax,TMP1(%rsp),%ecx,%edx,%esi,%rsi)
	addq	$4, %rbx
	movq	%rbx, %r8
	movq	TMP1(%rsp), %rbx

	BODY(4,FN1,0x5a827999,%esi,%eax,%ebx,%ecx,%edx,%rdx)
	BODY(8,FN1,0x5a827999,%edx,%esi,%eax,%ebx,%ecx,%rcx)
	BODY(12,FN1,0x5a827999,%ecx,%edx,%esi,%eax,%ebx,%rbx)
	BODY(16,FN1,0x5a827999,%ebx,%ecx,%edx,%esi,%eax,%rax)

	addq	$20, %rbp

3:
	BODY(0,FN24,0x6ed9eba1,%eax,%ebx,%ecx,%edx,%esi,%rsi)
	BODY(4,FN24,0x6ed9eba1,%esi,%eax,%ebx,%ecx,%edx,%rdx)
	BODY(8,FN24,0x6ed9eba1,%edx,%esi,%eax,%ebx,%ecx,%rcx)
	BODY(12,FN24,0x6ed9eba1,%ecx,%edx,%esi,%eax,%ebx,%rbx)
	BODY(16,FN24,0x6ed9eba1,%ebx,%ecx,%edx,%esi,%eax,%rax)

	addq	$20, %rbp
	cmpq	W40(%rsp), %rbp
	jb	3b

4:
	BODY(0,FN3,0x8f1bbcdc,%eax,%ebx,%ecx,%edx,%esi,%rsi)
	BODY(4,FN3,0x8f1bbcdc,%esi,%eax,%ebx,%ecx,%edx,%rdx)
	BODY(8,FN3,0x8f1bbcdc,%edx,%esi,%eax,%ebx,%ecx,%rcx)
	BODY(12,FN3,0x8f1bbcdc,%ecx,%edx,%esi,%eax,%ebx,%rbx)
	BODY(16,FN3,0x8f1bbcdc,%ebx,%ecx,%edx,%esi,%eax,%rax)

	addq	$20, %rbp
	cmpq	W60(%rsp), %rbp
	jb	4b

5:
	BODY(0,FN24,0xca62c1d6,%eax,%ebx,%ecx,%edx,%esi,%rsi)
	BODY(4,FN24,0xca62c1d6,%esi,%eax,%ebx,%ecx,%edx,%rdx)
	BODY(8,FN24,0xca62c1d6,%edx,%esi,%eax,%ebx,%ecx,%rcx)
	BODY(12,FN24,0xca62c1d6,%ecx,%edx,%esi,%eax,%ebx,%rbx)
	BODY(16,FN24,0xca62c1d6,%ebx,%ecx,%edx,%esi,%eax,%rax)

	addq	$20, %rbp
	cmpq	W80(%rsp), %rbp
	jb	5b

	addl	%eax, 0(%r9)
	addl	%ebx, 4(%r9)
	addl	%ecx, 8(%r9)
	addl	%edx, 12(%r9)
	addl	%esi, 16(%r9)

	cmpq	%r10, %r8
	jb	1b

	addq	$384, %rsp
	popq	%rbp
	popq	%rbx
	ret
