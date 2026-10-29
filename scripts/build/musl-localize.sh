#!/bin/bash
# After APE's musl part is appended to libap.a (sys/src/ape/lib/musl,
# genmusl.py's Post step): in each musl member (x.musl.o), the names that
# libap's own members define too become local, as 9front-gcc's compat had
# it (i386's floor.s is ceil too, scalbn.s ldexp; libap keeps its own).
# Replaced only when it changes (build.exe -D).
#
# usage: objtype=386|amd64|arm64 scripts/build/musl-localize.sh   (build.exe sets objtype)
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
objtype=${objtype:?objtype not set}
case $objtype in arm64) P=${TOOLPREFIX-aarch64-linux-musl-} ;; *) P= ;; esac
A=$ROOT/$objtype/lib.gcc/ape/libap.a
T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
# member: symbol, for the defined globals
${P}nm -A -g --defined-only "$A" 2>/dev/null |
	awk '$(NF-1) ~ /^[TDBRCVW]$/ {n = split($1, f, ":"); print f[n-1], $NF}' > "$T/defs"
awk '$1 !~ /\.musl\.o$/ {print $2}' "$T/defs" | sort -u > "$T/ape"
cd "$T"
changed=
for m in $(awk '$1 ~ /\.musl\.o$/ {print $1}' defs | sort -u); do
	syms=$(awk -v m="$m" '$1 == m {print $2}' defs | sort -u | comm -12 - ape)
	[ -n "$syms" ] || continue
	${P}ar x "$A" "$m"
	${P}objcopy $(for s in $syms; do printf -- '--localize-symbol=%s ' "$s"; done) "$m"
	${P}ar r "$A" "$m"
	echo "musl-localize: $m: $syms"
	changed=1
done
[ -z "$changed" ] || ${P}ranlib "$A"
