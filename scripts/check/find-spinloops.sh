#!/bin/bash
# Busy-wait loops gcc compiled so that they can never end. kencc reloads
# memory on every iteration of `while(p->x) ;'; gcc may load it once
# before the loop (not volatile) and then spin forever (ilock's
# `while(l->key) ;' did, so did syncclock's and arm64's synccycles).
# Reported are backward branches whose loop body only compares or tests
# what is already in registers -- a branch to itself (x86: jmp ., jne .)
# or `cmp x1, x0; b.ne <the cmp>' (arm64), `bnez a5, .' (riscv64) -- except the intentional
# `for(;;);' (panic, rebootjump, abort), data in the vector table and
# arm64's _v*patch branches, which trap.c rewrites at run time.
#
# usage: scripts/check/find-spinloops.sh kernel.elf   (exit 1 if any)
set -e
k=${1:?usage: $0 kernel}
case $(readelf -h "$k" | awk '/Machine:/{print $2}') in
AArch64)
	OBJDUMP=aarch64-linux-musl-objdump ADDR2LINE=aarch64-linux-musl-addr2line
	BR='^(b|b[.][a-z]+|cbn?z|tbn?z)$' CMP='^(cmp|cmn|tst|ccmp)$'
	;;
RISC-V)
	# the compare is the branch itself: a loop of just the branch
	OBJDUMP=riscv64-linux-gnu-objdump ADDR2LINE=riscv64-linux-gnu-addr2line
	BR='^(j|b[a-z]+)$' CMP='^$'
	;;
*)
	OBJDUMP=objdump ADDR2LINE=addr2line
	BR='^j[a-z]+$' CMP='^(cmp[bwlq]?|test[bwlq]?)$'
	;;
esac
$OBJDUMP -d --no-show-raw-insn "$k" |
	awk -v br="$BR" -v cmp="$CMP" '
	/^[0-9a-f]+ <.*>:$/ { n = 0; next }
	/^ *[0-9a-f]+:\t/ {
		split($0, f, "\t"); a = f[1]; gsub(/[ :]/, "", a)
		m = f[2]; sub(/ .*/, "", m)
		ops = f[3]
		n++; addr[n] = a; mn[n] = m
		if(m !~ br)
			next
		# the branch target: a hex word equal to an earlier address
		t = ""
		nw = split(ops, w, /[ ,]+/)
		for(i = 1; i <= nw; i++)
			if(w[i] ~ /^[0-9a-f]+$/ && length(w[i]) >= 6)
				t = w[i]
		if(t == "")
			next
		for(j = n; j >= 1 && n - j < 4; j--){
			if(addr[j] != t)
				continue
			ok = 1
			for(i = j; i < n; i++)
				if(mn[i] !~ cmp)
					ok = 0
			if(ok)
				print a
			break
		}
	}' |
	while read a; do $ADDR2LINE -e "$k" -f 0x$a | paste - -; done |
	grep -vE '^(panic|rebootjump|vectortable|_[a-z]*halt|stop|_stop|abort|_v[a-z]*patch)\b' && exit 1
exit 0
