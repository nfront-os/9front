#!/bin/bash
# The device tree of a board qemu emulates without making one itself
# (microchip-icicle-kit): Linux's description of the board, fetched
# from the kernel's git (tag LINUX, default v6.12) into its own
# directory, preprocessed and compiled with cpp and dtc.
#
# usage: scripts/guest/build-dtb.sh board	-> $CLI_DIR/dtb/board.dtb
#   board: icicle (PolarFire SoC Icicle Kit, sys/src/9/riscv64/pf)
#   needs: curl, cpp, dtc
set -e
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
LINUX=${LINUX:-v6.12}
die(){ echo "build-dtb: $*" >&2; exit 1; }
case $1 in
icicle)	dts=arch/riscv/boot/dts/microchip/mpfs-icicle-kit.dts
	files="$dts arch/riscv/boot/dts/microchip/mpfs-icicle-kit-fabric.dtsi
		arch/riscv/boot/dts/microchip/mpfs.dtsi
		include/dt-bindings/clock/microchip,mpfs-clock.h
		include/dt-bindings/interrupt-controller/irq.h
		include/dt-bindings/gpio/gpio.h include/dt-bindings/leds/common.h" ;;
*)	die "usage: build-dtb.sh icicle" ;;
esac
OUT=$CLI_DIR/dtb/$1.dtb
[ -s "$OUT" ] && [ "$(cat "$OUT.version" 2>/dev/null)" = "$LINUX" ] && { echo "ok: $OUT"; exit 0; }
S=$CLI_DIR/dtb/linux-$LINUX
mkdir -p "$S"
for f in $files; do
	[ -s "$S/$f" ] && continue
	mkdir -p "$S/$(dirname "$f")"
	curl -fsSL -o "$S/$f" "https://raw.githubusercontent.com/torvalds/linux/$LINUX/$f" || die "fetch $f"
done
cpp -nostdinc -I "$S/include" -I "$S/$(dirname "$dts")" -undef -x assembler-with-cpp "$S/$dts" > "$OUT.dts"
dtc -q -I dts -O dtb -o "$OUT" "$OUT.dts"
echo "$LINUX" > "$OUT.version"
echo "ok: $OUT"
