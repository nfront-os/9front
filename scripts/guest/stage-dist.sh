#!/bin/bash
# Stage the 9front distribution tree on the build host: what
# sys/lib/dist/mkfile copies onto an install disk (disk/mkfs -s /n/src9
# with distproto and 9bootproto), after its binds:
#
#	binds:V:
#		test -d /n/src9/.git && bind /n/src9 /n/src9/dist/9front
#		bind cfg /n/src9/cfg
#		bind mail/lib /n/src9/mail/lib
#		bind ndb /n/src9/lib/ndb
#		bind -a adm/timezone /n/src9/adm/timezone
#		bind usr /n/src9/usr
#
# The binds become a view of symlinks (dist/9front, the repository
# itself, is left out: the disk gets the system, not a second source
# tree); /sys/src holds the repository's files only, not the gcc build's
# objects in the working tree; scripts/guest/boot/gccproto adds /amd64/lib.gcc,
# the gcc build's archives (distproto ships /amd64/lib empty, as 9front
# does: libraries are built with mk on the installed system);
# scripts/guest/proto.py expands the protos. Our
# binaries carry DWARF (8l writes none) and are stripped. The guest's install script
# (scripts/guest/boot/mkdisk.rc) reads the result as /n/src9.
#
# usage: [objtype=386] scripts/guest/stage-dist.sh dir
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
OUT=${1:?usage: stage-dist.sh dir}
DIST=$ROOT/sys/lib/dist
VIEW=$(mktemp -d)
trap 'rm -rf "$VIEW"' EXIT

# view: every top-level entry of the tree, with the bound ones replaced
for f in "$ROOT"/*; do
	ln -s "$f" "$VIEW/$(basename "$f")"
done
rm -f "$VIEW/cfg" "$VIEW/usr" "$VIEW/mail" "$VIEW/lib" "$VIEW/adm"
ln -s "$DIST/cfg" "$VIEW/cfg"
ln -s "$DIST/usr" "$VIEW/usr"
mkdir "$VIEW/mail" "$VIEW/lib" "$VIEW/adm" "$VIEW/adm/timezone"
for f in "$ROOT"/mail/*; do [ -e "$f" ] && ln -s "$f" "$VIEW/mail/$(basename "$f")"; done
rm -f "$VIEW/mail/lib"; ln -s "$DIST/mail/lib" "$VIEW/mail/lib"
for f in "$ROOT"/lib/*; do ln -s "$f" "$VIEW/lib/$(basename "$f")"; done
rm -f "$VIEW/lib/ndb"; ln -s "$DIST/ndb" "$VIEW/lib/ndb"
for f in "$ROOT"/adm/*; do [ "$(basename "$f")" = timezone ] || ln -s "$f" "$VIEW/adm/$(basename "$f")"; done
# bind -a: the tree's zones first, then dist's
for f in "$DIST"/adm/timezone/* "$ROOT"/adm/timezone/*; do
	ln -sfn "$f" "$VIEW/adm/timezone/$(basename "$f")"
done
# /sys/src: the sources as the repository has them -- the working tree
# also holds the gcc build's objects, archives and generated files
rm "$VIEW/sys"
mkdir "$VIEW/sys"
for f in "$ROOT"/sys/*; do
	[ "$(basename "$f")" = src ] || ln -s "$f" "$VIEW/sys/$(basename "$f")"
done
git -C "$ROOT" ls-files -z sys/src | (cd "$VIEW" && xargs -0 -n 500 sh -c '
	for f; do mkdir -p "$(dirname "$f")"; ln -s "'"$ROOT"'/$f" "$f"; done' sh)
# generated files a 9front release tree has: libc/ucd's tables, which mk
# would make with a helper linked against a Plan 9 libc.a (the native
# /amd64/lib; the gcc build's archives are in /amd64/lib.gcc)
for f in "$ROOT"/sys/src/libc/ucd/rune*data; do
	ln -s "$f" "$VIEW/sys/src/libc/ucd/$(basename "$f")"
done

# the kernel where 9front installs it: amd64/9pc64 (386/9pc), the a.out
# the build's install step makes (scripts/build/elf2aout.py)
A=${objtype:-amd64}
case $A in amd64) K=9pc64 ;; 386) K=9pc ;; *) echo "stage-dist: objtype=$A unknown" >&2; exit 1 ;; esac
[ -s "$ROOT/$A/$K" ] || { echo "stage-dist: no $A/$K, run CC=gcc ARCH=$A sys/src/cmd/build/build.exe" >&2; exit 1; }
mkdir -p "$VIEW/.$A"
for f in "$ROOT"/$A/*; do ln -s "$f" "$VIEW/.$A/$(basename "$f")"; done
rm "$VIEW/$A"; mv "$VIEW/.$A" "$VIEW/$A"

rm -rf "$OUT"
mkdir -p "$OUT"
export objtype=$A
python3 "$ROOT/scripts/guest/proto.py" -s "$VIEW" -d "$OUT" \
	-e tmp -e lib/firmware -e n -e mnt \
	"$ROOT/sys/lib/sysconfig/proto/distproto" "$ROOT/sys/lib/sysconfig/proto/9bootproto" \
	"$ROOT/scripts/guest/boot/gccproto" \
	2> "$OUT.warnings" || { cat "$OUT.warnings"; exit 1; }
# for the guest's disk/mkfs (scripts/guest/boot/mkdisk.rc); no proto lists it,
# so it stays off the disk
cp "$ROOT/scripts/guest/boot/gccproto" "$OUT/gccproto"
find "$OUT" -type f -perm -u+x -exec sh -c 'file -b "$1" | grep -q ^ELF && strip "$1"' _ {} \;
echo "$(grep -c . "$OUT.warnings") proto warnings ($OUT.warnings)"
# everything mk install installs made it into the distribution
python3 "$ROOT/scripts/build/check-install.py" -d "$OUT" | grep -v '^broken' | tail -20
[ "${PIPESTATUS[0]}" = 0 ] || { echo "stage-dist: distribution incomplete" >&2; exit 1; }
du -sh "$OUT"
