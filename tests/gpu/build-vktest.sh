#!/bin/bash
# Build tests/gpu/vktest and vkdraw (POSIX, APE) against Mesa's venus as
# scripts/mesa/build.sh leaves it: root/tests/vktest, root/tests/vkdraw.
# The archive whole (venus' entry points are weak references), the
# unused sections dropped; APE's libdraw and lib9 for the plan9 WSI.
# vktri is an ordinary Vulkan program: the SDK's headers and
# -lvulkan (the static loader), its shaders compiled with glslc.
set -e
. "$(dirname "$0")/../../scripts/mesa/common.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
[ -f "$BUILD/libvulkan_venus.a" ] || "$ROOT/scripts/mesa/build.sh"
mkdir -p "$HERE/root/tests"
"$CROSS/$TRIPLE-gcc" -std=c11 -O2 -Wall -c -o "$BUILD/btassert.o" "$HERE/btassert.c"
"$CROSS/$TRIPLE-gcc" -std=c11 -O2 -Wall -D_PLAN9_SOURCE -I"$ROOT/sys/src/ape/lib/9" \
	-c -o "$BUILD/screenpeek.o" "$HERE/screenpeek.c"
for p in vktest vkdraw; do
	extra=
	[ $p = vkdraw ] && extra=$BUILD/screenpeek.o
	"$CROSS/$TRIPLE-gcc" -std=c11 -O2 -Wall -I"$SRC/include" -c -o "$BUILD/$p.o" "$HERE/$p.c"
	"$CROSS/$TRIPLE-g++" -o "$HERE/root/tests/$p" "$BUILD/$p.o" "$BUILD/btassert.o" $extra \
		-Wl,--gc-sections -Wl,--whole-archive "$BUILD/libvulkan_venus.a" -Wl,--no-whole-archive \
		-L"$BUILD/plan9" -lmesaplan9 -ldraw -l9 -lfmt -lutf
done
SDK=$BUILD/sdk
for s in tri.vert tri.frag; do
	glslc -O -mfmt=c -o "$BUILD/$s.inc" "$HERE/shaders/$s"
done
"$CROSS/$TRIPLE-gcc" -std=c11 -O2 -Wall -I"$SDK/include" -I"$BUILD" -c -o "$BUILD/vktri.o" "$HERE/vktri.c"
"$CROSS/$TRIPLE-g++" -o "$HERE/root/tests/vktri" "$BUILD/vktri.o" "$BUILD/btassert.o" "$BUILD/screenpeek.o" \
	-Wl,--gc-sections -L"$SDK/lib" -lvulkan -ldraw -l9 -lfmt -lutf
ls -la "$HERE/root/tests/vktest" "$HERE/root/tests/vkdraw" "$HERE/root/tests/vktri"
