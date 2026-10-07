#!/bin/bash
# Plan 9 yacc for the gcc build: plan9port's yacc, run with 9front's own
# parser templates (sys/lib/yaccpar, yaccpars) so the output matches what
# 9front's yacc would generate (including the y.tab.h format that tools
# such as awk's maketab parse). Arguments are passed to yacc unchanged.
set -e
ROOT=$(git -C "$(dirname "$0")" rev-parse --show-toplevel)
P9P=${PLAN9PORT:-$ROOT/../plan9port}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
mkdir "$TMP/lib"
cp "$ROOT/sys/lib/yaccpar" "$ROOT/sys/lib/yaccpars" "$TMP/lib/"
PLAN9=$TMP "$P9P/bin/yacc" "$@"
