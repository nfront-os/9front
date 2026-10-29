#!/bin/bash
# Generate musl's bits/alltypes.h and bits/syscall.h for APE's musl part
# (sys/src/ape/lib/musl) the way musl's Makefile does, into
# sys/src/ape/lib/musl/obj/<arch>/include/bits. usage: gen-musl-bits.sh arch
# (musl's: x86_64, i386, aarch64). A file that comes out the same keeps its time.
set -e
ROOT=${PLAN9:-$(git rev-parse --show-toplevel)}
M=$ROOT/sys/src/ape/lib/musl
A=$1
O=$M/obj/$A/include/bits
mkdir -p "$O"
put(){	# put file: stdin to file, if it differs
	if ! cmp -s - "$1" < "$O/.new"; then mv "$O/.new" "$1"; else rm -f "$O/.new"; fi
}
sed -f "$M/tools/mkalltypes.sed" "$M/arch/$A/bits/alltypes.h.in" "$M/include/alltypes.h.in" > "$O/.new"
put "$O/alltypes.h"
{ cat "$M/arch/$A/bits/syscall.h.in"; sed -n -e 's/__NR_/SYS_/p' < "$M/arch/$A/bits/syscall.h.in"; } > "$O/.new"
put "$O/syscall.h"
