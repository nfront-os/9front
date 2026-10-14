#!/bin/bash
# Build the host-side root tree that Harvey's `ufs` 9P server exports
# to the kernel booted by scripts/guest/boot-cli.sh (bootrc's tcp root,
# srv tcp!fs!564), for $objtype (default amd64).
#
# Layout follows the stock 9front root:
#   /$objtype/init, /$objtype/bin/... gcc-built binaries from $ROOT/$objtype/bin
#   /rc/lib/rcmain, /rc/bin     stock rc library/scripts
#   /lib/namespace, /lib/ndb    stock name space file (newns), network db
#   /adm/timezone/local         init copies it into #e/timezone
#   /usr/glenda, /tmp, mount points referenced by /lib/namespace
#
# usage: gen-ufs-root.sh <outdir>
#   then: ufs -root=<outdir> -addr=:5640
set -e
ROOT=${PLAN9:-$(git rev-parse --show-toplevel)}
OUT=${1:?usage: $0 outdir}
A=${objtype:-amd64}

rm -rf "$OUT"
mkdir -p "$OUT"/$A/bin "$OUT"/rc "$OUT"/lib "$OUT"/adm/timezone \
	"$OUT"/usr/glenda/tmp "$OUT"/tmp "$OUT"/n "$OUT"/mnt "$OUT"/srv

# directories a 9front root has, here empty: /386, /arm64 (where the
# boot loaders install), /$objtype/include, /sys/include, /sys/src (bound
# over by a source tree in the guest)
mkdir -p "$OUT"/386 "$OUT"/arm64 "$OUT"/$A/include "$OUT"/sys/include "$OUT"/sys/src
cp "$ROOT"/$A/init "$OUT"/$A/init
cp -r "$ROOT"/$A/bin/. "$OUT"/$A/bin/
rm -f "$OUT"/$A/bin/9pc64 "$OUT"/$A/bin/9pc "$OUT"/$A/bin/9qemu	# the kernel

cp -r "$ROOT"/rc/lib "$ROOT"/rc/bin "$OUT"/rc/
cp -r "$ROOT"/lib/. "$OUT"/lib/	# namespace, vgadb, fonts, keyboard, ...
mkdir -p "$OUT"/lib/ndb	# like the dist install: /lib/ndb from sys/lib/dist/ndb
cp "$ROOT"/sys/lib/dist/ndb/* "$OUT"/lib/ndb/ && rm -f "$OUT"/lib/ndb/mkfile
cp "$ROOT"/adm/timezone/GMT "$OUT"/adm/timezone/local
# like the dist install: the user's home from sys/lib/dist/usr/glenda
cp -r "$ROOT"/sys/lib/dist/usr/glenda/. "$OUT"/usr/glenda/
mkdir -p "$OUT"/usr/glenda/bin/$A
# /sys/lib: plumbing rules, troff macros, ...
mkdir -p "$OUT"/sys
cp -r "$ROOT"/sys/lib "$OUT"/sys/

# guest tests (tests/guest), if built: /tests/run [test...]
[ ! -d "$ROOT"/tests/guest/root ] || cp -r "$ROOT"/tests/guest/root/. "$OUT"/

echo "ufs root in $OUT:"
ls "$OUT"/$A/bin
