#!/bin/bash
# The system builds itself: boots the gcc-built system with the native
# gcc and binutils of ../9front-gcc (/$objtype/gnu) and runs the build
# tool there (scripts/guest/boot/mkself.rc) on a copy of the tree's
# sources ($CLI_DIR/self: git ls-files) -- the build of
# CC=gcc ARCH=amd64 sys/src/cmd/build/build.exe, on 9front. The build
# tool for Plan 9 is built here (GOOS=plan9), the build steps are
# scripts/build/rc/*.rc, the expected files check-install.py -l's.
# The guest gets the copy as an ext4 disk ($CLI_DIR/self.img, made by
# mkfs.ext4 -d, served by 9front's ext4srv: much faster than ufs over
# the network); afterwards the image's tree goes back to $CLI_DIR/self
# (debugfs rdump). The result stays there (amd64/bin, the kernel); its
# objects are compared with this tree's (the Linux build's, if built for
# amd64).
#
# usage: scripts/guest/build-self.sh [buildfile...]   (needs the build, the
#        native toolchain: ../9front-gcc's out/amd64, GCC_OUT)
#        buildfile: only these manifests (relative to the tree, e.g.
#        sys/src/ape/cmd/cmds.json), else the whole build.json
#   env: WAITMAX  seconds at most for the build in the guest (28800)
#        SMP      CPUs of the guest (4)
#        KEEP     1: build in the copy of the last run (scripts/, the
#                 build tool and the manifests renewed), e.g. after a fix
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
A=amd64
GCC_OUT=${GCC_OUT:-$ROOT/../9front-gcc/out/$A}
die(){ echo "build-self: $*" >&2; exit 1; }
[ -x "$GCC_OUT/$A/gnu/bin/gcc" ] || die "no $GCC_OUT/$A/gnu/bin/gcc (../9front-gcc: build.sh native)"
[ -s "$ROOT/$A/lib.gcc/libc.a" ] || die "no $A/lib.gcc/libc.a, run CC=gcc ARCH=$A sys/src/cmd/build/build.exe"

S=$CLI_DIR/self
if [ "$KEEP" = 1 ] && [ -d "$S/sys/src" ]; then
	git -C "$ROOT" ls-files -z scripts '*.json' | (cd "$ROOT" && tar -cf - --null -T -) | tar -xf - -C "$S"
else
	rm -rf "$S"
	mkdir -p "$S"
	git -C "$ROOT" ls-files -z | (cd "$ROOT" && tar -cf - --null -T -) | tar -xf - -C "$S"
fi
(cd "$ROOT/sys/src/cmd/build" && GOOS=plan9 GOARCH=$A GO111MODULE=off go build -o "$S/sys/src/cmd/build/build.exe" .)
objtype=$A python3 "$ROOT/scripts/build/check-install.py" -l > "$S/check-install.$A"

T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
# what the guest's root lacks: the native toolchain, the installed
# libraries and the mkfile of /$objtype (helper programs, mk)
mkdir -p "$T/extra/rc/bin" "$T/extra/$A"
cp -r "$GCC_OUT/$A/gnu" "$T/extra/$A/"
cp -r "$ROOT/$A/lib.gcc" "$ROOT/$A/mkfile" "$ROOT/$A/plan9.ld" "$T/extra/$A/"
cp "$ROOT/scripts/guest/boot/mkself.rc" "$T/extra/rc/bin/mkself"

# the copy as an ext4 file system with room for the build (virtio disk:
# sdF0 in the guest). No journal, orphan file or metadata checksums:
# ext4srv (lwext4) need not know them, the image is scratch
IMG=$CLI_DIR/self.img
mb=$(( $(du -sm "$S" | cut -f1) + 4096 ))
rm -f "$IMG"
mkfs.ext4 -q -F -L src9 -O ^has_journal,^orphan_file,^metadata_csum,^metadata_csum_seed \
	-E root_owner=$(id -u):$(id -g) -d "$S" "$IMG" ${mb}M || die "mkfs.ext4 $IMG"

LOG=$CLI_DIR/mkself.log
objtype=$A MEM=${MEM:-4096} UFS_EXTRA=$T/extra \
	QEMU_EXTRA="-smp ${SMP:-4} -drive file=$IMG,format=raw,if=virtio,cache=unsafe $QEMU_EXTRA" \
	WAITFOR='mkself: (done|failed:)' WAITMAX=${WAITMAX:-28800} CMDWAIT=5 \
	bash "$ROOT/scripts/guest/boot-cli.sh" "mkself $*" > "$LOG" 2>&1 || true
tr -d '\r' < "$LOG" > "$LOG.txt"

# the tree as the guest left it back to $S (chown fails, not root)
rm -rf "$S.new"
mkdir -p "$S.new"
debugfs -R "rdump / $S.new" "$IMG" 2>&1 | grep -vE '^debugfs [0-9]|changing ownership' | grep . && die "debugfs rdump $IMG"
rm -rf "$S.new/lost+found" "$S"
mv "$S.new" "$S"
grep -qE '^mkself: (done|failed:)' "$LOG.txt" || { tail -30 "$LOG.txt"; die "did not finish (log: $LOG.txt)"; }
grep -E '^mkself: status|files expected' "$LOG.txt"
grep -qE 'files expected, ok' "$LOG.txt" || { grep -B5 -E 'exit status|failed' "$LOG.txt" | tail -40; die "build failed (log: $LOG.txt)"; }
echo "ok: the system built itself ($S)"

# the same code as the build on Linux: every object of the tree, without
# debug information and .comment (the compilers' version strings)
same=0 diff=0
: > "$CLI_DIR/self.diff"
while IFS= read -r o; do
	[ -f "$ROOT/$o" ] || continue
	objcopy --strip-debug -R .comment -R .note.gnu.property "$S/$o" "$T/a.o" 2>/dev/null || continue
	objcopy --strip-debug -R .comment -R .note.gnu.property "$ROOT/$o" "$T/b.o" 2>/dev/null || continue
	if cmp -s "$T/a.o" "$T/b.o"; then
		same=$((same+1))
	else
		diff=$((diff+1))
		echo "$o" >> "$CLI_DIR/self.diff"
	fi
done < <(cd "$S" && find sys/src acme -name '*.o' | sort)
echo "objects as on Linux: $same, different: $diff ($CLI_DIR/self.diff)"
