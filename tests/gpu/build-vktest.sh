#!/bin/bash
# Build the Vulkan tests of tests/gpu (POSIX, APE) into root/tests, against
# Mesa as the tree has it: build.exe makes /$objtype/lib.gcc/ape/libvulkan.a
# from sys/src/mesa/build.json (the vendored drivers: venus, hasvk, RADV; the static loader,
# libmesaplan9; scripts/mesa/vendor.sh), the headers are in /sys/include/ape.
# Programs link -lvulkan, and APE's libdraw and lib9 for the plan9 WSI.
# vktest and vkdraw call the driver's vk_icdGetInstanceProcAddr, the others
# are ordinary Vulkan programs; vktri's shaders are compiled with glslc.
# The OpenGL tests (gltest, gldesk, glwin: zink, EGL) link the same library.
set -e
. "$(dirname "$0")/../../scripts/mesa/common.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
mkdir -p "$HERE/root/tests" "$BUILD/tests"
O=$BUILD/tests
CC=$CROSS/$TRIPLE-gcc ARCH=$objtype "$ROOT/sys/src/cmd/build/build.exe" -D "$ROOT/sys/src/mesa/build.json" \
	> "$O/libvulkan.log" 2>&1 || { tail -20 "$O/libvulkan.log"; exit 1; }
libs="-Wl,--gc-sections -lvulkan -ldraw -l9 -lfmt -lutf"
"$CROSS/$TRIPLE-gcc" -std=c11 -O2 -Wall -c -o "$O/btassert.o" "$HERE/btassert.c"
"$CROSS/$TRIPLE-gcc" -std=c11 -O2 -Wall -D_PLAN9_SOURCE -I"$ROOT/sys/src/ape/lib/9" \
	-c -o "$O/screenpeek.o" "$HERE/screenpeek.c"
for s in tri.vert tri.frag; do
	glslc -O -mfmt=c -o "$O/$s.inc" "$HERE/shaders/$s"
done
for p in vktest vkdraw vktri vkspeed radvtest gltest gldesk glwin; do
	extra=
	case $p in
	vkdraw|vktri)	extra="$O/btassert.o $O/screenpeek.o" ;;
	vktest)		extra=$O/btassert.o ;;
	esac
	"$CROSS/$TRIPLE-gcc" -std=gnu11 -O2 -Wall -I"$O" -c -o "$O/$p.o" "$HERE/$p.c"
	"$CROSS/$TRIPLE-g++" -o "$HERE/root/tests/$p" "$O/$p.o" $extra $libs
done
# rsvk: vktri in Rust (ash, ../9front-ui stage 1b), built by cargo with ../9front-rust's
# stage-1 rustc for x86_64-unknown-plan9 and linked by the cross gcc above (the view of this
# tree: libp9, APE, libvulkan.a); the crates are in rsvk/vendor (no network). Skipped
# without that rustc.
RUST_DIR=${RUST_DIR:-$ROOT/../9front-rust}
RUSTC=${RUSTC:-$RUST_DIR/rust/build/host/stage1/bin/rustc}
if [ "$objtype" = amd64 ] && [ -x "$RUSTC" ] && command -v cargo >/dev/null; then
	for s in tri.vert tri.frag; do
		glslc -O -o "$O/$s.spv" "$HERE/shaders/$s"
	done
	"$CROSS/$TRIPLE-ar" rcs "$O/libscreenpeek.a" "$O/screenpeek.o"
	T=x86_64-unknown-plan9
	(cd "$HERE/rsvk" &&
	 RUSTC="$RUSTC" RSVK_SPV="$O" CARGO_TARGET_DIR="$O/rsvk" \
	 CARGO_TARGET_X86_64_UNKNOWN_PLAN9_LINKER="$CROSS/$TRIPLE-gcc" \
	 RUSTFLAGS="-L $O -l screenpeek -l draw -l 9 -l fmt -l utf -l stdc++ -C link-arg=-Wl,--gc-sections" \
	 cargo build --offline --release --target $T)
	cp "$O/rsvk/$T/release/rsvk" "$HERE/root/tests/rsvk"
else
	echo "rsvk: not built (objtype=$objtype, or no stage-1 rustc $RUSTC or cargo)" >&2
fi
# Khronos' vkcube and vulkaninfo, ported (scripts/mesa/vktools.sh)
"$ROOT/scripts/mesa/vktools.sh" build
cp "$BUILD/vktools/vkcube" "$BUILD/vktools/vulkaninfo" "$HERE/root/tests/"
ls -la "$HERE/root/tests/vktest" "$HERE/root/tests/vkdraw" "$HERE/root/tests/vktri" \
	"$HERE/root/tests/vkspeed" "$HERE/root/tests/radvtest" "$HERE/root/tests/vkcube" "$HERE/root/tests/vulkaninfo" \
	"$HERE/root/tests/gltest" "$HERE/root/tests/gldesk" "$HERE/root/tests/glwin"
