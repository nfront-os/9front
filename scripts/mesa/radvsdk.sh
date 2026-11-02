#!/bin/bash
# RADV for 9front, from the meson build in $BUILD/radv (scripts/mesa: the
# cross build of Mesa with -Dvulkan-drivers=amd): libvulkan.a with RADV
# (linked into the program, the static loader), and vulkaninfo, vkcube and
# tests/gpu/radvtest linked with it into tests/gpu/root/tests (vulkaninfo-radv,
# vkcube-radv, radvtest).  Not yet part of sys/src/mesa/build.json.
set -e
. "$(dirname "$0")/common.sh"
R=$BUILD/radv
export PATH=$WORK/clc/bin:$VENV/bin:$PATH
P9=$MESA/plan9
O=$R/plan9
# the meson build of RADV, made once: its own cross file, and libdrm and
# libdrm_amdgpu as pkg-config sees them (sys/src/mesa/plan9/drm)
if [ ! -f "$R/mesa/build.ninja" ]; then
	mkdir -p "$R/pkgconfig"
	cat > "$R/pkgconfig/libdrm.pc" <<EOC
Name: libdrm
Description: libdrm's interface over gpu(4), 9front (sys/src/mesa/plan9/drm)
Version: 2.4.124
Cflags: -I$P9/drm/include
Libs: -L$BUILD/plan9 -ldrm
EOC
	cat > "$R/pkgconfig/libdrm_amdgpu.pc" <<EOC
Name: libdrm_amdgpu
Description: libdrm_amdgpu's interface over gpu(4), 9front (sys/src/mesa/plan9/drm)
Version: 2.4.134
Requires: libdrm
Cflags: -I$P9/drm/include
Libs: -L$BUILD/plan9 -ldrm
EOC
	cat > "$R/cross.ini" <<EOC
[binaries]
c = '$CROSS/$TRIPLE-gcc'
cpp = '$CROSS/$TRIPLE-g++'
ar = '$CROSS/$TRIPLE-ar'
strip = '$CROSS/$TRIPLE-strip'
pkg-config = 'pkg-config'

[host_machine]
system = 'plan9'
cpu_family = 'x86_64'
cpu = 'x86_64'
endian = 'little'

[properties]
needs_exe_wrapper = true
pkg_config_libdir = ['$R/pkgconfig']

[built-in options]
c_args = ['-I$P9/include', '-DHAVE_ENDIAN_H', '-DNO_REGEX', '-fno-strict-aliasing']
cpp_args = ['-I$P9/include', '-DHAVE_ENDIAN_H', '-DNO_REGEX', '-fno-strict-aliasing']
c_link_args = ['-L$BUILD/plan9', '-lmesaplan9']
cpp_link_args = ['-L$BUILD/plan9', '-lmesaplan9']
EOC
	meson setup "$R/mesa" "$SRC" --cross-file "$R/cross.ini" \
		-Dplatforms= -Dvulkan-drivers=amd -Dgallium-drivers= -Dopengl=false -Degl=disabled \
		-Dglx=disabled -Dgbm=disabled -Dgles1=disabled -Dgles2=disabled -Dllvm=disabled \
		-Dmesa-clc=system -Dzstd=disabled -Dexpat=disabled -Dxmlconfig=disabled \
		-Dvalgrind=disabled -Dlibunwind=disabled -Dbuild-tests=false -Dtools= \
		-Dvulkan-layers= -Ddefault_library=static -Dshader-cache=disabled > "$R/setup.log" 2>&1 ||
		{ tail -20 "$R/setup.log"; exit 1; }
fi
mkdir -p $O
for c in $P9/lib/*.c; do
	o=$O/$(basename $c .c).o
	case $c in
	*/plan9win.c)	flags="-D_PLAN9_SOURCE -I$ROOT/sys/src/ape/lib/9 -I$P9/include" ;;
	*)		flags="-I$P9/include" ;;
	esac
	$CROSS/$TRIPLE-gcc -std=c11 -O2 -Wall $flags -c -o $o $c
done
for c in drm9 amdgpu9; do
	$CROSS/$TRIPLE-gcc -std=c11 -O2 -Wall -I$P9/drm/include -I$P9/include -I$SRC/include -c -o $O/$c.o $P9/drm/$c.c
done
SO=src/amd/vulkan/libvulkan_radeon.so
cd $R/mesa
link=$(ninja -t commands $SO | tail -1)
ninja $(echo "$link" | tr ' ' '\n' | grep '\.[oa]$' | grep -v '^/') >/dev/null
objs=$(python3 $ROOT/scripts/mesa/members.py $R/mesa $CROSS/$TRIPLE-nm $SO)
rm -f $R/libvulkan_mesa.a
$CROSS/$TRIPLE-ar rcS $R/libvulkan_mesa.a $objs && $CROSS/$TRIPLE-ranlib $R/libvulkan_mesa.a
L=$R/loader
mkdir -p $L $R/sdk/lib
python3 $P9/loader/genloader.py $SRC/src/vulkan/registry/vk.xml $L/vkcmds.c
for c in $L/vkcmds.c $P9/loader/vkloader.c; do
	$CROSS/$TRIPLE-gcc -std=c11 -O2 -Wall -ffunction-sections -I$SRC/include -I$L -I$P9/loader -c -o $L/$(basename $c .c).o $c
done
$CROSS/$TRIPLE-ld -r --no-warn-execstack -o $L/mesa.o --whole-archive $R/libvulkan_mesa.a --no-whole-archive
rm -f $R/sdk/lib/libvulkan.a
$CROSS/$TRIPLE-ar rcs $R/sdk/lib/libvulkan.a $L/vkloader.o $L/vkcmds.o $L/mesa.o $O/*.o
V=$BUILD/vktools
T=$ROOT/tests/gpu
for p in vulkaninfo:vulkaninfo cube:vkcube; do
	$CROSS/$TRIPLE-g++ -o $R/${p#*:} $V/${p%:*}.o -Wl,--gc-sections -L$R/sdk/lib -lvulkan -ldraw -l9 -lfmt -lutf
done
$CROSS/$TRIPLE-gcc -std=gnu11 -O2 -Wall -I$SRC/include -c -o $R/radvtest.o $T/radvtest.c
$CROSS/$TRIPLE-g++ -o $R/radvtest $R/radvtest.o -Wl,--gc-sections -L$R/sdk/lib -lvulkan -ldraw -l9 -lfmt -lutf
mkdir -p $T/root/tests
cp $R/vulkaninfo $T/root/tests/vulkaninfo-radv
cp $R/vkcube $T/root/tests/vkcube-radv
cp $R/radvtest $T/root/tests/radvtest
ls -la $T/root/tests/vulkaninfo-radv $T/root/tests/vkcube-radv $T/root/tests/radvtest
