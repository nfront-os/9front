#!/bin/bash
# The first step of the build for $objtype (build.$objtype.json, run by
# sys/src/cmd/build/build.exe): the install directories the manifests
# expect, and gcc's runtime library (64-bit division on 386, ...: 8c
# had vlrt.c), linked after the libraries (/$objtype/cflags.json).
#
# usage: objtype=386|amd64|arm64 scripts/prep-objtype.sh   (build.exe sets objtype)
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
objtype=${objtype:?objtype not set}
cd "$ROOT"
mkdir -p $objtype/bin/aux $objtype/lib.gcc
case $objtype in
386)	cp "$(gcc -m32 -print-libgcc-file-name)" 386/lib.gcc/libgcc.a ;;
arm64)	cp "$(${TOOLPREFIX-aarch64-linux-musl-}gcc -print-libgcc-file-name)" arm64/lib.gcc/libgcc.a ;;
esac
