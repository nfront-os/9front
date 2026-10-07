#!/bin/bash
# Build the host-side root tree that Harvey's `ufs` 9P server exports
# to the pc64 test kernel (plan Phase 2.5; scripts/boot/netmount.c mounts
# it as /root and execs /$cputype/init from it, like bootrc would).
#
# Layout follows the stock 9front root (only what init + rc need for a
# minimal interactive shell):
#   /amd64/init, /amd64/bin/*   gcc-built binaries from $ROOT/amd64/bin
#   /rc/lib/rcmain, /rc/bin     stock rc library/scripts
#   /lib/namespace              stock name space file (used by newns)
#   /adm/timezone/local         init copies it into #e/timezone
#   /usr/glenda, /tmp, mount points referenced by /lib/namespace
#
# usage: gen-ufs-root.sh <outdir>
#   then: ufs -root=<outdir> -addr=:5640
set -e
ROOT=${PLAN9:-$(git rev-parse --show-toplevel)}
OUT=${1:?usage: $0 outdir}

rm -rf "$OUT"
mkdir -p "$OUT"/amd64/bin "$OUT"/rc "$OUT"/lib "$OUT"/adm/timezone \
	"$OUT"/usr/glenda/tmp "$OUT"/tmp "$OUT"/n "$OUT"/mnt "$OUT"/srv

cp "$ROOT"/amd64/bin/init "$OUT"/amd64/init
for f in "$ROOT"/amd64/bin/*; do
	[ -f "$f" ] || continue	# aux/ etc.
	case $(basename "$f") in
	9pc64|init|netmount)	;;	# kernel / boot-only / test programs
	*)	cp "$f" "$OUT"/amd64/bin/ ;;
	esac
done
if [ -d "$ROOT"/amd64/bin/aux ]; then
	mkdir -p "$OUT"/amd64/bin/aux
	cp "$ROOT"/amd64/bin/aux/* "$OUT"/amd64/bin/aux/	# kbdfs (console line discipline)
fi

cp -r "$ROOT"/rc/lib "$ROOT"/rc/bin "$OUT"/rc/
cp "$ROOT"/lib/namespace "$OUT"/lib/
cp "$ROOT"/adm/timezone/GMT "$OUT"/adm/timezone/local

echo "ufs root in $OUT:"
ls "$OUT"/amd64/bin
