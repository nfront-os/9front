/*
 * libmp unit tests, built with the gcc port toolchain and run inside
 * 9front (tests/guest/run.sh): arithmetic on the Ed448 field prime
 * p = 2^448-2^224-1 that dp9ik's authpak_hash (factotum) computes in,
 * with and without mpfield's special reduction. Reference values from
 * Python's integers.
 *
 * Output: one "ok name" or "FAIL name: detail" line per check, then
 * "mptest: N ok, M failed".
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

static char P[] = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
static char X[] = "1655D2CE0563F7604E438536EA8BD18717A8D41BA44B36007FC71BFEEDF8DDAA6168BCC52F6106C8206C1293F75602875A28BD71067DEC768F857B788769FE7D07580E5E29AFCFD919FB9A70357D9830E106A8C5A03CA5239747C67CF0F71DB37D9DD11";
static char Y[] = "7AF417A046C316FF6D2E48D6AD05942FC72613B8FF39DFB0EB5B1D8CFC01445C21B70DF71CFB98C8D20A0D666CA959601C6631EF99B169B08AAB2441C293EE1AB3DAD504E53D64DA04F7FF697C47D6D42BB55EE01BA3A1A889E4C3701F1CC6D4FDA897CA910BA965EB38E8792DC30240BF4B2702B5FEF0425BEFC46E31DDBFCA2ADD9BF4D6A9A12CBA95C4181";
static char XY[] = "ABA3252CCAF87709853BE105052521DB15A98F8DC04935A4080590F54DA0B9D4B79EC4CE23BF4873906E818D4FC395A7BA17D914CEF62676AE4CBEBF6F46A0C84A0D56C6D228DB38DFBD48A7F755298D8FAF7505C9660F470063BC85500FDD39057D0DAA2E3F2BED9A6C01A3A221121E3633A79C5059FE5F8D779F58C3B94514995C8BDE4E29D41ECE113C2ECC2A560ECE6006F2752DEB25959A3BE9ADA1DCB730A82527379D165AEED6F4D902256055C203D72185AE366650F30ABEE4C05A9B7D8552F663CE1C3551FE0650B7B952B907F2C694ECF79F025070F353086FC452F3D47C08769561086DF6C5C2105B691";
static char XYMODP[] = "EB276E5B3EA207EA9E60EF15EF48472D9578AE6C6ABDA09CB7261BD3D6BCAA21F8EDB57114C69E22F033DAFA720EF5076048EC125A4B09B2";
static char YDIVX[] = "58141A72C115DD982C22F49E5F339F6EE7CAD57D72438678C78AA289AB245838A331E39342C331CDD0C";
static char YMODX[] = "4DF46E249448E414FF60D8645B4C57CBC6AF83B803189ADDA7338A008E989C4282CE38A8EEA1E87B3F9A10AAB0693B7A61E22C0815E6820EABEE642E27384B1D98B9E3C93FFF833BA43411C7B5D4EBE1885FD6111A8E4E18FB4E7D2F598783667237B5";
static char INV5[] = "3333333333333333333333333333333333333333333333333333333300000000000000000000000000000000000000000000000000000000";
static char EXP[] = "23F6AB6ECA844B46A054D0E478134BBAF823A0A5FB3CC3A654C1879491BF879908B6AA81A5BA01C7A320FEE3921D5E7A42E61638C71AFCCF";
static char SUB[] = "7AF417A046C316FF6D2E48D6AD05942FC72613B8FF39DFB0EB5B1D8CFC01445C21B70DF71CFB98C8D1F3B7939EA3F568BC17EE6A62C6DDDF03937B6DA6EFA2E4B35B0DE8E64F6BFC5A9696ACB71875CD6394F2CD87AC4BA6028A9AB2AE1648E88719124F18843F676E31906ACF995270E6312B6845C972AA2B0EBDC56C3D83250746542E59B8AA0F071826470";

static int
eq(mpint *a, char *hex)
{
	char *s;
	int r;

	s = smprint("%B", a);
	r = cistrcmp(s, hex) == 0;
	if(!r)
		print("\tgot  %s\n\twant %s\n", s, hex);
	free(s);
	return r;
}

static void
testmp(void)
{
	mpint *p, *f, *x, *y, *r, *q, *five;

	p = strtomp(P, nil, 16, nil);
	x = strtomp(X, nil, 16, nil);
	y = strtomp(Y, nil, 16, nil);
	r = mpnew(0);
	q = mpnew(0);
	check("strtomp/%B round trip", eq(x, X), nil);
	mpmul(x, y, r);
	check("mpmul", eq(r, XY), nil);
	mpsub(y, x, r);
	check("mpsub", eq(r, SUB), nil);
	mpdiv(y, x, q, r);
	check("mpdiv quotient", eq(q, YDIVX), nil);
	check("mpdiv remainder", eq(r, YMODX), nil);
	mpmul(x, y, r);
	mpmod(r, p, r);
	check("mpmod plain", eq(r, XYMODP), nil);
	f = mpfield(mpcopy(p));
	check("mpfield recognises p448", f != p && f->flags & MPfield, nil);
	mpmul(x, y, r);
	mpmod(r, f, r);
	check("mpmod mpfield", eq(r, XYMODP), nil);
	five = itomp(5, nil);
	mpinvert(five, p, r);
	check("mpinvert", eq(r, INV5), nil);
	mpmod(x, p, q);
	mpexp(q, y, p, r);
	check("mpexp", eq(r, EXP), nil);
	mpexp(q, y, f, r);
	check("mpexp mpfield", eq(r, EXP), nil);
}

static void
testauthpak(void)
{
	Authkey k;
	Waitmsg *w;
	vlong t;

	memset(&k, 0, sizeof k);
	passtokey(&k, "gccport9");
	t = nsec();
	switch(fork()){
	case -1:
		check("authpak_hash", 0, "fork: %r");
		return;
	case 0:
		alarm(30*1000);
		authpak_hash(&k, "glenda");
		exits(nil);
	}
	w = wait();
	t = nsec() - t;
	check("authpak_hash (dp9ik key) returns within 30s", w != nil && w->msg[0] == 0,
		"%s after %lld ms", w ? w->msg : "no wait", t/1000000);
	print("authpak_hash: %lld ms\n", t/1000000);
	free(w);
}

void
main(void)
{
	fmtinstall('B', mpfmt);
	testmp();
	testauthpak();
	print("mptest: %d ok, %d failed\n", nok, nfail);
	exits(nfail ? "failures" : nil);
}
