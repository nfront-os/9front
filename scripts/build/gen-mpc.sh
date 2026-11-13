#!/bin/bash
# Generate the C sources 9front's mkfiles build from .mp files with the
# host tool mpc (sys/src/cmd/mpc.y):
#   libsec/port/%.c     <- %.mp  ('#include "os.h"', '#include <mp.h>', mpc output)
#   libauthsrv/%.mpc    <- %.mp  (mpc output)
# mpc runs on the build host, so it is built with plan9port's yacc/9c/9l.
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
. "$ROOT/scripts/lib/paths.sh"	# PLAN9PORT (the distribution's paths.env)
P9P=$PLAN9PORT
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

cd "$TMP"
# mpc reads its constants with strtomp(s, nil, 0, nil). 9front's
# strtomp takes base 0 as "by prefix" (0x hex, 0b binary, 0 octal, else
# decimal, libmp/port/strtomp.c); plan9port's has no base 0 and read
# them as hex (ed448's d = -39081 became -0x39081). The shim does what
# 9front's strtomp does for base 0 and leaves the rest to plan9port's.
cat > strtomp0.c <<'SHIM'
#include <u.h>
#include <libc.h>
#include <mp.h>

mpint*
strtomp0(char *a, char **pp, int base, mpint *b)
{
	char *s, *t, *c, *e;
	mpint *r, *d;

	if(base != 0)
		return strtomp(a, pp, base, b);
	for(s = a; *s == ' ' || *s == '\t' || *s == '-'; s++)
		;
	base = 10;
	t = s;
	if(s[0] == '0'){
		if(s[1] == 'x' || s[1] == 'X'){
			base = 16;
			t = s+2;
		} else if(s[1] == 'b' || s[1] == 'B'){
			base = 2;
			t = s+2;
		} else if(s[1] >= '0' && s[1] <= '7'){
			base = 8;
			t = s+1;
		}
	}
	if(base == 10 || base == 16){
		/* the signs, then the digits without their prefix */
		c = smprint("%.*s%s", (int)(s-a), a, t);
		r = strtomp(c, &e, base, b);
		if(pp != nil)
			*pp = t + (e - c - (s-a));
		free(c);
		return r;
	}
	/* binary, octal: plan9port's strtomp knows neither */
	if(b == nil)
		b = mpnew(0);
	mpassign(mpzero, b);
	d = mpnew(0);
	for(; *t >= '0' && *t < '0'+base; t++){
		uitomp(base, d);
		mpmul(b, d, b);
		uitomp(*t - '0', d);
		mpadd(b, d, b);
	}
	mpfree(d);
	if(pp != nil)
		*pp = t;
	for(; a < s; a++)
		if(*a == '-')
			b->sign = -b->sign;
	return b;
}
SHIM
PLAN9=$P9P "$P9P/bin/yacc" "$ROOT/sys/src/cmd/mpc.y"
PLAN9=$P9P "$P9P/bin/9c" -Dstrtomp=strtomp0 -o mpc.o y.tab.c
PLAN9=$P9P "$P9P/bin/9c" -o strtomp0.o strtomp0.c
PLAN9=$P9P "$P9P/bin/9l" -o mpc mpc.o strtomp0.o

cd "$ROOT/sys/src/libsec/port"
# replace a file only when its content changes: its time is what build.exe -D
# compares (libsec and libauthsrv both run this step)
update(){ if cmp -s "$TMP/out" "$1"; then rm "$TMP/out"; else mv "$TMP/out" "$1"; fi; }
for f in *.mp; do
	{ echo '#include "os.h"'; echo '#include <mp.h>'; "$TMP/mpc" "$f"; } > "$TMP/out"
	update "${f%.mp}.c"
	echo "libsec/port/${f%.mp}.c"
done
cd "$ROOT/sys/src/libauthsrv"
for f in *.mp; do
	"$TMP/mpc" "$f" > "$TMP/out"
	update "${f%.mp}.mpc"
	echo "libauthsrv/${f%.mp}.mpc"
done
