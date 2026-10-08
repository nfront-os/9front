#!/bin/bash
# Generated sources of single commands, as the rules of their mkfiles
# make them; run as a Pre step in the command's directory:
#	gen-src.sh <dir below sys/src>
# The rc/awk/sed recipes run with plan9port's tools (../plan9port),
# absolute 9front paths point into this tree.
set -e
ROOT=$(git -C "$(dirname "$0")" rev-parse --show-toplevel)
P9P=${PLAN9PORT:-$ROOT/../plan9port}
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
*)
	echo "gen-src.sh: no rules for $1" >&2
	exit 1
esac
