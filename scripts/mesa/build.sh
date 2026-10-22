#!/bin/bash
# Cross-build Mesa's venus Vulkan driver for 9front with ../9front-gcc:
# libmesaplan9.a (sys/src/mesa/plan9: what APE lacks), a meson cross file,
# meson setup (once) and ninja in $BUILD. 9front has no shared libraries:
# instead of the ICD (libvulkan_virtio.so) the objects and static
# libraries it would link become one archive, $BUILD/libvulkan_venus.a,
# which a program links with libmesaplan9.a (see tests/gpu/vktest.c).
# Extra arguments go to ninja.
set -e
. "$(dirname "$0")/common.sh"
[ -d "$SRC/.git" ] || "$(dirname "$0")/prepare.sh"
export PATH=$VENV/bin:$PATH
mkdir -p "$BUILD/plan9"
P9=$MESA/plan9

# the platform library; plan9win.c is Plan 9 C over APE's libdraw
# (programs link -ldraw -l9 -lfmt -lutf with it, what #pragma lib names)
objs=
for c in "$P9"/lib/*.c; do
	o=$BUILD/plan9/$(basename "$c" .c).o
	case $c in
	*/plan9win.c)	flags="-D_PLAN9_SOURCE -I$ROOT/sys/src/ape/lib/9 -I$P9/include" ;;
	*)		flags="-I$P9/include" ;;
	esac
	"$CROSS/$TRIPLE-gcc" -std=c11 -O2 -Wall $flags -c -o "$o" "$c"
	objs="$objs $o"
done
rm -f "$BUILD/plan9/libmesaplan9.a"
"$CROSS/$TRIPLE-ar" rcs "$BUILD/plan9/libmesaplan9.a" $objs

q(){ printf "'%s', " "$@"; }
# -fno-strict-aliasing: Vulkan's pNext chains are read through
# VkBaseInStructure; with gcc 16's inlining the venus encoder read a
# struct before its initialisation (vkGetDeviceQueue2: 64 bytes sized,
# 80 encoded, vn_cs.h assertion)
args="$(q "-I$P9/include" -DHAVE_ENDIAN_H -DNO_REGEX -fno-strict-aliasing)"
links="$(q "-L$BUILD/plan9" -lmesaplan9)"
cat > "$BUILD/cross.ini" <<EOC
[binaries]
c = '$CROSS/$TRIPLE-gcc'
cpp = '$CROSS/$TRIPLE-g++'
ar = '$CROSS/$TRIPLE-ar'
strip = '$CROSS/$TRIPLE-strip'
pkg-config = 'false'

[host_machine]
system = 'plan9'
cpu_family = '$CPU'
cpu = '$CPU'
endian = 'little'

[properties]
needs_exe_wrapper = true

[built-in options]
c_args = [${args%, }]
cpp_args = [${args%, }]
c_link_args = [${links%, }]
cpp_link_args = [${links%, }]
EOC

if [ ! -f "$BUILD/mesa/build.ninja" ]; then
	meson setup "$BUILD/mesa" "$SRC" --cross-file "$BUILD/cross.ini" \
		-Dplatforms= -Dvulkan-drivers=virtio -Dgallium-drivers= -Dopengl=false \
		-Degl=disabled -Dglx=disabled -Dgbm=disabled -Dgles1=disabled -Dgles2=disabled \
		-Dllvm=disabled -Dzstd=disabled -Dexpat=disabled -Dxmlconfig=disabled \
		-Dvalgrind=disabled -Dlibunwind=disabled -Dbuild-tests=false -Dtools= \
		-Dvulkan-layers= -Ddefault_library=static -Dshader-cache=disabled
fi
# everything the ICD is made of, without linking the ICD (build.ninja
# first: meson regenerates it when a meson.build changed)
ninja -C "$BUILD/mesa" build.ninja
link=$(ninja -C "$BUILD/mesa" -t commands src/virtio/vulkan/libvulkan_virtio.so | tail -1)
objs=$(echo "$link" | tr ' ' '\n' | grep '\.o$')
libs=$(echo "$link" | tr ' ' '\n' | grep '\.a$' | awk '!seen[$0]++')
ninja -C "$BUILD/mesa" $objs $libs "$@"
rm -f "$BUILD/libvulkan_venus.a"
{
	echo "create $BUILD/libvulkan_venus.a"
	for o in $objs; do echo "addmod $BUILD/mesa/$o"; done
	for a in $libs; do echo "addlib $BUILD/mesa/$a"; done
	echo save
	echo end
} | "$CROSS/$TRIPLE-ar" -M
echo "build: $BUILD/libvulkan_venus.a $BUILD/plan9/libmesaplan9.a"
