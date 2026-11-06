#!/bin/bash
# GLFW into the tree (sys/src/glfw): the release named in sys/src/glfw/VERSION
# (its zip in ../9front-ui/src), unpacked with sys/src/glfw/overlay over it;
# of it, the files sys/src/glfw/SOURCES names and the headers they include
# (the cross gcc's -MM) into sys/src/glfw/vendor, GLFW's public headers into
# /sys/include/ape/GLFW, and sys/src/glfw/build.json.  Then build.exe builds
# libglfw3.a:
#   CC=$CROSS/x86_64-pc-plan9-gcc ARCH=amd64 sys/src/cmd/build/build.exe sys/src/glfw/build.json
set -e
. "$(dirname "$0")/../mesa/common.sh"	# CROSS, TRIPLE: ../9front-gcc's cross gcc with the tree as sysroot
CC=$CROSS/$TRIPLE-gcc WORK=${GLFW_WORK:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-glfw} \
	python3 "$(dirname "$0")/vendor.py" "$ROOT"
