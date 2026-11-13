#!/bin/bash
# Make the hardware test stick image (tests/hw/README.md): a 9front disk
# (9fat with 9bootfat, plan9.ini, the kernel and hwtest/ for the logs;
# hjfs root with the system, the test runner, its categories and the
# guest test programs) that boots by itself from a USB stick and runs
# tests/hw/hwtest.rc. Built in a QEMU guest like scripts/guest/mkdisk.sh.
#
# usage: scripts/hw/mkstick.sh [image]	(default $CLI_DIR/hwstick.amd64.img)
#   env:	CONFIGS	the configurations (default tests/hw/configs)
#	CONSOLE	1: plan9.ini gets console=0 (serial; for tests/hw/run.sh)
#	MB	image size in MB (3770, fits a 4 GB stick)
#	PACK	1: also a hand-off dir for another (Linux) machine next to
#		the image: the image packed (zstd, else xz), collect.py and
#		HOWTO.txt (written to the stick with dd)
# Needs the build (CC=gcc ARCH=amd64 sys/src/cmd/build/build.exe) and
# scripts/guest/build-boot.sh. Write it to a stick with dd (HOWTO.txt of PACK=1).
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
. "$ROOT/scripts/lib/paths.sh"	# CLI_DIR, CACHE (dist/paths.env: nfront/cache/cli)
OUT=${1:-$CLI_DIR/hwstick.amd64.img}
MB=${MB:-3770}
CONFIGS=${CONFIGS:-$ROOT/tests/hw/configs}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
die(){ echo "mkstick: $*" >&2; exit 1; }
for f in mbr pbs 9bootfat; do
	[ -s "$ROOT/386/$f" ] || die "no 386/$f, run scripts/guest/build-boot.sh"
done
[ -f "$CONFIGS" ] || die "no $CONFIGS"

echo "build the guest test programs"
(cd "$ROOT/tests/guest" && env PLAN9="$ROOT" ARCH=amd64 CC=gcc "$ROOT/sys/src/cmd/build/build.exe" build.json) \
	> "$TMP/build.log" 2>&1 || { tail -20 "$TMP/build.log"; die "tests/guest build failed"; }
install -m 755 "$ROOT/tests/guest/run.rc" "$ROOT/tests/guest/root/tests/run"
for t in "$ROOT"/tests/*/build.json; do
	case $t in */guest/*) continue ;; esac
	[ -f "$(dirname "$t")/hwstick" ] || continue	# a test dir that wants its programs on the stick
	(cd "$(dirname "$t")" && env PLAN9="$ROOT" ARCH=amd64 CC=gcc "$ROOT/sys/src/cmd/build/build.exe" build.json) \
		> "$TMP/build.log" 2>&1 || { tail -20 "$TMP/build.log"; die "$t build failed"; }
done

echo "stage the distribution and the tests"
bash "$ROOT/scripts/guest/stage-dist.sh" "$CLI_DIR/src9"
H=$CLI_DIR/src9/hwtest
rm -rf "$H"
mkdir -p "$H/cat" "$H/tests"
cp "$ROOT/tests/hw/hwtest.rc" "$ROOT/tests/hw/termrc" "$ROOT/tests/hw/README.txt" "$H/"
cp "$CONFIGS" "$H/configs"
cp "$ROOT"/tests/hw/cat/* "$H/cat/"
cp -r "$ROOT"/tests/guest/root/tests/. "$H/tests/"
for d in "$ROOT"/tests/*/root/tests; do
	case $d in */guest/*) continue ;; esac
	[ -d "$d" ] && cp -r "$d"/. "$H/tests/"
done
[ "${CONSOLE:-0}" = 1 ] && echo 'console=0' > "$H/console"

echo "install onto an empty disk in the guest"
IMG=$CLI_DIR/mkstick.raw
rm -f "$IMG"
truncate -s ${MB}M "$IMG"
mkdir -p "$TMP/extra/rc/bin"
cp "$ROOT/scripts/guest/boot/mkstick.rc" "$TMP/extra/rc/bin/mkstick"
SRC9=$CLI_DIR/src9 DISK=$IMG UFS_EXTRA=$TMP/extra MEM=${MEM:-1024} \
WAITFOR='mkstick: (done|failed)' WAITMAX=${WAITMAX:-7200} CMDWAIT=5 \
	bash "$ROOT/scripts/guest/boot-cli.sh" 'mkstick' > "$CLI_DIR/mkstick.log" 2>&1 || true
grep -q '^mkstick: done' "$CLI_DIR/mkstick.log" \
	|| { tail -30 "$CLI_DIR/mkstick.log"; die "installation failed (log: $CLI_DIR/mkstick.log)"; }
mv "$IMG" "$OUT"
echo "ok: $OUT"
[ "${PACK:-0}" = 1 ] || exit 0
HAND=${OUT%.img}-handoff
rm -rf "$HAND"
mkdir -p "$HAND"
b=$(basename "$OUT")
if command -v zstd >/dev/null; then
	zstd -q -T0 -10 -o "$HAND/$b.zst" "$OUT"; b=$b.zst
else
	xz -T0 -6 -c "$OUT" > "$HAND/$b.xz"; b=$b.xz
fi
cp "$ROOT/scripts/hw/collect.py" "$HAND/"
cat > "$HAND/HOWTO.txt" <<HOWTO
9front hardware test stick ($(date -I), $(git -C "$ROOT" describe --always --dirty))

Write it to the stick (check the device with lsblk first: dd overwrites it):
	$(case $b in *.zst) echo zstd ;; *) echo xz ;; esac) -dc $b | sudo dd of=/dev/sdX bs=4M iflag=fullblock conv=fsync status=progress

Boot the laptop from the stick (BIOS/legacy boot); it runs the tests by
itself and halts at the end (see hwtest/README.txt on the stick).

Read the logs back (python3 and mtools):
	python3 collect.py /dev/sdX [dir]
or mount the stick's 9fat:
	sudo mount -o loop,ro,offset=\$(python3 collect.py --offset /dev/sdX) /dev/sdX /mnt
HOWTO
echo "ok: $HAND ($(du -sh "$HAND" | cut -f1))"
