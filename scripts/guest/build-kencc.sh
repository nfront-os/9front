#!/bin/bash
# Check that the system still builds with 9front's own compilers (kencc:
# 6c/6a/6l, mk) -- the gcc port must not break them. Boots the
# gcc-built system and runs scripts/guest/boot/mkkencc.rc there:
# cd /sys/src; mk install (libraries, ape, acme, cmd, games) and the
# kernel pc64, on a copy of the tree's sources ($CLI_DIR/kencc:
# git ls-files, no gcc objects); mk installs into the guest's root.
# 9front's top-level mkfile goes on after a failed directory, so the
# result is read from mk's messages: every "mk: ... exit status" is a
# failure, listed with the lines before it.
#
# usage: scripts/guest/build-kencc.sh [kernel]   (needs the build first: CC=gcc ARCH=amd64 sys/src/cmd/build/build.exe)
#   kernel: only the libraries and the kernel; its bootdir then takes
#           the guest's gcc-built ELF programs, which 9front's strip
#           does not know (port/mkroot: "not a recognizeable binary",
#           harmless: they go in unstripped)
#   env: WAITMAX  seconds at most for the build in the guest (10800)
#        SMP      CPUs of the guest (4; mk builds in parallel, NPROC)
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
die(){ echo "build-kencc: $*" >&2; exit 1; }

S=$CLI_DIR/kencc
rm -rf "$S"
mkdir -p "$S"
git -C "$ROOT" ls-files -z sys/src sys/include sys/lib acme rc/bin \
	386/include 386/mkfile amd64/include amd64/mkfile arm64/include arm64/mkfile \
	| (cd "$ROOT" && tar -cf - --null -T -) | tar -xf - -C "$S"
mkdir -p "$S/acme/bin/amd64"

T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
# mount points the guest's root lacks
mkdir -p "$T/extra/rc/bin" "$T/extra/amd64/lib/ape" "$T/extra/acme" "$T/extra/lib/firmware"
# the mkfiles' </$objtype/mkfile (the guest's /amd64 has none); the
# other architectures' include directories (libmach includes every
# ureg.h), which the guest's root lacks
cp "$ROOT/amd64/mkfile" "$T/extra/amd64/mkfile"
for a in $(git -C "$ROOT" ls-files '*/include/*' | cut -d/ -f1 | sort -u); do
	case $a in sys|amd64|386|arm64) continue ;; esac
	mkdir -p "$T/extra/$a" && cp -r "$ROOT/$a/include" "$T/extra/$a/"
done
cp "$ROOT/scripts/guest/boot/mkkencc.rc" "$T/extra/rc/bin/mkkencc"
LOG=$CLI_DIR/mkkencc.log
objtype=amd64 MEM=${MEM:-2048} QEMU_EXTRA="-smp ${SMP:-4} $QEMU_EXTRA" SRC9=$S UFS_EXTRA=$T/extra WAITFOR='mkkencc: (done|failed:)' \
	WAITMAX=${WAITMAX:-10800} CMDWAIT=5 \
	bash "$ROOT/scripts/guest/boot-cli.sh" "mkkencc $1" > "$LOG" 2>&1 || true
tr -d '\r' < "$LOG" > "$LOG.txt"
grep -qE '^mkkencc: (done|failed:)' "$LOG.txt" || { tail -30 "$LOG.txt"; die "did not finish (log: $LOG.txt)"; }

n=$(grep -c 'mk: .*exit status' "$LOG.txt" || true)
# the kernel's mk may fail without an "exit status" line (a missing target)
grep -qE '^mkkencc: status kernel pc64 *$' "$LOG.txt" || n=$((n+1))
grep '^mkkencc: status' "$LOG.txt"
echo "built: $(grep -c '^cp 6\.' "$LOG.txt" || true) programs installed, kernel: $(grep -c '^mkkencc: status kernel pc64 *$' "$LOG.txt" || true) ok"
ls -l "$S"/sys/src/9/pc64/9pc64 2>/dev/null | awk '{print $5, $9}' || true
if [ "$n" != 0 ]; then
	grep -B3 'mk: .*exit status' "$LOG.txt" | head -150
	die "$n failed mk targets (log: $LOG.txt)"
fi
echo "ok: the tree builds with kencc"
