#!/bin/bash
# libwayland into the tree (sys/src/wayland): the releases named in
# sys/src/wayland/VERSION (wayland and wayland-protocols, their tarballs in
# ../9front-ui/src); of them, the files sys/src/wayland/SOURCES names and the
# headers they include (the cross gcc's -MM) into sys/src/wayland/vendor, the
# code the host's wayland-scanner (of the same version) makes for the
# protocols into sys/src/wayland/gen, the public headers into
# /sys/include/ape, and sys/src/wayland/build.json.  Then build.exe builds
# libwayland-client.a and libwayland-server.a (with libffi.a: sys/src/libffi):
#   CC=$CROSS/x86_64-pc-plan9-gcc ARCH=amd64 sys/src/cmd/build/build.exe sys/src/wayland/build.json
set -e
. "$(dirname "$0")/../mesa/common.sh"	# CROSS, TRIPLE: ../9front-gcc's cross gcc with the tree as sysroot
CC=$CROSS/$TRIPLE-gcc WORK=${WAYLAND_WORK:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-wayland} \
	python3 "$(dirname "$0")/vendor.py" "$ROOT"
