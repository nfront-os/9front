#!/bin/bash
# Khronos' Vulkan-Tools for 9front: vkcube and vulkaninfo, built with
# ../9front-gcc against the tree's Vulkan: the headers in /sys/include/ape,
# /$objtype/lib.gcc/ape/libvulkan.a (build.exe sys/src/mesa/build.json).
# Release in sys/src/vktools/VERSION (no signature: the
# sha256 is the check), the port in sys/src/vktools/patches.
#
# usage: vktools.sh [prepare|patches|build]   (default build)
#   prepare  download, unpack to $WORK/src/Vulkan-Tools-<version> as a
#            git repository (tag pristine), apply the patches (git am)
#   patches  write sys/src/vktools/patches from the commits there
#   build    $BUILD/vktools/vkcube, vulkaninfo (prepare first if needed)
set -e
. "$(dirname "$0")/common.sh"
VT=$ROOT/sys/src/vktools
vfield(){ awk -v k="$1" '$1 == k { print $2 }' "$VT/VERSION"; }
VT_VERSION=$(vfield version)
VT_URL=$(vfield url)
VT_SHA256=$(vfield sha256)
VT_SRC=$WORK/src/Vulkan-Tools-$VT_VERSION

prepare(){
	mkdir -p "$WORK/dl"
	f=$WORK/dl/Vulkan-Tools-$VT_VERSION.tar.gz
	[ -f "$f" ] || curl -fsSL -o "$f" "$VT_URL"
	echo "$VT_SHA256  $f" | sha256sum -c - >/dev/null
	rm -rf "$VT_SRC"
	mkdir -p "$WORK/src"
	tar -C "$WORK/src" -xzf "$f"
	cd "$VT_SRC"
	git init -q
	git add -A
	git -c user.name=pristine -c user.email=pristine@localhost commit -q -m "Vulkan-Tools $VT_VERSION"
	git tag pristine
	for p in "$VT"/patches/*.patch; do
		[ -f "$p" ] || continue
		git -c user.name=port -c user.email=port@localhost am -q -3 "$p"
	done
	echo "vktools: $VT_SRC ($(git rev-list --count pristine..HEAD) patches)"
}

patches(){
	cd "$VT_SRC"
	[ -z "$(git status --porcelain)" ] || { echo "vktools: commit the changes in $VT_SRC first" >&2; exit 1; }
	rm -f "$VT"/patches/*.patch
	git format-patch -q --no-signature --zero-commit -N -o "$VT/patches" pristine..HEAD
	ls "$VT/patches"
}

build(){
	[ -d "$VT_SRC/.git" ] || prepare
	CC=$CROSS/$TRIPLE-gcc ARCH=$objtype "$ROOT/sys/src/cmd/build/build.exe" -D \
		"$ROOT/sys/src/mesa/build.json" > "$BUILD/libvulkan.log" 2>&1 ||
		{ tail -20 "$BUILD/libvulkan.log"; exit 1; }
	O=$BUILD/vktools
	mkdir -p "$O"
	flags="-O2 -DVK_USE_PLATFORM_PLAN9_9FRONT"
	# vkcube with Wayland too (--wsi wayland, wlsys(4)): the protocols' headers
	# under the names its CMake gives them (the code is in libwayland-client.a)
	echo '#include <xdg-shell-client-protocol.h>' > "$O/xdg-shell-client-header.h"
	echo '#include <xdg-decoration-unstable-v1-client-protocol.h>' > "$O/xdg-decoration-client-header.h"
	"$CROSS/$TRIPLE-gcc" -std=gnu11 $flags -DVK_USE_PLATFORM_WAYLAND_KHR -I"$O" -I"$VT_SRC/cube" \
		-c -o "$O/cube.o" "$VT_SRC/cube/cube.c"
	"$CROSS/$TRIPLE-g++" -std=c++17 $flags -I"$VT_SRC/vulkaninfo" -I"$VT_SRC/vulkaninfo/generated" \
		-c -o "$O/vulkaninfo.o" "$VT_SRC/vulkaninfo/vulkaninfo.cpp"
	# g++: Mesa has C++ in it
	"$CROSS/$TRIPLE-g++" -o "$O/vkcube" "$O/cube.o" -Wl,--gc-sections \
		-lvulkan -ldraw -l9 -lfmt -lutf -lwayland-client -lffi
	"$CROSS/$TRIPLE-g++" -o "$O/vulkaninfo" "$O/vulkaninfo.o" -Wl,--gc-sections \
		-lvulkan -ldraw -l9 -lfmt -lutf
	echo "vktools: $O/vkcube $O/vulkaninfo"
}

case ${1:-build} in
prepare|patches|build)	"${1:-build}" ;;
*)	echo "usage: vktools.sh [prepare|patches|build]" >&2; exit 1 ;;
esac
