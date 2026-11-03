#!/bin/bash
# Cross-build Mesa's Vulkan drivers for 9front with ../9front-gcc: venus
# (virtio-gpu), hasvk (Intel gen 7-8) and RADV (AMD, libdrm_amdgpu over
# gpu(4): plan9/drm/amdgpu9.c); and OpenGL (ES) on them: zink, EGL (the
# plan9 platform: draw(3) windows; and surfaceless), libGLESv2's entry points, in the same objects
# (one copy of Mesa's common code for both). libmesaplan9.a (sys/src/mesa/
# plan9: what APE lacks), libdrm.a (plan9/drm: libdrm's interface over
# gpu(4), for hasvk, found by pkg-config), a meson cross file, meson
# setup (once) and ninja in $BUILD; the Intel shaders need mesa_clc on
# the build machine (clc.sh). 9front has no shared libraries: instead
# of the ICDs (libvulkan_virtio.so, libvulkan_intel_hasvk.so) the
# objects and static libraries they would link become one relocatable
# object (each driver's vk_icdGetInstanceProcAddr has a name of its own
# on 9front, the rest of Mesa is there once).
# The SDK in $BUILD/sdk is what Vulkan programs build with: the headers
# and libvulkan.a, the static loader (sys/src/mesa/plan9/loader) with
# the drivers and libmesaplan9 in it; programs link -lvulkan -ldraw -l9
# -lfmt -lutf.
# Extra arguments go to ninja.
set -e
. "$(dirname "$0")/common.sh"
[ -d "$SRC/.git" ] || "$(dirname "$0")/prepare.sh"
[ -x "$WORK/clc/bin/mesa_clc" ] || "$(dirname "$0")/clc.sh"
export PATH=$WORK/clc/bin:$VENV/bin:$PATH
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

# libdrm for 9front, with Mesa's uAPI headers, and its pkg-config file
# and libdrm_amdgpu's, for RADV (drm9.c: Intel's and the descriptors;
# amdgpu9.c: libdrm_amdgpu and AMD's ioctls)
for c in drm9 amdgpu9; do
	"$CROSS/$TRIPLE-gcc" -std=c11 -O2 -Wall -I"$P9/drm/include" -I"$P9/include" -I"$SRC/include" \
		-c -o "$BUILD/plan9/$c.o" "$P9/drm/$c.c"
done
rm -f "$BUILD/plan9/libdrm.a"
"$CROSS/$TRIPLE-ar" rcs "$BUILD/plan9/libdrm.a" "$BUILD/plan9/drm9.o" "$BUILD/plan9/amdgpu9.o"
mkdir -p "$BUILD/plan9/pkgconfig"
cat > "$BUILD/plan9/pkgconfig/libdrm.pc" <<EOC
Name: libdrm
Description: libdrm's interface over gpu(4), 9front (sys/src/mesa/plan9/drm)
Version: 2.4.124
Cflags: -I$P9/drm/include
Libs: -L$BUILD/plan9 -ldrm
EOC
cat > "$BUILD/plan9/pkgconfig/libdrm_amdgpu.pc" <<EOC
Name: libdrm_amdgpu
Description: libdrm_amdgpu's interface over gpu(4), 9front (sys/src/mesa/plan9/drm)
Version: 2.4.134
Requires: libdrm
Cflags: -I$P9/drm/include
Libs: -L$BUILD/plan9 -ldrm
EOC

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
pkg-config = 'pkg-config'

[host_machine]
system = 'plan9'
cpu_family = '$CPU'
cpu = '$CPU'
endian = 'little'

[properties]
needs_exe_wrapper = true
pkg_config_libdir = ['$BUILD/plan9/pkgconfig']

[built-in options]
c_args = [${args%, }]
cpp_args = [${args%, }]
c_link_args = [${links%, }]
cpp_link_args = [${links%, }]
EOC

DRIVERS=virtio,intel_hasvk,amd
GL="-Dgallium-drivers=zink -Dopengl=true -Degl=enabled -Dgles2=enabled"
if [ ! -f "$BUILD/mesa/build.ninja" ]; then
	meson setup "$BUILD/mesa" "$SRC" --cross-file "$BUILD/cross.ini" \
		-Dplatforms=plan9 -Dvulkan-drivers=$DRIVERS $GL \
		-Dglx=disabled -Dgbm=disabled -Dgles1=disabled \
		-Dllvm=disabled -Dmesa-clc=system -Dzstd=disabled -Dexpat=disabled -Dxmlconfig=disabled \
		-Dvalgrind=disabled -Dlibunwind=disabled -Dbuild-tests=false -Dtools= \
		-Dvulkan-layers= -Ddefault_library=static -Dshader-cache=disabled
elif [ "$(python3 -c 'import json, sys; print(",".join(*[o["value"] for o in json.load(open(sys.argv[1])) if o["name"] == "vulkan-drivers"]))' "$BUILD/mesa/meson-info/intro-buildoptions.json")" != $DRIVERS ]; then
	# a build made before a driver was added (RADV: 2026-11)
	meson configure "$BUILD/mesa" -Dvulkan-drivers=$DRIVERS
fi
if [ "$(python3 -c 'import json, sys; print(",".join(*[o["value"] for o in json.load(open(sys.argv[1])) if o["name"] == "gallium-drivers"]))' "$BUILD/mesa/meson-info/intro-buildoptions.json")" != zink ]; then
	# a build made before zink (2026-11)
	meson configure "$BUILD/mesa" $GL
fi
if [ "$(python3 -c 'import json, sys; print(",".join(*[o["value"] for o in json.load(open(sys.argv[1])) if o["name"] == "platforms"]))' "$BUILD/mesa/meson-info/intro-buildoptions.json")" != plan9 ]; then
	# a build made before EGL's plan9 platform (2026-11)
	meson configure "$BUILD/mesa" -Dplatforms=plan9
fi
# everything the ICDs are made of, without linking them (build.ninja
# first: meson regenerates it when a meson.build changed): their
# objects, each once (members.py), in one archive
# hasvk first, then RADV: their objects stand for venus' equal ones (members.py)
SOS="src/intel/vulkan_hasvk/libvulkan_intel_hasvk.so src/amd/vulkan/libvulkan_radeon.so src/virtio/vulkan/libvulkan_virtio.so"
# OpenGL: EGL, the GLES 2 entry points, gallium with zink and the GL core
SOS="$SOS src/egl/libEGL.so.1.0.0 src/mesa/glapi/es2api/libGLESv2.so.2.0.0 src/gallium/targets/dri/libgallium-$MESA_VERSION.so"
ninja -C "$BUILD/mesa" build.ninja
for so in $SOS; do
	link=$(ninja -C "$BUILD/mesa" -t commands $so | tail -1)
	ninja -C "$BUILD/mesa" $(echo "$link" | tr ' ' '\n' | grep '\.[oa]$' | grep -v '^/') "$@"
done
objs=$(python3 "$(dirname "$0")/members.py" "$BUILD/mesa" "$CROSS/$TRIPLE-nm" $SOS)
rm -f "$BUILD/libvulkan_mesa.a"
(cd "$BUILD/mesa" && "$CROSS/$TRIPLE-ar" rcS "$BUILD/libvulkan_mesa.a" $objs && "$CROSS/$TRIPLE-ranlib" "$BUILD/libvulkan_mesa.a")

# the SDK: the loader's trampolines from Mesa's (patched) vk.xml; the
# drivers as one relocatable object, so that the loader's reference to
# vk_icdNegotiateLoaderICDInterfaceVersion brings all of it (the
# drivers' entry points are weak references, which do not take members
# out of an archive)
SDK=$BUILD/sdk
L=$BUILD/loader
mkdir -p "$L" "$SDK/lib" "$SDK/include"
python3 "$MESA/plan9/loader/genloader.py" "$SRC/src/vulkan/registry/vk.xml" "$L/vkcmds.c"
for c in "$L/vkcmds.c" "$MESA/plan9/loader/vkloader.c"; do
	"$CROSS/$TRIPLE-gcc" -std=c11 -O2 -Wall -ffunction-sections -I"$SRC/include" \
		-I"$L" -I"$MESA/plan9/loader" -c -o "$L/$(basename "$c" .c).o" "$c"
done
# (ld, not gcc -r: gcc adds plan9.ld, which would merge the sections
# of all functions into one .text that --gc-sections cannot thin)
"$CROSS/$TRIPLE-ld" -r --no-warn-execstack -o "$L/mesa.o" \
	--whole-archive "$BUILD/libvulkan_mesa.a" --no-whole-archive
rm -f "$SDK/lib/libvulkan.a"
"$CROSS/$TRIPLE-ar" rcs "$SDK/lib/libvulkan.a" "$L/vkloader.o" "$L/vkcmds.o" "$L/mesa.o" \
	"$BUILD"/plan9/*.o
rm -rf "$SDK/include"/*
cp -r "$SRC/include/vulkan" "$SRC/include/vk_video" "$SRC/include/EGL" "$SRC/include/GLES2" "$SRC/include/GLES3" "$SRC/include/KHR" "$SDK/include/"
echo "build: $BUILD/libvulkan_mesa.a $BUILD/plan9/libmesaplan9.a $SDK"
