#!/bin/bash
# Build tests/gpu/vktest (POSIX, APE) against Mesa's venus as
# scripts/mesa/build.sh leaves it: root/tests/vktest.
set -e
. "$(dirname "$0")/../../scripts/mesa/common.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
[ -f "$BUILD/libvulkan_venus.a" ] || "$ROOT/scripts/mesa/build.sh"
mkdir -p "$HERE/root/tests"
"$CROSS/$TRIPLE-gcc" -std=c11 -O2 -Wall -I"$SRC/include" -c -o "$BUILD/vktest.o" "$HERE/vktest.c"
"$CROSS/$TRIPLE-gcc" -std=c11 -O2 -Wall -c -o "$BUILD/btassert.o" "$HERE/btassert.c"
"$CROSS/$TRIPLE-g++" -o "$HERE/root/tests/vktest" "$BUILD/vktest.o" "$BUILD/btassert.o" \
	-Wl,--gc-sections -Wl,--whole-archive "$BUILD/libvulkan_venus.a" -Wl,--no-whole-archive -L"$BUILD/plan9" -lmesaplan9 -l9 -Wl,--start-group -lap -lbsd -Wl,--end-group
ls -la "$HERE/root/tests/vktest"
