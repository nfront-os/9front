#!/bin/bash
# Make a bootable 9front disk image with an hjfs root: the system as
# 9front's installer leaves it (sys/lib/dist/mkfile, %.amd64.qcow2).
#
#   1. scripts/stage-dist.sh: the distribution tree (distproto,
#      9bootproto, the mkfile's binds), served to the guest as /n/src9
#   2. scripts/boot-cli.sh boots the system with an empty disk (sdC0) and
#      runs scripts/boot/mkdisk.rc there: mbr, fdisk, prep (9fat nvram
#      fs), format 9fat (plan9.ini, kernel), hjfs -r, users, disk/mkfs
#   3. GRUB as boot loader -- 9front's 16-bit loaders (/386/mbr, pbs,
#      9bootfat) are not ported: boot.img in the MBR, core.img in the
#      cylinder before the plan9 partition, what grub-bios-setup would do.
#      GRUB reads the kernel from 9fat (part_plan, fat) and passes
#      plan9.ini's settings plus bootargs=local!/dev/sdC0/fs.
#
# usage: scripts/mkdisk.sh [image]	(default $CLI_DIR/9front.amd64.img;
#        .qcow2 converts with qemu-img). Needs scripts/build-cli.sh.
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
OUT=${1:-$CLI_DIR/9front.amd64.img}
MB=3770	# the mkfile's size for amd64 disks
GRUBDIR=/usr/lib/grub/i386-pc
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
die(){ echo "mkdisk: $*" >&2; exit 1; }

echo "stage the distribution"
bash "$ROOT/scripts/stage-dist.sh" "$CLI_DIR/src9"

echo "install onto an empty disk in the guest"
IMG=$CLI_DIR/mkdisk.raw
rm -f "$IMG"
truncate -s ${MB}M "$IMG"
mkdir -p "$TMP/extra/rc/bin"
cp "$ROOT/scripts/boot/mkdisk.rc" "$TMP/extra/rc/bin/mkdisk"
SRC9=$CLI_DIR/src9 DISK=$IMG UFS_EXTRA=$TMP/extra MEM=${MEM:-1024} \
WAITFOR='mkdisk: (done|failed)' WAITMAX=${WAITMAX:-7200} CMDWAIT=5 \
	bash "$ROOT/scripts/boot-cli.sh" 'mkdisk' > "$CLI_DIR/mkdisk.log" 2>&1 || true
grep -q '^mkdisk: done' "$CLI_DIR/mkdisk.log" \
	|| { tail -30 "$CLI_DIR/mkdisk.log"; die "installation failed (log: $CLI_DIR/mkdisk.log)"; }

echo "boot loader: GRUB"
# the plan9 partition (MBR entry 1) and 9fat (first entry of its
# partition table, GRUB's plan1)
cat > "$TMP/early.cfg" <<'EOF'
set root=(hd0,msdos1,plan1)
multiboot /9pc64 console=0 bootfile=9pc64 bootargs=local!/dev/sdC0/fs nobootprompt=local!/dev/sdC0/fs
boot
EOF
grub-mkimage -O i386-pc -o "$TMP/core.img" -c "$TMP/early.cfg" -p '(hd0,msdos1,plan1)/' \
	biosdisk part_msdos part_plan fat multiboot boot
python3 - "$IMG" "$GRUBDIR/boot.img" "$TMP/core.img" <<'PY'
import struct, sys
img, bootimg, coreimg = sys.argv[1:]
boot = bytearray(open(bootimg, 'rb').read())
core = bytearray(open(coreimg, 'rb').read())
with open(img, 'r+b') as f:
	mbr = bytearray(f.read(512))
	# first partition's start: the gap must hold core.img
	start = struct.unpack('<I', mbr[446+8:446+12])[0]
	nsec = (len(core) + 511) // 512
	if 1 + nsec > start:
		sys.exit('core.img: %d sectors, partition starts at %d' % (nsec, start))
	# boot.img: code before the BPB area and up to the partition table;
	# it loads sector 1 (offset 0x5c) from the BIOS boot drive (0x64)
	mbr[0:3] = boot[0:3]
	mbr[0x5a:446] = boot[0x5a:446]
	# diskboot.img (core.img's first sector) loads the rest:
	# block list at 0x1f4 -- start LBA (8 bytes), sectors, segment
	struct.pack_into('<QH', core, 0x1f4, 2, nsec - 1)
	f.seek(0)
	f.write(mbr)
	f.seek(512)
	f.write(core)
PY

case $OUT in
*.qcow2)	qemu-img convert -O qcow2 "$IMG" "$OUT" && rm -f "$IMG" ;;
*)	mv "$IMG" "$OUT" ;;
esac
echo "ok: $OUT"
