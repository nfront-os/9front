/*
 * Host unit tests for the GAS ports of 9front's libsec machine assembly
 * (sys/src/libsec/$objtype/*.S, symbols prefixed p9_, see run.sh)
 * against the portable C 9front uses without them (sys/src/libsec/port,
 * prefixed c_): the MD5 and SHA-1 block functions on random data of 1
 * to 20 blocks, ChaCha's block function at 8, 12 and 20 rounds, and
 * (amd64, when the CPU has AES-NI) key setup, encryption and
 * decryption for 128, 192 and 256-bit keys; also the callee-saved
 * registers and the direction flag after every call (callcheck*.s).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint32_t u32;
typedef unsigned char uchar;

void p9__md5block(uchar*, u32, u32*), c__md5block(uchar*, u32, u32*);
void p9__sha1block(uchar*, u32, u32*), c__sha1block(uchar*, u32, u32*);
#ifdef __x86_64__
void p9__chachablock(u32*, int), c__chachablock(u32*, int);
void *p9_aesni_init(void);
void (*p9_aes_encrypt)(u32*, int, uchar*, uchar*);
void (*p9_aes_decrypt)(u32*, int, uchar*, uchar*);
/* the C reference through its public interface (libsec.h's AESstate,
 * ulong being 32 bits): setupAESstate takes the C path, c_aesni_init
 * finding no AES-NI */
typedef struct {
	u32	setup, offset;
	int	rounds, keybytes;
	void	*ekey, *dkey;
	uchar	key[32], ivec[16], storage[512];
} AESstate;
void c_setupAESstate(AESstate*, uchar*, int, uchar*);
extern void (*c_aes_encrypt)(u32*, int, uchar*, uchar*);
#endif
long callcheck(void *fn, long a, long b, long c, long d, ...);

/* what the C references need from libc */
void *c_memmove(void *a, void *b, long n) { return memmove(a, b, n); }
void *c_memset(void *a, int c, long n) { return memset(a, c, n); }
void *c_aesni_init(void) { return 0; }

static int nfail, ntest;

#define T(name, cond) do{ ntest++; if(!(cond)){ nfail++; if(nfail < 20) printf("FAIL %s (%s:%d)\n", name, __FILE__, __LINE__); } }while(0)

static void
rbytes(void *v, int n)
{
	uchar *p = v;

	while(n-- > 0)
		*p++ = rand();
}

static void
hashes(void)
{
	static uchar data[20*64];
	u32 s1[5], s2[5];
	int n, k;

	for(k = 0; k < 200; k++)
	for(n = 1; n <= 20; n++){
		rbytes(data, n*64);
		rbytes(s1, sizeof s1);
		memcpy(s2, s1, sizeof s1);
		p9__md5block(data, n*64, s1);
		c__md5block(data, n*64, s2);
		T("_md5block", memcmp(s1, s2, 4*4) == 0);
		rbytes(s1, sizeof s1);
		memcpy(s2, s1, sizeof s1);
		p9__sha1block(data, n*64, s1);
		c__sha1block(data, n*64, s2);
		T("_sha1block", memcmp(s1, s2, 5*4) == 0);
	}
}

#ifdef __x86_64__
static void
chacha(void)
{
	u32 x1[16], x2[16];
	static int rounds[] = {8, 12, 20};
	int k, r;

	for(k = 0; k < 10000; k++)
	for(r = 0; r < 3; r++){
		rbytes(x1, sizeof x1);
		memcpy(x2, x1, sizeof x1);
		p9__chachablock(x1, rounds[r]);
		c__chachablock(x2, rounds[r]);
		T("_chachablock", memcmp(x1, x2, sizeof x1) == 0);
	}
}

static void
aes(void)
{
	int (*setup)(u32*, u32*, uchar*, int);
	static u32 e1[64] __attribute__((aligned(16))), d1[64] __attribute__((aligned(16)));
	static AESstate s2;
	uchar key[32], pt[16], c1[16], c2[16], p1[16];
	static int nkeys[] = {16, 24, 32};
	int k, i, j, nr1;

	setup = p9_aesni_init();
	if(setup == 0){
		printf("no AES-NI on this CPU: aesni not tested\n");
		return;
	}
	T("aesni_init: aes_encrypt set", p9_aes_encrypt != 0 && p9_aes_decrypt != 0);
	T("aesni_init: bad key length", setup(e1, d1, key, 20) == 0);
	for(k = 0; k < 500; k++)
	for(i = 0; i < 3; i++){
		rbytes(key, sizeof key);
		nr1 = setup(e1, d1, key, nkeys[i]);
		c_setupAESstate(&s2, key, nkeys[i], 0);
		T("AESsetup rounds", nr1 == s2.rounds);
		for(j = 0; j < 20; j++){
			rbytes(pt, sizeof pt);
			p9_aes_encrypt(e1, nr1, pt, c1);
			c_aes_encrypt(s2.ekey, s2.rounds, pt, c2);
			T("aes_encrypt", memcmp(c1, c2, 16) == 0);
			p9_aes_decrypt(d1, nr1, c1, p1);
			T("aes_decrypt", memcmp(p1, pt, 16) == 0);
		}
	}
	T("callee-saved/DF aesni_init", callcheck(p9_aesni_init, 0, 0, 0, 0) == 0);
	T("callee-saved/DF aes setup", callcheck(setup, (long)e1, (long)d1, (long)key, 32) == 0);
	T("callee-saved/DF aes_encrypt", callcheck(p9_aes_encrypt, (long)e1, 14, (long)pt, (long)c1) == 0);
	T("callee-saved/DF aes_decrypt", callcheck(p9_aes_decrypt, (long)d1, 14, (long)c1, (long)p1) == 0);
}
#endif

static void
regs(void)
{
	static uchar data[4*64];
	u32 s[16];

	T("callee-saved/DF _md5block", callcheck(p9__md5block, (long)data, sizeof data, (long)s, 0) == 0);
	T("callee-saved/DF _sha1block", callcheck(p9__sha1block, (long)data, sizeof data, (long)s, 0) == 0);
#ifdef __x86_64__
	T("callee-saved/DF _chachablock", callcheck(p9__chachablock, (long)s, 20, 0, 0) == 0);
#endif
}

int
main(void)
{
	srand(1);
	hashes();
#ifdef __x86_64__
	chacha();
	aes();
#endif
	regs();
	printf("%d tests, %d failed\n", ntest, nfail);
	return nfail != 0;
}
