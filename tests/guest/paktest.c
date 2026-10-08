/*
 * The dp9ik PAK curve code of libauthsrv (sys/src/libauthsrv/*.mpc,
 * generated from *.mp by scripts/gen-mpc.sh) step by step, each step in
 * its own process with a time limit, built with the gcc port toolchain
 * and run inside 9front (tests/guest/run.sh). authpak_hash, which
 * factotum -S runs for its nvram key, is built from these steps.
 *
 * Output: one "ok name" or "FAIL name: detail" line per check, then
 * "paktest: N ok, M failed".
 */
#include <u.h>
#include <libc.h>
#include <mp.h>
#include <libsec.h>
#include <authsrv.h>

#include "../../sys/src/libauthsrv/msqrt.mpc"
#include "../../sys/src/libauthsrv/decaf.mpc"
#include "../../sys/src/libauthsrv/edwards.mpc"
#include "../../sys/src/libauthsrv/elligator2.mpc"
#include "../../sys/src/libauthsrv/spake2ee.mpc"
#include "../../sys/src/libauthsrv/ed448.mpc"

static int nok, nfail;

static void
check(char *name, int ok, char *fmt, ...)
{
	char buf[256];
	va_list arg;

	if(ok){
		nok++;
		print("ok %s\n", name);
		return;
	}
	nfail++;
	buf[0] = 0;
	if(fmt != nil){
		va_start(arg, fmt);
		vsnprint(buf, sizeof buf, fmt, arg);
		va_end(arg);
	}
	print("FAIL %s: %s\n", name, buf);
}

static mpint *P, *A, *D, *X, *Y;

/* run f in a child with a 10s limit; the child prints its own detail */
static void
step(char *name, void (*f)(void))
{
	Waitmsg *w;
	vlong t;

	t = nsec();
	switch(fork()){
	case -1:
		check(name, 0, "fork: %r");
		return;
	case 0:
		alarm(10*1000);
		f();
		exits(nil);
	}
	w = wait();
	t = nsec() - t;
	check(name, w != nil && w->msg[0] == 0, "%s after %lld ms", w ? w->msg : "no wait", t/1000000);
	free(w);
}

static void
legendre(mpint *p)
{
	mpint *n, *r;
	int i;

	n = mpnew(0);
	r = mpnew(0);
	for(i = 2; i < 8; i++){
		uitomp(i, n);
		legendresymbol(n, p, r);
		print("\tlegendre(%d) = %B\n", i, r);
		if(mpcmp(r, mpone) != 0 && !(r->sign < 0 && mpmagcmp(r, mpone) == 0))
			exits("legendre symbol not +1 or -1");
	}
}

static void
legendreplain(void)
{
	legendre(P);
}

static void
legendrefield(void)
{
	legendre(mpfield(mpcopy(P)));
}

static void
sqrtfield(void)
{
	mpint *p, *a, *r, *rr;

	p = mpfield(mpcopy(P));
	a = itomp(4, nil);
	r = mpnew(0);
	rr = mpnew(0);
	msqrt(a, p, r);
	mpmodmul(r, r, p, rr);
	print("\tmsqrt(4)^2 = %B\n", rr);
	if(mpcmp(rr, a) != 0)
		exits("msqrt(4)^2 != 4");
}

static void
h2p(void)
{
	mpint *p, *h, *PX, *PY, *PZ, *PT;

	p = mpfield(mpcopy(P));
	h = strtomp("123456789ABCDEF0123456789ABCDEF", nil, 16, nil);
	PX = mpnew(0); PY = mpnew(0); PZ = mpnew(0); PT = mpnew(0);
	spake2ee_h2P(p, A, D, h, PX, PY, PZ, PT);
	print("\th2P X = %B\n", PX);
}

void
main(void)
{
	fmtinstall('B', mpfmt);
	P = mpnew(0); A = mpnew(0); D = mpnew(0); X = mpnew(0); Y = mpnew(0);
	ed448_curve(P, A, D, X, Y);
	check("ed448_curve p = 2^448-2^224-1", cistrcmp(smprint("%B", P),
		"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF") == 0,
		"%B", P);
	step("legendresymbol, plain p", legendreplain);
	step("legendresymbol, mpfield p", legendrefield);
	step("msqrt, mpfield p", sqrtfield);
	step("spake2ee_h2P", h2p);
	print("paktest: %d ok, %d failed\n", nok, nfail);
	exits(nfail ? "failures" : nil);
}
