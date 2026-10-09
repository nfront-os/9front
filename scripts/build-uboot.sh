#!/bin/bash
# Build the u-boot firmware 9front's arm64 QEMU image boots from
# (sys/lib/dist/mkfile, %.arm64.qcow2: boot.scr, plan9.ini and 9qemu.u
# on the FAT partition; u-boot runs boot.scr, sys/src/boot/qemu/boot.txt).
# u-boot is not part of 9front: the official release is fetched from
# ftp.denx.de into its own directory and built for qemu_arm64 with the
# port's arm64 cross compiler.
#
# usage: scripts/build-uboot.sh	-> $CLI_DIR/uboot/u-boot.bin
#   env:  UBOOT	release (default 2026.07)
#         CROSS	cross compiler prefix (default aarch64-linux-musl-)
#   needs: curl, make, gcc, bison, flex, python3 (host side of u-boot's build)
set -e
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
UBOOT=${UBOOT:-2026.07}
CROSS=${CROSS:-aarch64-linux-musl-}
D=$CLI_DIR/uboot
OUT=$D/u-boot.bin
if [ -s "$OUT" ] && [ "$(cat "$D/version" 2>/dev/null)" = "$UBOOT" ]; then
	echo "ok: $OUT (u-boot $UBOOT)"
	exit 0
fi
mkdir -p "$D/src"
T=$D/u-boot-$UBOOT.tar.bz2
[ -s "$T" ] || curl -fsSL -o "$T" "https://ftp.denx.de/pub/u-boot/u-boot-$UBOOT.tar.bz2"
rm -rf "$D/src/u-boot-$UBOOT"
tar -xjf "$T" -C "$D/src"
cd "$D/src/u-boot-$UBOOT"
make CROSS_COMPILE=$CROSS qemu_arm64_defconfig > "$D/build.log" 2>&1
make CROSS_COMPILE=$CROSS -j"$(nproc)" >> "$D/build.log" 2>&1 \
	|| { tail -20 "$D/build.log"; exit 1; }
cp u-boot.bin "$OUT"
echo "$UBOOT" > "$D/version"
sha256sum "$T" | cut -d' ' -f1 > "$D/tarball.sha256"
echo "ok: $OUT (u-boot $UBOOT)"
