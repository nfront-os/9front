#!/bin/bash
# Generate sys/src/cmd/aux/kbdfs/latin1.h (compose-key table) the way
# kbdfs's mkfile does: build the host tool mklatin and run it on
# /lib/keyboard. mklatin runs on the build host, so it is compiled with
# plan9port's 9c/9l (not the Plan 9 target gcc toolchain).
set -e
ROOT=${PLAN9:-$(git rev-parse --show-toplevel)}
P9P=${PLAN9PORT:-$ROOT/../plan9port}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
cd "$TMP"
PLAN9=$P9P "$P9P/bin/9c" -o mklatin.o "$ROOT/sys/src/cmd/aux/kbdfs/mklatin.c"
PLAN9=$P9P "$P9P/bin/9l" -o mklatin mklatin.o
./mklatin "$ROOT/lib/keyboard" > "$ROOT/sys/src/cmd/aux/kbdfs/latin1.h"
echo "wrote $(wc -l < "$ROOT/sys/src/cmd/aux/kbdfs/latin1.h") lines to sys/src/cmd/aux/kbdfs/latin1.h"
