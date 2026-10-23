#!/bin/bash
# mesa_clc and vtn_bindgen2 for the build machine (Linux): the Intel
# drivers' shaders are OpenCL C, which mesa_clc turns into SPIR-V and
# vtn_bindgen2 into C++ at build time. A native meson build of the same
# Mesa in $WORK/build-native-clc, the two programs linked into
# $WORK/clc/bin, which build.sh puts first on the PATH
# (-Dmesa-clc=system). Needs LLVM, clang, SPIRV-Tools and
# spirv-llvm-translator on the host.
set -e
. "$(dirname "$0")/common.sh"
[ -d "$SRC/.git" ] || "$(dirname "$0")/prepare.sh"
export PATH=$VENV/bin:$PATH
N=$WORK/build-native-clc
if [ ! -f "$N/build.ninja" ]; then
	meson setup "$N" "$SRC" -Dbuildtype=release -Dmesa-clc=enabled -Dinstall-mesa-clc=true \
		-Dllvm=enabled -Dplatforms= -Dvulkan-drivers= -Dgallium-drivers= -Dopengl=false \
		-Degl=disabled -Dglx=disabled -Dgbm=disabled -Dgles1=disabled -Dgles2=disabled \
		-Dzstd=disabled -Dxmlconfig=disabled -Dvalgrind=disabled -Dlibunwind=disabled \
		-Dbuild-tests=false -Dtools= -Dvulkan-layers= -Dshader-cache=disabled
fi
ninja -C "$N" src/compiler/clc/mesa_clc src/compiler/spirv/vtn_bindgen2
mkdir -p "$WORK/clc/bin"
ln -sf "$N/src/compiler/clc/mesa_clc" "$N/src/compiler/spirv/vtn_bindgen2" "$WORK/clc/bin/"
echo "clc: $WORK/clc/bin"
