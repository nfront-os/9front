#!/bin/bash
# The tree's parts for APE (POSIX on 9front) made with ../9front-gcc's cross
# compiler: build.exe -D on the libraries' compatibility manifests (sys/src/mesa:
# libvulkan.a, the Vulkan drivers and the static loader, EGL and GLES on zink;
# sys/src/libffi, sys/src/wayland: libwayland; sys/src/sdl3; sys/src/glfw: into
# /$objtype/lib.gcc/ape), then the programs of them in /$objtype/bin
# (scripts/gen/apeprogs.py: wlsys, scripts/wayland/build.sh). build.exe's step
# ape, before rust (wsys links libvulkan.a). Without the cross compiler it says
# so and builds nothing (check-install.py: nocross). A program that comes out
# the same keeps its time (build.exe -D).
# usage: [objtype=amd64] scripts/build/ape.sh
set -e
ROOT=$(git -C "$(dirname "${BASH_SOURCE[0]}")" rev-parse --show-toplevel)
objtype=${objtype:-amd64}
if ! python3 -c "import sys; sys.path.insert(0, '$ROOT/scripts/gen'); import apeprogs; sys.exit(not apeprogs.cross('$objtype'))"; then
	echo "ape: no cross compiler of nfront's gcc (GCC_DIR, toolchain/gcc) for $objtype: Mesa, libwayland, SDL3, GLFW, wlsys not built"
	exit 0
fi
. "$ROOT/scripts/mesa/common.sh"
LOG=$BUILD/ape.log
mkdir -p "$BUILD"
for d in $(python3 -c "import sys; sys.path.insert(0, '$ROOT/scripts/gen'); import apeprogs; print(' '.join(apeprogs.LIBS))"); do
	echo "ape: $d"
	CC=$CROSS/$TRIPLE-gcc ARCH=$objtype "$ROOT/sys/src/cmd/build/build.exe" -D "$ROOT/$d/build.json" > "$LOG" 2>&1 ||
		{ tail -20 "$LOG"; echo "ape: $d failed (all of it in $LOG)" >&2; exit 1; }
done
# the programs, into /$objtype/bin when they changed
OUT=$BUILD/ape-bin
mkdir -p "$OUT" "$ROOT/$objtype/bin"
bash "$ROOT/scripts/wayland/build.sh" "$OUT"
# programs of a vendored release, from its library (apeprogs.LINKS: foot)
python3 -c "import sys; sys.path.insert(0, '$ROOT/scripts/gen'); import apeprogs
for p, a in sorted(apeprogs.LINKS.items()): print(p, ' '.join(a))" | while read -r p args; do
	"$CROSS/$TRIPLE-gcc" -o "$OUT/$p" $args > "$LOG" 2>&1 ||
		{ tail -20 "$LOG"; echo "ape: linking $p failed" >&2; exit 1; }
done
for p in $(python3 -c "import sys; sys.path.insert(0, '$ROOT/scripts/gen'); import apeprogs; print(' '.join(apeprogs.PROGS))"); do
	cmp -s "$OUT/$p" "$ROOT/$objtype/bin/$p" && continue
	cp "$OUT/$p" "$ROOT/$objtype/bin/$p.new"
	mv "$ROOT/$objtype/bin/$p.new" "$ROOT/$objtype/bin/$p"
	echo "ape: /$objtype/bin/$p"
done
