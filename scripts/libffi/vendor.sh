#!/bin/bash
# libffi into the tree (sys/src/libffi): the release named in
# sys/src/libffi/VERSION (its tarball in ../9front-ui/src); of it, the files
# sys/src/libffi/SOURCES names and the headers they include (the cross gcc's
# -MM) into sys/src/libffi/vendor, ffi.h (made from ffi.h.in, as configure
# would for amd64) and ffitarget.h into /sys/include/ape, and
# sys/src/libffi/build.json.  Then build.exe builds libffi.a:
#   CC=$CROSS/x86_64-pc-plan9-gcc ARCH=amd64 sys/src/cmd/build/build.exe sys/src/libffi/build.json
set -e
. "$(dirname "$0")/../mesa/common.sh"	# CROSS, TRIPLE: ../9front-gcc's cross gcc with the tree as sysroot
CC=$CROSS/$TRIPLE-gcc WORK=${LIBFFI_WORK:-$CACHE/libffi} \
	python3 "$(dirname "$0")/vendor.py" "$ROOT"
