#!/bin/bash
# Make the 9front amd64 (objtype=386: 386) install/live CD as the release
# does (sys/lib/dist/mkfile, %.amd64.iso, %.386.iso); the system of that
# architecture makes it:
#
#   1. scripts/guest/stage-dist.sh: the distribution tree (distproto,
#      9bootproto, the mkfile's binds), served to the guest as /n/src9
#   2. scripts/guest/boot-cli.sh boots the system and runs scripts/guest/boot/mkiso.rc
#      there: disk/mk9660 -c9j with 9bootiso (BIOS, El Torito) and
#      efiboot.fat (UEFI), then the hybrid part -- an MBR with mbr and a
#      FAT ESP (pbs, 9boothyb as 9bootfat, efi/boot/boot*.efi) appended,
#      so the image also boots from a disk or USB stick; made in a ramfs
#      in the guest and copied onto a scratch disk, whose first bytes
#      are the ISO
#
# usage: [objtype=386] scripts/guest/mkiso.sh [iso]	(default $CLI_DIR/9front.$objtype.iso).
#        PLAN9INI: lines name=value for the CD's plan9.ini, each in place of
#        the CD's own line of that name (sys/lib/dist/cfg/plan9.ini), e.g.
#        PLAN9INI='monitor=i915' boots with i915 as the screen (gpustart).
#        Needs the build (CC=gcc ARCH=$objtype sys/src/cmd/build/build.exe)
#        and scripts/build-boot.sh.
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
export objtype=${objtype:-amd64}
OUT=${1:-$CLI_DIR/9front.$objtype.iso}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
die(){ echo "mkiso: $*" >&2; exit 1; }
for f in mbr pbs 9bootiso 9boothyb efiboot.fat bootx64.efi; do
	[ -s "$ROOT/386/$f" ] || die "no 386/$f, run scripts/guest/build-boot.sh"
done

echo "stage the distribution"
bash "$ROOT/scripts/guest/stage-dist.sh" "$CLI_DIR/src9"

echo "make the CD in the guest"
DSK=$CLI_DIR/mkiso.raw
rm -f "$DSK"
# the guest writes the CD here (sparse: only what it writes takes room);
# 4 GB: with GPU programs (wsys, ui: libvulkan.a) it is past 2 GB
truncate -s 4G "$DSK"
mkdir -p "$TMP/extra/rc/bin"
cp "$ROOT/scripts/guest/boot/mkiso.rc" "$TMP/extra/rc/bin/mkiso"
if [ -n "$PLAN9INI" ]; then
	printf '%s\n' "$PLAN9INI" | grep -v '^[ 	]*$' > "$TMP/extra/mkiso.ini"
	grep -qv '^[A-Za-z0-9_*.]*=' "$TMP/extra/mkiso.ini" && die "PLAN9INI: lines name=value"
	echo "plan9.ini:"; sed 's/^/	/' "$TMP/extra/mkiso.ini"
fi
# MEM: the guest holds the whole image in its ramfs (mkiso.rc) besides the system: the
# amd64 CD is past 2 GB since wsys links libvulkan.a (2048 MB: "ramfs: Insufficient
# physical memory"); the 386 CD is smaller, and 9pc uses at most 4 GB
[ "$objtype" = 386 ] && mem=3072 || mem=6144
SRC9=$CLI_DIR/src9 DISK=$DSK UFS_EXTRA=$TMP/extra MEM=${MEM:-$mem} \
WAITFOR='mkiso: (done|failed)' WAITMAX=${WAITMAX:-7200} CMDWAIT=5 \
	bash "$ROOT/scripts/guest/boot-cli.sh" "mkiso $objtype" > "$CLI_DIR/mkiso.log" 2>&1 || true
size=$(tr -d '\r' < "$CLI_DIR/mkiso.log" | sed -n 's/^mkiso: done \([0-9][0-9]*\)$/\1/p')
[ -n "$size" ] || { tail -30 "$CLI_DIR/mkiso.log"; die "failed (log: $CLI_DIR/mkiso.log)"; }
head -c "$size" "$DSK" > "$OUT"
rm -f "$DSK"
echo "ok: $OUT ($(du -h "$OUT" | cut -f1))"
