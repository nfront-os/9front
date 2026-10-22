#!/bin/bash
# Build tests/gpu/vktest and vkdraw (POSIX, APE) against Mesa's venus as
# scripts/mesa/build.sh leaves it: root/tests/vktest, root/tests/vkdraw.
# The archive whole (venus' entry points are weak references), the
# unused sections dropped; APE's libdraw and lib9 for the plan9 WSI.
set -e
. "$(dirname "$0")/../../scripts/mesa/common.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
[ -f "$BUILD/libvulkan_venus.a" ] || "$ROOT/scripts/mesa/build.sh"
mkdir -p "$HERE/root/tests"
"$CROSS/$TRIPLE-gcc" -std=c11 -O2 -Wall -c -o "$BUILD/btassert.o" "$HERE/btassert.c"
for p in vktest vkdraw; do
	"$CROSS/$TRIPLE-gcc" -std=c11 -O2 -Wall -I"$SRC/include" -c -o "$BUILD/$p.o" "$HERE/$p.c"
	"$CROSS/$TRIPLE-g++" -o "$HERE/root/tests/$p" "$BUILD/$p.o" "$BUILD/btassert.o" \
		-Wl,--gc-sections -Wl,--whole-archive "$BUILD/libvulkan_venus.a" -Wl,--no-whole-archive \
		-L"$BUILD/plan9" -lmesaplan9 -ldraw -l9 -lfmt -lutf
done
ls -la "$HERE/root/tests/vktest" "$HERE/root/tests/vkdraw"
