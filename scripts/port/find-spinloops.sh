#!/bin/bash
# Busy-wait loops gcc compiled into a branch to itself. kencc reloads
# memory on every iteration of `while(p->x) ;'; gcc may load it once
# before the loop (not volatile) and then spin forever (ilock's
# `while(l->key) ;' did, so did syncclock's). Every branch whose target
# is its own address in the kernel is reported, except the intentional
# `for(;;);' (panic, rebootjump) and data in the vector table.
#
# usage: scripts/port/find-spinloops.sh kernel.elf   (exit 1 if any)
set -e
k=${1:?usage: $0 kernel}
case $(readelf -h "$k" | awk '/Machine:/{print $2}') in
AArch64) OBJDUMP=aarch64-linux-musl-objdump ADDR2LINE=aarch64-linux-musl-addr2line PAT='^\t(b|b\.[a-z]+|cbn?z|tbn?z)\t' ;;
*) OBJDUMP=objdump ADDR2LINE=addr2line PAT='^\t(j[a-z]+)' ;;
esac
$OBJDUMP -d --no-show-raw-insn "$k" |
	awk -v pat="$PAT" '{ split($0, f, ":"); a=f[1]; gsub(/ /, "", a); rest=substr($0, length(f[1])+2) } rest ~ pat { n=split(rest, w, /[ \t,]+/); for(i=1;i<=n;i++) if(w[i]==a) print a }' |
	while read a; do $ADDR2LINE -e "$k" -f 0x$a | paste - -; done |
	grep -vE '^(panic|rebootjump|vectortable|_[a-z]*halt|stop|_stop)\b' && exit 1
exit 0
