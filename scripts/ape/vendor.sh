#!/bin/bash
# a library for APE into the tree from its release (sys/src/<lib>/VENDOR.json):
# scripts/ape/vendor.py with ../9front-gcc's cross compiler (its -MM finds
# what the sources include). Then build.exe -D builds it (scripts/build/ape.sh).
# usage: scripts/ape/vendor.sh lib
set -e
[ $# -eq 1 ] || { echo "usage: scripts/ape/vendor.sh lib" >&2; exit 1; }
. "$(dirname "$0")/../mesa/common.sh"	# CROSS, TRIPLE: the cross gcc with the tree as sysroot
CC=$CROSS/$TRIPLE-gcc WORK=${APE_WORK:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-ape} \
	python3 "$(dirname "$0")/vendor.py" "$ROOT" "$1"
