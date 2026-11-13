#!/bin/bash
# The first step of the build for $objtype (build.$objtype.json, run by
# sys/src/cmd/build/build.exe): the install directories the manifests
# expect, and gcc's runtime library (64-bit division on 386, ...: 8c
# had vlrt.c), linked after the libraries (/$objtype/cflags.json).
#
# 386: the runtime library of ../9front-gcc's i686-pc-plan9 (GCC_OUT, as
# scripts/guest/stage-dist.sh), built for the tree's -march=pentium. gcc
# -m32's is the x86-64 Linux multilib's: SSE2 and cmov in its 64-bit
# division and friends, which the 386 programs and the kernel link (a
# Pentium has neither, the kernel does not save the SSE registers). Without
# ../9front-gcc the host's stays, with a warning.
#
# usage: objtype=386|amd64|arm64 scripts/build/prep-objtype.sh   (build.exe sets objtype)
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
. "$ROOT/scripts/lib/paths.sh"	# GCC_DIR, RUST_DIR, GO_DIR, MCP_DIR, DISTFILES (the distribution's paths.env)
objtype=${objtype:?objtype not set}
cd "$ROOT"
mkdir -p $objtype/bin/aux $objtype/lib.gcc
# copied only when it differs: every program links it, a new time would
# make them all out of date for build.exe -D
copy(){ cmp -s "$1" "$2" || cp "$1" "$2"; }
case $objtype in
386)
	GCC_OUT=${GCC_OUT:-$GCC_DIR/out/386}
	lib=$(ls "$GCC_OUT"/386/gnu/lib/gcc/i686-pc-plan9/*/libgcc.a 2>/dev/null | tail -1)
	if [ -z "$lib" ]; then
		lib=$(gcc -m32 -print-libgcc-file-name)
		echo "prep-objtype.sh: warning: no $GCC_OUT/386/gnu/lib/gcc/i686-pc-plan9/*/libgcc.a" \
			"(GCC_DIR: toolchain/gcc, objtype=386 build.sh cross native); the host's $lib" \
			"needs SSE2 and cmov (not a Pentium)" >&2
	fi
	copy "$lib" 386/lib.gcc/libgcc.a
	;;
arm64)	copy "$(${TOOLPREFIX-aarch64-linux-musl-}gcc -print-libgcc-file-name)" arm64/lib.gcc/libgcc.a ;;
# the Linux drivers (sys/src/linux) need gcc's helpers (128-bit division)
amd64)	copy "$(gcc -print-libgcc-file-name)" amd64/lib.gcc/libgcc.a ;;
esac
# POSIX's names of libraries whose functions are all in libap (gcc's APE,
# as musl): empty archives, for builds that name them (-lm -lpthread -lrt
# -ldl: LLVM, autoconf)
mkdir -p $objtype/lib.gcc/ape
for l in m pthread rt dl; do
	[ -f $objtype/lib.gcc/ape/lib$l.a ] || printf '!<arch>\n' > $objtype/lib.gcc/ape/lib$l.a
done
