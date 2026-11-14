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
# usage: gen-ufs-root.sh <outdir> [extra...]
#   then: ufs -root=<outdir> -addr=:5640
# Each extra directory's files go over the root (boot-cli.sh's UFS_EXTRA).
#
# The root is put together in <outdir>.stage from hard links of the
# sources (no file data written; nothing writes into the stage), then
# rsync -a --delete makes <outdir> the same: it writes only what differs,
# the files of a new build and what the last guest changed or made, not
# the whole root (~3 GB with the GPU tests) for every boot. Sources on
# another file system are copied into the stage instead (slower, same root,
# twice the root's space for the while).
set -e
ROOT=${PLAN9:-$(git rev-parse --show-toplevel)}
DEST=${1:?usage: $0 outdir [extra...]}
shift
A=${objtype:-amd64}
command -v rsync >/dev/null || { echo "gen-ufs-root.sh: rsync missing" >&2; exit 1; }
OUT=$DEST.stage
# cp's arguments, as links (-f: a later source replaces a file, its link
# only); copied where linking fails (another file system), the links
# removed first, not written through (they are the tree's files); with
# their times, as a link has them (tests/replica: an installed system's
# files have the replica's times)
put(){ cp -alf "$@" 2>/dev/null || cp -r --preserve=mode,timestamps --remove-destination "$@"; }

rm -rf "$OUT"
mkdir -p "$OUT"/$A/bin "$OUT"/rc "$OUT"/lib "$OUT"/adm/timezone \
	"$OUT"/usr/glenda/tmp "$OUT"/tmp "$OUT"/n "$OUT"/mnt "$OUT"/srv

# directories a 9front root has, here empty: /386, /arm64 (where the
# boot loaders install), /$objtype/include, /sys/include, /sys/src (bound
# over by a source tree in the guest)
mkdir -p "$OUT"/386 "$OUT"/arm64 "$OUT"/$A/include "$OUT"/sys/include "$OUT"/sys/src
put "$ROOT"/$A/init "$OUT"/$A/init
put "$ROOT"/$A/bin/. "$OUT"/$A/bin/
rm -f "$OUT"/$A/bin/9pc64 "$OUT"/$A/bin/9pc "$OUT"/$A/bin/9qemu	# the kernel

put "$ROOT"/rc/lib "$ROOT"/rc/bin "$OUT"/rc/
put "$ROOT"/lib/. "$OUT"/lib/	# namespace, vgadb, fonts, keyboard, ...
mkdir -p "$OUT"/lib/ndb	# like the dist install: /lib/ndb from sys/lib/dist/ndb
put "$ROOT"/sys/lib/dist/ndb/* "$OUT"/lib/ndb/ && rm -f "$OUT"/lib/ndb/mkfile
put "$ROOT"/adm/timezone/GMT "$OUT"/adm/timezone/local
# like the dist install: the user's home from sys/lib/dist/usr/glenda
put "$ROOT"/sys/lib/dist/usr/glenda/. "$OUT"/usr/glenda/
mkdir -p "$OUT"/usr/glenda/bin/$A
# /sys/lib: plumbing rules, troff macros, ...
mkdir -p "$OUT"/sys
put "$ROOT"/sys/lib "$OUT"/sys/

# guest tests (tests/guest), if built: /tests/run [test...]
[ ! -d "$ROOT"/tests/guest/root ] || put "$ROOT"/tests/guest/root/. "$OUT"/
# performance counter tests (tests/pmc), if built
[ ! -d "$ROOT"/tests/pmc/root ] || put "$ROOT"/tests/pmc/root/. "$OUT"/

for x; do put "$x"/. "$OUT"/; done

mkdir -p "$DEST"
rsync -a --delete "$OUT"/ "$DEST"/
rm -rf "$OUT"
echo "ufs root in $DEST:"
ls "$DEST"/$A/bin
