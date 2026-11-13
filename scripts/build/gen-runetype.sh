#!/bin/bash
# Generate libc's Unicode tables (sys/src/libc/ucd/rune*data) the way
# ucd/mkfile does: build the host tool mkrunetype and run it on the UCD
# files. mkrunetype reads /lib/ucd/... by absolute path, so a temporary
# copy is compiled with those paths pointing into this tree; the tool is
# built with plan9port's 9c/9l.
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
. "$ROOT/scripts/lib/paths.sh"	# PLAN9PORT (the distribution's paths.env)
P9P=$PLAN9PORT
UCD=$ROOT/sys/src/libc/ucd
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
sed "s|\"/lib/ucd/|\"$ROOT/lib/ucd/|" "$UCD/mkrunetype.c" > "$TMP/mkrunetype.c"
cd "$TMP"
PLAN9=$P9P "$P9P/bin/9c" -o mkrunetype.o mkrunetype.c
PLAN9=$P9P "$P9P/bin/9l" -o mkrunetype mkrunetype.o
cd "$UCD"
"$TMP/mkrunetype"
ls -l runenormdata runetotypedata runeistypedata runebreakdata
