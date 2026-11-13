#!/bin/bash
# SDL3 into the tree (sys/src/sdl3): the release named in sys/src/sdl3/VERSION
# (its tarball in ../9front-ui/src), unpacked with sys/src/sdl3/overlay over it;
# of it, the files sys/src/sdl3/SOURCES names and the headers they include
# (the cross gcc's -MM) into sys/src/sdl3/vendor, SDL's public headers into
# /sys/include/ape/SDL3, and sys/src/sdl3/build.json.  Then build.exe builds
# libSDL3.a:
#   CC=$CROSS/x86_64-pc-plan9-gcc ARCH=amd64 sys/src/cmd/build/build.exe sys/src/sdl3/build.json
set -e
. "$(dirname "$0")/../mesa/common.sh"	# CROSS, TRIPLE: ../9front-gcc's cross gcc with the tree as sysroot
CC=$CROSS/$TRIPLE-gcc WORK=${SDL_WORK:-$CACHE/sdl} \
	python3 "$(dirname "$0")/vendor.py" "$ROOT"
