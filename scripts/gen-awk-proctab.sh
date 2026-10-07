#!/bin/bash
# Generate sys/src/cmd/awk/proctab.c the way awk's mkfile does: build the
# host tool maketab (needs y.tab.h from awkgram.y, see the awk manifest's
# Pre step) and run it. maketab runs on the build host, so it is built
# with plan9port's 9c/9l.
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
P9P=${PLAN9PORT:-$ROOT/../plan9port}
AWK=$ROOT/sys/src/cmd/awk
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
cd "$TMP"
PLAN9=$P9P "$P9P/bin/9c" -I "$AWK" -o maketab.o "$AWK/maketab.c"
PLAN9=$P9P "$P9P/bin/9l" -o maketab maketab.o
(cd "$AWK" && "$TMP/maketab" > proctab.c)	# reads ./y.tab.h
echo "wrote sys/src/cmd/awk/proctab.c"
