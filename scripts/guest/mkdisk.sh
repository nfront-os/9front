#!/bin/bash
# Make a bootable 9front disk image with an hjfs root: the system as
# 9front's installer leaves it (sys/lib/dist/mkfile, %.amd64.qcow2).
#
#   1. scripts/guest/stage-dist.sh: the distribution tree (distproto,
#      9bootproto, the mkfile's binds), served to the guest as /n/src9
#   2. scripts/guest/boot-cli.sh boots the system with an empty disk (sdC0) and
#      runs scripts/guest/boot/mkdisk.rc there: mbr -m /386/mbr, fdisk, prep
#      (9fat nvram fs), format -b /386/pbs 9fat (9bootfat, plan9.ini,
#      kernel), hjfs -r, users, disk/mkfs
#   Boots like 9front's image: BIOS -> mbr -> pbs -> 9bootfat -> 9pc64;
#   plan9.ini has only bootfile and console, so bootrc asks for bootargs.
#
# usage: scripts/guest/mkdisk.sh [image]	(default $CLI_DIR/9front.amd64.img;
#        .qcow2 converts with qemu-img). Needs the build
#        (CC=gcc ARCH=amd64 sys/src/cmd/build/build.exe) and scripts/build-boot.sh.
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
A=${objtype:-amd64}
OUT=${1:-$CLI_DIR/9front.$A.img}
MB=3770	# the mkfile's size for amd64 (and arm64) disks
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
die(){ echo "mkdisk: $*" >&2; exit 1; }
case $A in
amd64)	need="386/mbr 386/pbs 386/9bootfat" ;;
# as 9front's %.arm64.qcow2: u-boot's boot.scr and the kernel's u-boot image
riscv64)	need="sys/src/boot/virt/boot.scr riscv64/9virt.u" ;;
*)	die "objtype=$A: no disk image" ;;
esac
for f in $need; do
	[ -s "$ROOT/$f" ] || die "no $f, run scripts/guest/build-boot.sh"
done

echo "stage the distribution"
objtype=$A bash "$ROOT/scripts/guest/stage-dist.sh" "$CLI_DIR/src9"
[ $A != riscv64 ] || cp "$ROOT/sys/src/boot/virt/boot.scr" "$CLI_DIR/src9/.boot.scr"

echo "install onto an empty disk in the guest"
IMG=$CLI_DIR/mkdisk.raw
rm -f "$IMG"
truncate -s ${MB}M "$IMG"
mkdir -p "$TMP/extra/rc/bin"
cp "$ROOT/scripts/guest/boot/mkdisk.rc" "$TMP/extra/rc/bin/mkdisk"
# the installer runs on amd64, as 9front makes any machine's image anywhere
objtype=amd64 SRC9=$CLI_DIR/src9 DISK=$IMG UFS_EXTRA=$TMP/extra MEM=${MEM:-1024} \
WAITFOR='mkdisk: (done|failed)' WAITMAX=${WAITMAX:-7200} CMDWAIT=5 \
	bash "$ROOT/scripts/guest/boot-cli.sh" "mkdisk $A" > "$CLI_DIR/mkdisk.log" 2>&1 || true
grep -q '^mkdisk: done' "$CLI_DIR/mkdisk.log" \
	|| { tail -30 "$CLI_DIR/mkdisk.log"; die "installation failed (log: $CLI_DIR/mkdisk.log)"; }

case $OUT in
*.qcow2)	qemu-img convert -O qcow2 "$IMG" "$OUT" && rm -f "$IMG" ;;
*)	mv "$IMG" "$OUT" ;;
esac
echo "ok: $OUT"
