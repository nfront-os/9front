#!/bin/bash
# Mesa's venus driver into the tree (sys/src/mesa/vendor, build$SUFFIX.json), from
# the meson build of build.sh: see vendor.py. Then build.exe builds
# libvulkan.a without meson:
#   CC=$CROSS/x86_64-pc-plan9-gcc ARCH=amd64 sys/src/cmd/build/build.exe sys/src/mesa/build.json
set -e
. "$(dirname "$0")/common.sh"
[ -f "$BUILD/mesa/compile_commands.json" ] || "$(dirname "$0")/build.sh"
export PATH=$VENV/bin:$PATH
export objtype
NM=$CROSS/$TRIPLE-nm python3 "$(dirname "$0")/vendor.py" "$SRC" "$BUILD" "$ROOT"
