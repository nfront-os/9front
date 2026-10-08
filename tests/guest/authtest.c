/*
 * libsec/libauthsrv unit tests, built with the gcc port toolchain and
 * run inside 9front (tests/guest/run.sh). Reference values come from
 * Python's hashlib (RFC 3174 / RFC 6070 vectors and the Plan 9 key
 * derivation of factotum's nvram password, passtoaeskey).
 *
 * Output: one "ok name" or "FAIL name: detail" line per check, then
 * "authtest: N ok, M failed".
 */
#include <u.h>
#include <libc.h>
#include <mp.h>
#include <libsec.h>
#include <authsrv.h>

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

static char*
hex(uchar *p, int n)
{
	static char buf[2*64+1];

	snprint(buf, sizeof buf, "%.*H", n, p);
	return buf;
}

static void
testsha1(void)
{
	uchar d[SHA1dlen];
	char *k, *m;

	sha1((uchar*)"abc", 3, d, nil);
	check("sha1 abc", cistrcmp(hex(d, SHA1dlen), "a9993e364706816aba3e25717850c26c9cd0d89d") == 0, "%s", hex(d, SHA1dlen));
	k = "key";
	m = "The quick brown fox jumps over the lazy dog";
	hmac_sha1((uchar*)m, strlen(m), (uchar*)k, strlen(k), d, nil);
	check("hmac_sha1", cistrcmp(hex(d, SHA1dlen), "de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9") == 0, "%s", hex(d, SHA1dlen));
}

static void
testpbkdf2(void)
{
	uchar d[20];

	pbkdf2_x((uchar*)"password", 8, (uchar*)"salt", 4, 2, d, sizeof d, hmac_sha1, SHA1dlen);
	check("pbkdf2 hmac_sha1 c=2", cistrcmp(hex(d, sizeof d), "ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957") == 0, "%s", hex(d, sizeof d));
}

static void
testpasstokey(void)
{
	Authkey k;
	vlong t;

	t = nsec();
	passtokey(&k, "gccport9");
	t = nsec() - t;
	check("passtokey aes (factotum nvram)", cistrcmp(hex(k.aes, AESKEYLEN), "be5ebf83afa4b11025eb42a509399d4c") == 0, "%s", hex(k.aes, AESKEYLEN));
	check("passtokey time < 10s", t < 10*1000000000LL, "%lld ms", t/1000000);
	print("passtokey: %lld ms\n", t/1000000);
}

void
main(void)
{
	fmtinstall('H', encodefmt);
	testsha1();
	testpbkdf2();
	testpasstokey();
	print("authtest: %d ok, %d failed\n", nok, nfail);
	exits(nfail ? "failures" : nil);
}
