#!/bin/bash
# Build libffi and libwayland (build.exe -D: sys/src/libffi, sys/src/wayland) and
# wlsys (sys/src/cmd/wlsys, the Wayland server in wsys) with ../9front-gcc's cross
# gcc; wlsys goes to outdir. Not part of the full build yet (as the libraries).
# usage: scripts/wayland/build.sh outdir   (amd64)
set -e
[ $# -eq 1 ] || { echo "usage: scripts/wayland/build.sh outdir" >&2; exit 1; }
ROOT=$(git -C "$(dirname "${BASH_SOURCE[0]}")" rev-parse --show-toplevel)
. "$ROOT/scripts/mesa/common.sh"
LOG=${TMPDIR:-/tmp}/wayland-build.$$.log
for m in sys/src/libffi/build.json sys/src/wayland/build.json; do
	CC=$CROSS/$TRIPLE-gcc ARCH=amd64 "$ROOT/sys/src/cmd/build/build.exe" -D "$ROOT/$m" > "$LOG" 2>&1 ||
		{ tail -20 "$LOG"; rm -f "$LOG"; exit 1; }
done
rm -f "$LOG"
mkdir -p "$1"
W=$ROOT/sys/src/cmd/wlsys
"$CROSS/$TRIPLE-gcc" -std=gnu11 -O2 -Wall -o "$1/wlsys" \
	"$W/wlsys.c" "$W/shm.c" "$W/xdg.c" "$W/win.c" "$W/seat.c" "$W/keymap.c" "$W/dmabuf.c" \
	"$W/data.c" "$W/deco.c" "$W/sub.c" "$W/dnd.c" \
	-lwayland-server -lffi
