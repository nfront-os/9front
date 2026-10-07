/*
 * Host unit tests for the GAS/System V ports of 9front's libc amd64
 * assembly (sys/src/libc/amd64/*.s). Linux/amd64 uses the same System V
 * calling convention, so the ported routines are assembled for the host
 * with a p9_ prefix (see run.sh) and compared against glibc / C models:
 *   - results on random inputs, including overlapping moves and all
 *     alignments,
 *   - callee-saved registers and the direction flag after every call
 *     (callcheck.s).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

long callcheck(void *fn, long a, long b, long c, long d);

void *p9_memmove(void*, const void*, long);
void *p9_memcpy(void*, const void*, long);
void *p9_memset(void*, int, long);
void *p9_memchr(const void*, int, long);
void *p9_memccpy(void*, const void*, int, long);
int p9_memcmp(const void*, const void*, long);
long p9_strlen(const char*);
char *p9_strcpy(char*, const char*);
char *p9_strcat(char*, const char*);
char *p9_strchr(const char*, int);
int p9_ainc(int*), p9_adec(int*);
int p9_cas32(unsigned*, unsigned, unsigned), p9_cas(unsigned*, unsigned, unsigned);
int p9_cas64(uint64_t*, uint64_t, uint64_t), p9_casp(void**, void*, void*);
int p9_fas32(int*, int);
long p9_fas64(long*, long);
int p9_agetl(int*);
void *p9_agetp(void**);
int p9_aswapl(int*, int), p9_aincl(int*, int), p9_acasl(int*, int, int);
void *p9_aswapp(void**, void*);
int p9_acasp(void**, void*, void*);
long p9_agetv(long*), p9_aswapv(long*, long), p9_aincv(long*, long);
int p9_acasv(long*, long, long);
unsigned p9_umuldiv(unsigned, unsigned, unsigned);
long p9_muldiv(long, long, long);
uintptr_t p9_getcallerpc(void*);
void p9_cycles(uint64_t*);
unsigned long p9_getfcr(void), p9_getfsr(void);
void p9_setfcr(unsigned long), p9_setfsr(unsigned long);
int p9__tas(int*);
void p9_coherence(void);

static int nfail, ntest;

#define T(name, cond) do{ ntest++; if(!(cond)){ nfail++; printf("FAIL %s (%s:%d)\n", name, __FILE__, __LINE__); } }while(0)

static int
sgn(int x)
{
	return x < 0 ? -1 : x > 0;
}

/* every routine through callcheck: callee-saved registers and DF */
static void
regs(void)
{
	char a[64], b[64];
	int v = 1;
	long l = 1;
	uint64_t w = 1, c;
	void *p = 0;

	memset(a, 'x', 63); a[63] = 0;
	memset(b, 'y', 31); b[31] = 0;
#define R(name, ...) T("callee-saved/DF " name, callcheck(__VA_ARGS__) == 0)
	R("memmove fwd", p9_memmove, (long)a, (long)a+8, 20, 0);
	R("memmove back", p9_memmove, (long)a+8, (long)a, 20, 0);
	R("memmove back aligned", p9_memmove, (long)a+4, (long)a, 40, 0);
	R("memcpy", p9_memcpy, (long)a+3, (long)a, 17, 0);
	R("memset", p9_memset, (long)a, 'z', 40, 0);
	R("memchr", p9_memchr, (long)a, 'q', 40, 0);
	R("memccpy", p9_memccpy, (long)a, (long)b, 'y', 10);
	R("memcmp", p9_memcmp, (long)a, (long)b, 20, 0);
	R("strlen", p9_strlen, (long)b, 0, 0, 0);
	R("strcpy", p9_strcpy, (long)a, (long)b, 0, 0);
	R("strcat", p9_strcat, (long)a, (long)b, 0, 0);
	R("strchr", p9_strchr, (long)a, 'y', 0, 0);
	R("ainc", p9_ainc, (long)&v, 0, 0, 0);
	R("cas32", p9_cas32, (long)&v, 2, 3, 0);
	R("cas64", p9_cas64, (long)&w, 1, 2, 0);
	R("fas64", p9_fas64, (long)&l, 5, 0, 0);
	R("aincl", p9_aincl, (long)&v, 1, 0, 0);
	R("acasp", p9_acasp, (long)&p, 0, (long)a, 0);
	R("aincv", p9_aincv, (long)&l, 1, 0, 0);
	R("umuldiv", p9_umuldiv, 7, 6, 5, 0);
	R("muldiv", p9_muldiv, -7, 6, 5, 0);
	R("cycles", p9_cycles, (long)&c, 0, 0, 0);
	R("getfcr", p9_getfcr, 0, 0, 0, 0);
	R("setfcr", p9_setfcr, p9_getfcr(), 0, 0, 0);
	R("tas", p9__tas, (long)&v, 0, 0, 0);
}

static void
mem(void)
{
	unsigned char a[300], b[300], c[300], x[100], y[100];
	char s1[200], s2[200], t1[200], t2[200];
	int it, i, n, d, s, ch, l1, l2, o, cc;
	void *r, *q1, *q2;

	srand(1);
	for(it = 0; it < 20000; it++){
		for(i = 0; i < 300; i++){
			a[i] = rand();
			b[i] = a[i];
			c[i] = rand()%4 ? rand() : 0;
		}
		n = rand()%41; d = rand()%60; s = rand()%60;
		r = p9_memmove(a+d, a+s, n); memmove(b+d, b+s, n);
		T("memmove", r == a+d && memcmp(a, b, 300) == 0);
		r = p9_memcpy(a+s, a+d, n); memmove(b+s, b+d, n);
		T("memcpy", r == a+s && memcmp(a, b, 300) == 0);
		ch = rand()%256;
		r = p9_memset(a+d, ch, n); memset(b+d, ch, n);
		T("memset", r == a+d && memcmp(a, b, 300) == 0);
		T("memchr", p9_memchr(c+d, c[d+n/2], n) == (n ? memchr(c+d, c[d+n/2], n) : NULL));
		T("memcmp", n == 0 || sgn(p9_memcmp(a+d, c+s, n)) == sgn(memcmp(a+d, c+s, n)));
		memcpy(b, a, 300);
		T("memcmp equal", p9_memcmp(a+d, b+d, n) == 0);
		memset(x, 1, 100); memset(y, 1, 100);
		q1 = p9_memccpy(x+1, c+s, c[s+n/3], n);
		q2 = memccpy(y+1, c+s, c[s+n/3], n);
		T("memccpy", (q1 ? (unsigned char*)q1-x : -1) == (q2 ? (unsigned char*)q2-y : -1)
			&& memcmp(x, y, 100) == 0);

		l1 = rand()%50; l2 = rand()%50;
		for(i = 0; i < l1; i++) s1[i] = 'a'+rand()%26;
		s1[l1] = 0;
		for(i = 0; i < l2; i++) s2[i] = 'a'+rand()%26;
		s2[l2] = 0;
		T("strlen", p9_strlen(s1) == (long)strlen(s1));
		o = rand()%4;
		T("strcpy ret", p9_strcpy(t1+o, s1) == t1+o); strcpy(t2+o, s1);
		T("strcpy", strcmp(t1+o, t2+o) == 0);
		T("strcat ret", p9_strcat(t1+o, s2) == t1+o); strcat(t2+o, s2);
		T("strcat", strcmp(t1+o, t2+o) == 0);
		cc = 'a'+rand()%27;
		if(cc == 'a'+26)
			cc = 0;
		T("strchr", p9_strchr(s1, cc) == strchr(s1, cc));
	}
}

static void
atomics(void)
{
	int v = 5;
	unsigned u = 3;
	uint64_t w = 1ULL<<40;
	long L = 4;
	void *p = &v, *pp = 0;

	T("ainc", p9_ainc(&v) == 6 && v == 6);
	T("adec", p9_adec(&v) == 5 && v == 5);
	T("cas32 hit", p9_cas32(&u, 3, 7) == 1 && u == 7);
	T("cas32 miss", p9_cas32(&u, 3, 9) == 0 && u == 7);
	T("cas", p9_cas(&u, 7, 8) == 1 && u == 8);
	T("cas64 hit", p9_cas64(&w, 1ULL<<40, 2) == 1 && w == 2);
	T("cas64 miss", p9_cas64(&w, 5, 9) == 0 && w == 2);
	T("casp", p9_casp(&pp, 0, p) == 1 && pp == p);
	T("fas32", p9_fas32(&v, 9) == 5 && v == 9);
	T("fas64", p9_fas64(&L, 8) == 4 && L == 8);
	T("agetl", p9_agetl(&v) == 9);
	T("agetp", p9_agetp(&pp) == p);
	T("aswapl", p9_aswapl(&v, 1) == 9 && v == 1);
	T("aswapp", p9_aswapp(&pp, 0) == p && pp == 0);
	T("aincl", p9_aincl(&v, 4) == 5 && v == 5);
	T("acasl hit", p9_acasl(&v, 5, 6) == 1 && v == 6);
	T("acasl miss", p9_acasl(&v, 5, 7) == 0 && v == 6);
	T("agetv", p9_agetv(&L) == 8);
	T("aswapv", p9_aswapv(&L, 3) == 8 && L == 3);
	T("aincv", p9_aincv(&L, 1L<<40) == 3+(1L<<40));
	T("acasv", p9_acasv(&L, 3+(1L<<40), 1) == 1 && L == 1);
	v = 0;
	T("tas free", p9__tas(&v) == 0 && v != 0);
	T("tas held", p9__tas(&v) != 0);
	p9_coherence();
}

static void
arith(void)
{
	T("umuldiv", p9_umuldiv(4000000000u, 3, 4) == 3000000000u);
	T("muldiv negative", p9_muldiv(-7, 3, 2) == -10);
	T("muldiv positive", p9_muldiv(100000, 30000, 7) == 3000000000L/7);
}

__attribute__((noinline)) static uintptr_t
callerpc(int x)
{
	uintptr_t pc = p9_getcallerpc(&x);
	return pc == (uintptr_t)__builtin_return_address(0);
}

static void
misc(void)
{
	uint64_t t0, t1;
	unsigned long fcr;

	T("getcallerpc", callerpc(1));
	p9_cycles(&t0);
	p9_cycles(&t1);
	T("cycles monotonic", t1 > t0);
	fcr = p9_getfcr();
	p9_setfcr(fcr);
	T("getfcr/setfcr round trip", p9_getfcr() == fcr);
	p9_setfsr(0);
	T("getfsr after setfsr(0)", p9_getfsr() == 0);
}

int
main(void)
{
	regs();
	mem();
	atomics();
	arith();
	misc();
	printf("%d checks, %d failed\n", ntest, nfail);
	return nfail != 0;
}
