#!/bin/bash
# Generate the C sources 9front's mkfiles build from .mp files with the
# host tool mpc (sys/src/cmd/mpc.y):
#   libsec/port/%.c     <- %.mp  ('#include "os.h"', '#include <mp.h>', mpc output)
#   libauthsrv/%.mpc    <- %.mp  (mpc output)
# mpc runs on the build host, so it is built with plan9port's yacc/9c/9l.
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
P9P=${PLAN9PORT:-$ROOT/../plan9port}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

cd "$TMP"
PLAN9=$P9P "$P9P/bin/yacc" "$ROOT/sys/src/cmd/mpc.y"
PLAN9=$P9P "$P9P/bin/9c" -o mpc.o y.tab.c
PLAN9=$P9P "$P9P/bin/9l" -o mpc mpc.o

cd "$ROOT/sys/src/libsec/port"
for f in *.mp; do
	{ echo '#include "os.h"'; echo '#include <mp.h>'; "$TMP/mpc" "$f"; } > "${f%.mp}.c"
	echo "libsec/port/${f%.mp}.c"
done
cd "$ROOT/sys/src/libauthsrv"
for f in *.mp; do
	"$TMP/mpc" "$f" > "${f%.mp}.mpc"
	echo "libauthsrv/${f%.mp}.mpc"
done
