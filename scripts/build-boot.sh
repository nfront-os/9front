#!/bin/bash
# Build 9front's boot loaders -- the 16-bit PC loaders (mbr, pbs,
# 9bootiso, 9bootfat, 9boothyb, 9bootpxe) and the EFI loaders
# (bootia32.efi, bootx64.efi, bootaa64.efi, efiboot.fat) -- the way 9front
# does: with sys/src/boot/{pc,efi}/mkfile and 9front's own compilers
# (8c/8a/8l, 6c/6a/6l, 7c/7a/7l), which run in the gcc-built system. No
# port of the loaders: scripts/boot-cli.sh boots the system and runs
# scripts/boot/mkboot.rc on a copy of the sources; the results are
# installed into $ROOT/386 and $ROOT/arm64 like a native mk install.
# The compilers' LP64 safety is checked by scripts/port/cmp-lp64.sh.
#
# usage: scripts/build-boot.sh   (needs scripts/build-cli.sh first)
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
die(){ echo "build-boot: $*" >&2; exit 1; }

# the sources and what their mkfiles include, and empty install dirs
S=$TMP/src9
mkdir -p "$S/386" "$S/arm64"
git -C "$ROOT" ls-files sys/include 386/include 386/mkfile amd64/include amd64/mkfile \
	arm64/include arm64/mkfile sys/src/boot/pc sys/src/boot/efi sys/src/mkfile.proto \
	sys/src/cmd/mkone sys/src/cmd/mklib sys/src/cmd/mksyslib \
	| (cd "$ROOT" && tar -cf - -T -) | tar -xf - -C "$S"

mkdir -p "$TMP/extra/rc/bin"
cp "$ROOT/scripts/boot/mkboot.rc" "$TMP/extra/rc/bin/mkboot"
SRC9=$S UFS_EXTRA=$TMP/extra WAITFOR='mkboot: (done|failed)' WAITMAX=${WAITMAX:-1800} CMDWAIT=5 \
	bash "$ROOT/scripts/boot-cli.sh" 'mkboot' > "$CLI_DIR/mkboot.log" 2>&1 || true
grep -q '^mkboot: done' "$CLI_DIR/mkboot.log" \
	|| { tail -30 "$CLI_DIR/mkboot.log"; die "build failed (log: $CLI_DIR/mkboot.log)"; }

mkdir -p "$ROOT/386" "$ROOT/arm64"
for f in mbr pbs 9bootiso 9bootfat 9boothyb 9bootpxe bootia32.efi bootx64.efi efiboot.fat; do
	[ -s "$S/386/$f" ] || die "missing /386/$f"
	cp "$S/386/$f" "$ROOT/386/$f"
done
[ -s "$S/arm64/bootaa64.efi" ] || die "missing /arm64/bootaa64.efi"
cp "$S/arm64/bootaa64.efi" "$ROOT/arm64/bootaa64.efi"
ls -l "$ROOT"/386/{mbr,pbs,9boot*,*.efi,efiboot.fat} "$ROOT"/arm64/bootaa64.efi | awk '{print $5, $9}'
echo "ok: boot loaders in 386/, arm64/"
