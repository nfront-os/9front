#!/bin/bash
# Generated sources of single commands, as the rules of their mkfiles
# make them; run as a Pre step in the command's directory:
#	gen-src.sh <dir below sys/src>
# The rc/awk/sed recipes run with plan9port's tools (../plan9port),
# absolute 9front paths point into this tree.
set -e
ROOT=$(git -C "$(dirname "$0")" rev-parse --show-toplevel)
. "$ROOT/scripts/lib/paths.sh"	# PLAN9PORT (the distribution's paths.env)
P9P=$PLAN9PORT
S=$ROOT/scripts
cd "$ROOT/sys/src/$1"
p9() { PLAN9=$P9P PATH=$P9P/bin:$PATH "$@"; }
mkvar() {	# words of mkfile variable $1
	sed -e ':a' -e '/\\$/N; s/\\\n//; ta' mkfile | sed -n "s/^$1=//p" | tr -s ' \t' '\n\n' | grep -v '^$'
}

case $1 in
cmd/ip/snoopy)
	# protos.h, protos.c: from $PROTOS
	PROTOS=$(mkvar PROTOS)
	for i in $PROTOS; do echo "extern Proto $i;"; done >protos.h
	{
		echo '#include <u.h>'
		echo '#include <libc.h>'
		echo '#include "dat.h"'
		echo '#include "protos.h"'
		echo 'Proto *protos[] ='
		echo '{'
		for i in $PROTOS; do echo "	&$i,"; done
		echo '	0,'
		echo '};'
	} >protos.c
	;;
cmd/syscall)
	# tab.h: ./mktab.awk $SYSCALL
	p9 awk -f mktab.awk "$ROOT/sys/src/libc/9syscall/sys.h" >tab.h
	;;
cmd/tcs)
	# alias.h: ./charsets.awk charsets.xml alias.txt
	p9 awk -f charsets.awk charsets.xml alias.txt >alias.h
	;;
cmd/nusb/disk)
	# scsierrs.c: ./mkscsierrs (reads /sys/lib/scsicodes)
	sed "s#/sys/lib/scsicodes#$ROOT/sys/lib/scsicodes#" mkscsierrs | p9 rc >scsierrs.c
	;;
cmd/[0-9a-z]c|cmd/[0-9a-z]l)
	# compilers: enam.c: ../$D/$T.out.h	rc ../cc/mkenam (cc/mkfile.port);
	# loaders: ../Xc/enam.c: ../Xc/X.out.h	@{ cd ../Xc; mk enam.c }
	# (T from the compiler's mkfile: 1c builds with 2.out.h, tc with 5.out.h)
	case $1 in
	*c)	d=. ;;
	*l)	d=$(sed -n 's,^\.\./\([0-9a-z]c\)/enam\.c:.*,../\1,p' mkfile | head -1) ;;
	esac
	# made again on every build; one that comes out as it was keeps its
	# time (build.exe -D: 9l's and ql's cnam.c are checked in, not Clean)
	K=$(mktemp -d)
	trap 'rm -rf "$K"' EXIT
	for f in $d/enam.c cnam.c; do [ -f "$f" ] && cp -p "$f" "$K/$(echo $f | tr / _)"; done
	(cd "$d" && T=$(sed -n 's/^T=//p' mkfile) && export T && p9 rc ../cc/mkenam)
	# 7l: cnam.c: l.h mkcname	rc ./mkcname
	if grep -q '^cnam.c:' mkfile; then
		p9 rc ./mkcname
	fi
	for f in $d/enam.c cnam.c; do
		k=$K/$(echo $f | tr / _)
		if [ -f "$k" ] && cmp -s "$k" "$f"; then touch -r "$k" "$f"; fi
	done
	;;
games/blabs)
	# %.bits: ./mkface $stem, for $FACES (mkface reads /lib/face)
	sed "s#/lib/face#$ROOT/lib/face#g" mkface >mkface.host
	for f in $(mkvar FACES); do
		p9 rc ./mkface.host "${f%.bits}"
	done
	rm -f mkface.host
	;;
cmd/gs)
	# $objtype.h: $O.genarch	./$O.genarch $target (objtype = cputype):
	# genarch measures the compiler's types, so it is built with the
	# host's gcc, whose amd64 ABI (LP64, System V) the port's gcc shares
	# (386: gcc -m32, which the host runs too); else, as the mkfile
	# does for objtype != cputype, default.$objtype.h
	case ${objtype:-amd64} in
	amd64|386)
		m=; [ ${objtype:-amd64} = 386 ] && m=-m32
		gcc $m -std=gnu17 -DHAVE_LONG_LONG -o genarch.host src/genarch.c
		./genarch.host ${objtype:-amd64}.h
		rm -f genarch.host
		;;
	*)
		cp default.$objtype.h $objtype.h && chmod 664 $objtype.h
		;;
	esac
	;;
*)
	echo "gen-src.sh: no rules for $1" >&2
	exit 1
esac
