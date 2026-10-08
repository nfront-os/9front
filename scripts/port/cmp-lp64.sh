#!/bin/bash
# Differential test for the LP64 hazards of the gcc-built amd64 tools:
# build one of 6c, 6a, 6l twice as a host program against plan9port's
# lib9 -- once -m32, where long is 32 bits as under kencc (the
# reference, as the tool built on a 386), once -m64 (long is 64 bits,
# as the gcc-built tool in the guest) -- run both on the same input and
# compare the output. A difference is a place where the tool's sources
# assume a 32-bit long.
#
# usage: scripts/port/cmp-lp64.sh 6c [-S] file.c ...  compile each file, compare objects
#                                                     (-S: the assembly listings)
#        scripts/port/cmp-lp64.sh 6a file.s ...       assemble each file, compare objects
#        scripts/port/cmp-lp64.sh 6l [6l flags] file.6|lib.a ...
#                                                     link once, compare the a.out
#                                                     (AMD64: the directory seen as
#                                                     /amd64, for the objects'
#                                                     #pragma lib /amd64/lib/...)
#   paths are relative to the tree root
#   env:  P9P   plan9port tree (default ../plan9port beside the tree)
#         OUT   work dir (default /tmp/cmp-lp64)
#   needs: gcc with -m32 (multilib), setarch; for 6l with AMD64 bwrap
#
# Host-build shims: cc/compat's allocator grows from `end` with brk(2),
# so the tools run under setarch -R (no brk randomization); round,
# log2, IOUNIT and setmalloctag clash with the host headers and are
# renamed through a forced include, and ulong is 32 bits as in the gcc
# port's amd64/include/u.h (plan9port's is unsigned long). compat's
# malloc family is renamed with it (its ulong signatures); lib9 keeps
# glibc's malloc, told to use mmap only so that its heap never meets
# the brk-grown hunk.
set -e
ROOT=$(git -C "$(dirname "$0")" rev-parse --show-toplevel)
P9P=${P9P:-$(cd "$ROOT/.." && pwd)/plan9port}
OUT=${OUT:-/tmp/cmp-lp64}
TOOL=$1
shift || true
listing=
[ "$TOOL" = 6c ] && [ "$1" = -S ] && { listing=-S; shift; }
mkdir -p "$OUT/l32" "$OUT/obj"

cat > "$OUT/pre.h" <<'E'
#include <u.h>
#include <libc.h>
#include <bio.h>
#include <ctype.h>
#define round cc_round
#define log2 cc_log2
#define IOUNIT 8192
#define setmalloctag cc_setmalloctag
#define setrealloctag cc_setrealloctag
#define ulong unsigned int
#define malloc cc_malloc
#define calloc cc_calloc
#define realloc cc_realloc
#define free cc_free
#define mallocz cc_mallocz
void*	malloc(ulong);
void*	calloc(ulong, ulong);
void*	realloc(void*, ulong);
void	free(void*);
void*	mallocz(ulong, int);
E

# plan9port's lib9 + libbio, 32-bit (its installed archives are 64-bit);
# without the fmt/utf test programs, whose main() would win
if [ ! -f "$OUT/l32/lib9.a" ]; then
	(cd "$OUT/l32"
	for f in "$P9P"/src/lib9/*.c "$P9P"/src/lib9/fmt/*.c "$P9P"/src/lib9/utf/*.c "$P9P"/src/libbio/*.c; do
		case $f in */test*.c) continue ;; esac
		o=$(echo "${f#$P9P/src/}" | tr / _)
		gcc -m32 -std=gnu17 -w -c -fcommon -I"$P9P/include" -I"$P9P/src/lib9" \
			-I"$P9P/src/lib9/fmt" -I"$P9P/src/lib9/utf" -DPLAN9PORT -o "${o%.c}.o" "$f" 2>/dev/null || true
	done
	ar rcs lib9.a *.o)
fi

cd "$ROOT/sys/src/cmd"
case $TOOL in
6c)	SRC="$(ls cc/*.c | grep -v omachcap) 6c/cgen.c 6c/div.c 6c/enam.c 6c/list.c 6c/machcap.c 6c/mul.c 6c/peep.c 6c/reg.c 6c/sgen.c 6c/swt.c 6c/txt.c"
	INC="-Icc -I6c" ;;
6a)	SRC="6a/y.tab.c 6a/lex.c"
	INC="-Icc -I6a" ;;
6l)	SRC="6l/asm.c 6l/obj.c 6l/optab.c 6l/pass.c 6l/span.c 6l/list.c 6l/compat.c 6c/enam.c"
	INC="-Icc -I6l" ;;
*)	echo "usage: cmp-lp64.sh 6c|6a|6l ..." >&2; exit 2 ;;
esac
CF="-std=gnu17 -w -g -fno-builtin -fno-strict-aliasing -fcommon -include $OUT/pre.h -I$P9P/include $INC"
gcc -m32 $CF -o "$OUT/$TOOL.32" $SRC "$OUT/l32/lib9.a" -lm
gcc $CF -o "$OUT/$TOOL.64" $SRC -L"$P9P/lib" -lbio -l9 -lm

run(){	# arch, args...
	local a=$1 ns=()
	shift
	# a root of the host's /usr, the tree, /tmp and $AMD64 as /amd64
	[ -n "$AMD64" ] && ns=(bwrap --tmpfs / --ro-bind /usr /usr --symlink usr/lib /lib \
		--symlink usr/lib /lib64 --symlink usr/bin /bin --proc /proc --dev /dev \
		--bind /tmp /tmp --ro-bind "$ROOT" "$ROOT" --bind "$OUT" "$OUT" \
		--ro-bind "$AMD64" /amd64 --chdir "$PWD")
	GLIBC_TUNABLES=glibc.malloc.mmap_threshold=0 "${ns[@]}" setarch -R "$OUT/$TOOL.$a" "$@"
}

cd "$ROOT"
if [ "$TOOL" = 6l ]; then
	b=$OUT/obj/6l.out
	rm -f "$b.32" "$b.64"
	for a in 32 64; do
		run $a -o "$b.$a" "$@" > "$b.$a.err" 2>&1 || true
	done
	if [ ! -s "$b.32" ]; then
		echo "FAIL reference link: $(head -3 "$b.32.err")"
		exit 1
	fi
	cmp "$b.32" "$b.64" && echo "6l: same ($b.32)"
	exit
fi
n=0; d=0
for f; do
	b=$OUT/obj/$(echo "$f" | tr / _)
	rm -f "$b.32" "$b.64"
	for a in 32 64; do
		if [ -n "$listing" ]; then
			run $a -FTVw -S -Iamd64/include -Isys/include -I"$(dirname "$f")" -o /dev/null "$f" > "$b.$a" 2>&1 || true
		elif [ "$TOOL" = 6c ]; then
			run $a -FTVw -Iamd64/include -Isys/include -I"$(dirname "$f")" -o "$b.$a" "$f" > "$b.$a.err" 2>&1 || true
		else
			run $a -Iamd64/include -Isys/include -I"$(dirname "$f")" -o "$b.$a" "$f" > "$b.$a.err" 2>&1 || true
		fi
	done
	n=$((n + 1))
	if [ ! -s "$b.32" ] && [ ! -s "$b.64" ]; then
		echo "FAIL $f (both: $(head -1 "$b.32.err" 2>/dev/null))"
	elif ! cmp -s "$b.32" "$b.64"; then
		d=$((d + 1))
		echo "DIFF $f"
	fi
done
echo "$n compiled, $d differ (outputs in $OUT/obj)"
