#!/bin/bash
# usage: putstripped.sh STRIP SRC DST
# DST becomes SRC stripped (STRIP: the cross compiler's strip), replaced
# only when its content changes: an unchanged program keeps its time, so
# scripts/guest/gen-ufs-root.sh's rsync does not copy it into the guest's
# root again. Linked statically against Mesa, a test program is ~270 MB
# with its debug information and ~40 MB without. The unstripped SRC stays
# where it was built, for addr2line (btassert's backtraces), gdb and
# scripts/debug/thread-dump.py.
set -e
strip=${1:?usage: $0 STRIP SRC DST} src=${2:?} dst=${3:?}
"$strip" -o "$dst.new" "$src"
if cmp -s "$dst.new" "$dst"; then
	rm -f "$dst.new"
else
	mv -f "$dst.new" "$dst"
fi
