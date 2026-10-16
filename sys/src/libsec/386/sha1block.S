/*
 * GCC port: GAS translation. The arguments are where 8c left them
 * (cdecl); the original used BX, SI, DI, BP freely, which cdecl has
 * the callee save: they are pushed, then the original's $352 frame,
 * so data, len, state are at 372, 376, 380(%esp) and the locals at
 * their offsets from the frame's top (8a's pseudo-SP) plus 352.
 */

/* x = (wp[off-f] ^ wp[off-8] ^ wp[off-14] ^ wp[off-16]) <<< 1;
 * wp[off] = x;
 * x += A <<< 5;
 * E += 0xca62c1d6 + x;
 * x = FN(B,C,D);
 * E += x;
 * B >>> 2
 */
#define BODY(off,FN,V,A,B,C,D,E)\
	movl (off-64)(%ebp),%edi;\
	xorl (off-56)(%ebp),%edi;\
	xorl (off-32)(%ebp),%edi;\
	xorl (off-12)(%ebp),%edi;\
	roll $1,%edi;\
	movl %edi,off(%ebp);\
	leal V(%edi,E,1),E;\
	movl A,%edi;\
	roll $5,%edi;\
	addl %edi,E;\
	FN(B,C,D)\
	addl %edi,E;\
	rorl $2,B;\

#define BODY0(off,FN,V,A,B,C,D,E)\
	movl off(%ebx),%edi;\
	bswap %edi;\
	movl %edi,off(%ebp);\
	leal V(%edi,E,1),E;\
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
 * stack offsets
 * void sha1block(uchar *DATA, int LEN, ulong *STATE)
 */
#define	DATA	372
#define	LEN	376
#define	STATE	380

/*
 * locals, from %esp (the original's offsets from its frame's top + 352)
 * ulong w[80];
 * uchar *edata;
 * ulong *w15, *w40, *w60, *w80;
 * register local
 * ulong *wp = BP
 * ulong a = eax, b = ebx, c = ecx, d = edx, e = esi
 * ulong tmp = edi
 */
#define WARRAY	(352-4-(80*4))
#define TMP1	(352-8-(80*4))
#define TMP2	(352-12-(80*4))
#define W15	(352-16-(80*4))
#define W40	(352-20-(80*4))
#define W60	(352-24-(80*4))
#define W80	(352-28-(80*4))
#define EDATA	(352-32-(80*4))

.globl _sha1block
_sha1block:
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	pushl	%ebp
	subl	$352, %esp

	movl	DATA(%esp), %eax
	addl	LEN(%esp), %eax
	movl	%eax, EDATA(%esp)

	leal	(WARRAY+15*4)(%esp), %edi
	movl	%edi, W15(%esp)
	leal	(WARRAY+40*4)(%esp), %edx
	movl	%edx, W40(%esp)
	leal	(WARRAY+60*4)(%esp), %ecx
	movl	%ecx, W60(%esp)
	leal	(WARRAY+80*4)(%esp), %edi
	movl	%edi, W80(%esp)

1:
	leal	WARRAY(%esp), %ebp

	movl	STATE(%esp), %edi
	movl	(%edi), %eax
	movl	4(%edi), %ebx
	movl	%ebx, TMP1(%esp)
	movl	8(%edi), %ecx
	movl	12(%edi), %edx
	movl	16(%edi), %esi

	movl	DATA(%esp), %ebx

2:
	BODY0(0,FN1,0x5a827999,%eax,TMP1(%esp),%ecx,%edx,%esi)
	movl	%esi, TMP2(%esp)
	BODY0(4,FN1,0x5a827999,%esi,%eax,TMP1(%esp),%ecx,%edx)
	movl	TMP1(%esp), %esi
	BODY0(8,FN1,0x5a827999,%edx,TMP2(%esp),%eax,%esi,%ecx)
	BODY0(12,FN1,0x5a827999,%ecx,%edx,TMP2(%esp),%eax,%esi)
	movl	%esi, TMP1(%esp)
	BODY0(16,FN1,0x5a827999,%esi,%ecx,%edx,TMP2(%esp),%eax)
	movl	TMP2(%esp), %esi

	addl	$20, %ebx
	addl	$20, %ebp
	cmpl	W15(%esp), %ebp
	jb	2b

	BODY0(0,FN1,0x5a827999,%eax,TMP1(%esp),%ecx,%edx,%esi)
	addl	$4, %ebx
	movl	%ebx, DATA(%esp)
	movl	TMP1(%esp), %ebx

	BODY(4,FN1,0x5a827999,%esi,%eax,%ebx,%ecx,%edx)
	BODY(8,FN1,0x5a827999,%edx,%esi,%eax,%ebx,%ecx)
	BODY(12,FN1,0x5a827999,%ecx,%edx,%esi,%eax,%ebx)
	BODY(16,FN1,0x5a827999,%ebx,%ecx,%edx,%esi,%eax)

	addl	$20, %ebp

3:
	BODY(0,FN24,0x6ed9eba1,%eax,%ebx,%ecx,%edx,%esi)
	BODY(4,FN24,0x6ed9eba1,%esi,%eax,%ebx,%ecx,%edx)
	BODY(8,FN24,0x6ed9eba1,%edx,%esi,%eax,%ebx,%ecx)
	BODY(12,FN24,0x6ed9eba1,%ecx,%edx,%esi,%eax,%ebx)
	BODY(16,FN24,0x6ed9eba1,%ebx,%ecx,%edx,%esi,%eax)

	addl	$20, %ebp
	cmpl	W40(%esp), %ebp
	jb	3b

4:
	BODY(0,FN3,0x8f1bbcdc,%eax,%ebx,%ecx,%edx,%esi)
	BODY(4,FN3,0x8f1bbcdc,%esi,%eax,%ebx,%ecx,%edx)
	BODY(8,FN3,0x8f1bbcdc,%edx,%esi,%eax,%ebx,%ecx)
	BODY(12,FN3,0x8f1bbcdc,%ecx,%edx,%esi,%eax,%ebx)
	BODY(16,FN3,0x8f1bbcdc,%ebx,%ecx,%edx,%esi,%eax)

	addl	$20, %ebp
	cmpl	W60(%esp), %ebp
	jb	4b

5:
	BODY(0,FN24,0xca62c1d6,%eax,%ebx,%ecx,%edx,%esi)
	BODY(4,FN24,0xca62c1d6,%esi,%eax,%ebx,%ecx,%edx)
	BODY(8,FN24,0xca62c1d6,%edx,%esi,%eax,%ebx,%ecx)
	BODY(12,FN24,0xca62c1d6,%ecx,%edx,%esi,%eax,%ebx)
	BODY(16,FN24,0xca62c1d6,%ebx,%ecx,%edx,%esi,%eax)

	addl	$20, %ebp
	cmpl	W80(%esp), %ebp
	jb	5b

	movl	STATE(%esp), %edi
	addl	%eax, 0(%edi)
	addl	%ebx, 4(%edi)
	addl	%ecx, 8(%edi)
	addl	%edx, 12(%edi)
	addl	%esi, 16(%edi)

	movl	EDATA(%esp), %edi
	cmpl	%edi, DATA(%esp)
	jb	1b

	addl	$352, %esp
	popl	%ebp
	popl	%edi
	popl	%esi
	popl	%ebx
	ret
