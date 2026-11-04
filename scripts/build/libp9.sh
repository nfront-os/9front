#!/bin/bash
# libp9.a: the native libc (/$objtype/lib.gcc/libc.a) with every symbol it
# defines renamed __p9_<name>, for programs that have both it and APE in them
# (Rust's std is on the native libc, the C in a Rust program on APE: both
# define open, read, malloc, ...). The libc manifest's Post step, after
# libc.a is installed; replaced only when it changes (build.exe -D).
#
# usage: objtype=386|amd64|arm64|riscv64 scripts/build/libp9.sh   (build.exe sets objtype)
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
objtype=${objtype:?objtype not set}
case $objtype in arm64) P=${TOOLPREFIX-aarch64-linux-musl-} ;; riscv64) P=${TOOLPREFIX-riscv64-linux-gnu-} ;; *) P= ;; esac
L=$ROOT/$objtype/lib.gcc
T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
${P}nm -g --defined-only "$L/libc.a" | awk 'NF == 3 {print $3, "__p9_" $3}' | sort -u > "$T/syms"
${P}objcopy --redefine-syms="$T/syms" "$L/libc.a" "$T/libp9.a"
${P}ranlib "$T/libp9.a"
cmp -s "$T/libp9.a" "$L/libp9.a" || cp "$T/libp9.a" "$L/libp9.a"
