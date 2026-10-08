#!/bin/bash
# Plan 9 lex for the gcc build: plan9port's lex with 9front's own
# template (sys/lib/lex/ncform). Arguments are passed to lex unchanged;
# --out file first writes lex's standard output (-t) to file (the
# build tool's Pre steps have no shell redirection).
set -e
ROOT=$(git -C "$(dirname "$0")" rev-parse --show-toplevel)
P9P=${PLAN9PORT:-$ROOT/../plan9port}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
mkdir "$TMP/lib"
cp "$ROOT/sys/lib/lex/ncform" "$TMP/lib/lex.ncform"
if [ "$1" = --out ]; then
	out=$2
	shift 2
	PLAN9=$TMP "$P9P/bin/lex" "$@" > "$out"
else
	PLAN9=$TMP "$P9P/bin/lex" "$@"
fi
