/*
 * Host unit tests for the GAS ports of 9front's libmp machine assembly
 * (sys/src/libmp/$objtype/*.S): each routine (p9_, see run.sh) against
 * the portable C version 9front uses without it (sys/src/libmp/port,
 * built here with the c_ prefix) on random vectors of every length up
 * to 40 digits, with the edge cases of the carry and of the division
 * (overflow, divisor 0, all ones); on amd64 also the callee-saved
 * registers and the direction flag after every call (callcheck.s).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint32_t mpdigit;

void p9_mpdigdiv(mpdigit*, mpdigit, mpdigit*), c_mpdigdiv(mpdigit*, mpdigit, mpdigit*);
void p9_mpvecadd(mpdigit*, int, mpdigit*, int, mpdigit*), c_mpvecadd(mpdigit*, int, mpdigit*, int, mpdigit*);
void p9_mpvecsub(mpdigit*, int, mpdigit*, int, mpdigit*), c_mpvecsub(mpdigit*, int, mpdigit*, int, mpdigit*);
void p9_mpvecdigmuladd(mpdigit*, int, mpdigit, mpdigit*), c_mpvecdigmuladd(mpdigit*, int, mpdigit, mpdigit*);
int p9_mpvecdigmulsub(mpdigit*, int, mpdigit, mpdigit*), c_mpvecdigmulsub(mpdigit*, int, mpdigit, mpdigit*);
#if defined(__x86_64__) || defined(__i386__)
long callcheck(void *fn, long a, long b, long c, long d, ...);
#endif

enum { N = 40 };

static int nfail, ntest;

#define T(name, cond) do{ ntest++; if(!(cond)){ nfail++; if(nfail < 20) printf("FAIL %s (%s:%d)\n", name, __FILE__, __LINE__); } }while(0)

/* random digits, often all zeros or all ones (the carry's edge cases) */
static mpdigit
rdig(void)
{
	switch(rand() % 8){
	case 0:
		return 0;
	case 1:
		return ~(mpdigit)0;
	case 2:
		return 1;
	}
	return (mpdigit)rand() << 16 ^ (mpdigit)rand();
}

static void
rvec(mpdigit *v, int n)
{
	int i;

	for(i = 0; i < n; i++)
		v[i] = rdig();
}

static void
vecs(void)
{
	mpdigit a[N], b[N], s1[N+2], s2[N+2];
	int alen, blen, k;

	for(k = 0; k < 20; k++)
	for(alen = 0; alen < N; alen++)
	for(blen = 0; blen <= alen; blen++){
		rvec(a, alen);
		rvec(b, blen);
		memset(s1, 0x55, sizeof s1);
		memset(s2, 0x55, sizeof s2);
		p9_mpvecadd(a, alen, b, blen, s1);
		c_mpvecadd(a, alen, b, blen, s2);
		T("mpvecadd", memcmp(s1, s2, sizeof s1) == 0);
		memset(s1, 0x55, sizeof s1);
		memset(s2, 0x55, sizeof s2);
		p9_mpvecsub(a, alen, b, blen, s1);
		c_mpvecsub(a, alen, b, blen, s2);
		T("mpvecsub", memcmp(s1, s2, sizeof s1) == 0);
	}
}

static void
digmul(void)
{
	mpdigit b[N], p1[N+2], p2[N+2], m;
	int n, k, r1, r2;

	for(k = 0; k < 200; k++)
	for(n = 1; n < N; n++){
		rvec(b, n);
		rvec(p1, n);
		p1[n] = 0;	/* the callers' p[n]: the assembly adds to it, the C sets it */
		p1[n+1] = 0x55555555;
		memcpy(p2, p1, sizeof p1);
		m = rdig();
		p9_mpvecdigmuladd(b, n, m, p1);
		c_mpvecdigmuladd(b, n, m, p2);
		T("mpvecdigmuladd", memcmp(p1, p2, sizeof p1) == 0);
		rvec(p1, n+1);
		memcpy(p2, p1, sizeof p1);
		r1 = p9_mpvecdigmulsub(b, n, m, p1);
		r2 = c_mpvecdigmulsub(b, n, m, p2);
		T("mpvecdigmulsub", memcmp(p1, p2, sizeof p1) == 0 && r1 == r2);
	}
}

static void
digdiv(void)
{
	mpdigit d[2], q1, q2, v;
	int k;

	for(k = 0; k < 2000000; k++){
		d[0] = rdig();
		d[1] = rdig();
		v = rdig();
		if(k % 4 == 0 && v != 0)
			d[1] = (mpdigit)rand() % v;	/* no overflow */
		q1 = q2 = 0x55555555;
		p9_mpdigdiv(d, v, &q1);
		c_mpdigdiv(d, v, &q2);
		T("mpdigdiv", q1 == q2);
	}
}

static void
regs(void)
{
#if defined(__x86_64__) || defined(__i386__)
	mpdigit a[N], b[N], s[N+2], d[2] = {7, 1}, q;

	rvec(a, N);
	rvec(b, N);
	T("callee-saved/DF mpvecadd", callcheck(p9_mpvecadd, (long)a, N, (long)b, N/2, (long)s) == 0);
	T("callee-saved/DF mpvecsub", callcheck(p9_mpvecsub, (long)a, N, (long)b, N/2, (long)s) == 0);
	T("callee-saved/DF mpvecdigmuladd", callcheck(p9_mpvecdigmuladd, (long)b, N, 12345, (long)s) == 0);
	T("callee-saved/DF mpvecdigmulsub", callcheck(p9_mpvecdigmulsub, (long)b, N, 12345, (long)s) == 0);
	T("callee-saved/DF mpdigdiv", callcheck(p9_mpdigdiv, (long)d, 3, (long)&q, 0) == 0);
#endif
}

int
main(void)
{
	srand(1);
	vecs();
	digmul();
	digdiv();
	regs();
	printf("%d tests, %d failed\n", ntest, nfail);
	return nfail != 0;
}
