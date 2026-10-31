#!/bin/bash
# nix: NIX application cores (port/nix.c): boot with *nixac=$NAC on $NCPU
# processors (default 1 of 4), run tests/nix/nixtest.c and some commands
# on the last AC (execac(1)), move a process there and back with
# /proc/n/ctl. amd64 and arm64; 386 has no application cores
# (port/nixnone.c).
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
case $objtype in 386) skip "nix" "no application cores on 386"; done_tests; exit ;; esac
[ -f "$ROOT/$KBIN" ] || { skip "nix" "no kernel $KBIN, run $BUILDHOW"; done_tests; exit; }
[ -x "$BUILDEXE" ] || { skip "nix" "no build tool $BUILDEXE"; done_tests; exit; }

buildtests(){ (cd "$HERE" && env PLAN9="$ROOT" ARCH=$objtype CC=gcc "$BUILDEXE" build.json); }
check "build tests/nix with the port toolchain" buildtests || { done_tests; exit; }

NCPU=${NCPU:-4} NAC=${NAC:-1}
ac=$((NCPU-1)) ntc=$((NCPU-NAC))
rt=$((NCPU-NAC-1))	# a TC that becomes an AC at run time
# with two ACs or more: nixtest on two of them at once
par=()
[ "$NAC" -lt 2 ] || par=("execac -c $((ac-1)) /tests/nixtest $((ac-1)) > /tmp/par1 & execac -c $ac /tests/nixtest $ac > /tmp/par2; wait; echo par \`{grep -c '^ok' /tmp/par1} \`{grep -c '^ok' /tmp/par2}")
boot(){
	SMP=$NCPU KARGS="${KARGS:+$KARGS }*ncpu=$NCPU *nixac=$NAC" "$ROOT/scripts/guest/boot-cli.sh" \
		'cat /dev/sysstat | wc -l' \
		"grep -c ' AC\$' /dev/sysstat" \
		"execac -c $ac /bin/rc -c 'awk ''{print NF}'' /proc/\$pid/status'" \
		"execac -c $ac /tests/nixtest $ac" \
		"execac -c $ac /bin/hoc -e 'sqrt(2)*3'" \
		"execac -c $ac /bin/rc -c 'for(i in 1 2 3 4 5 6 7 8) echo \$i | wc -c' | wc -l" \
		"execac -c $ac /bin/rc -c 'while(){}' & sleep 2; x=\`{cat /proc/\$apid/core}; echo core \$x(1); echo kill > /proc/\$apid/note; sleep 1; ls /proc/\$apid >[2]/dev/null || echo killed" \
		"rc -c 'while(){}' & sleep 1; echo core $ac > /proc/\$apid/ctl; sleep 1; x=\`{cat /proc/\$apid/core}; echo moved \$x(1); echo core 0 > /proc/\$apid/ctl; sleep 1; x=\`{cat /proc/\$apid/core}; echo back \$x(1); echo kill > /proc/\$apid/note" \
		'execac -c 1 /bin/echo x' \
		"execac -c $ac /bin/sleep 3 & sleep 1; execac -c $ac /bin/echo y; wait" \
		'execac /bin/echo any' \
		"${par[@]}" \
		'cat /dev/nixctl' \
		"echo ac $rt > /dev/nixctl; grep -c ' AC\$' /dev/sysstat" \
		"execac -c $rt /tests/nixtest $rt | grep -c '^ok'" \
		'ls /bin | wc -l' \
		"echo tc $rt > /dev/nixctl; grep -c ' AC\$' /dev/sysstat" \
		"execac -c $rt /bin/echo x" \
		"echo kc 1 > /dev/nixctl; k=(); for(i in 1 2 3) { rc -c 'while(){}' & k=(\$k \$apid) }; sleep 4; awk '\$1 == 1 {print \"kc\", \$8} \$1 == 0 {print \"tc\", \$8}' /dev/sysstat; for(i in \$k) echo kill > /proc/\$i/note; echo tc 1 > /dev/nixctl" \
		> "$TMPD/console" 2>&1
	tr -d '\r' < "$TMPD/console" > "$TMPD/log"
}
check "boot with *nixac=$NAC on $NCPU processors" boot || { done_tests; exit; }
L=$TMPD/log
out(){ grep -A1 -F "# $1" "$L" | tail -1; }
# the last output line of a command (kernel messages may come first)
last(){ awk -v c="# $1" 'f && /# / {exit} f {l=$0} index($0, c) {f=1} END {print l}' "$L"; }

check "$NAC application cores (cpu$ntc-cpu$ac)" test "$(sed '/^init: starting/q' "$L" | grep -c '^cpu[0-9]*: application core')" -eq $NAC
check "/dev/sysstat lists all $NCPU cores" test "$(out 'cat /dev/sysstat | wc -l' | tr -d ' ')" -eq $NCPU
check "/dev/sysstat: $NAC with role AC" test "$(out "grep -c ' AC\$' /dev/sysstat" | tr -d ' ')" -eq $NAC
check "/proc/n/status: 15 fields (3 NIX counters)" test "$(out "execac -c $ac /bin/rc -c 'awk" | tr -d ' ')" = 15
while read -r st name; do
	case $st in
	ok)	pass "nixtest on the AC: $name" ;;
	FAIL)	fail "nixtest on the AC: $name" ;;
	esac
done < <(grep -E '^(ok|FAIL) ' "$L")
check "nixtest: all checks ran (13)" test "$(grep -cE '^(ok|FAIL) ' "$L")" -eq 13
check "hoc (floating point) on the AC" test "$(out "execac -c $ac /bin/hoc")" = 4.24264068712
check "rc forks and waits on the AC" test "$(out "execac -c $ac /bin/rc -c 'for" | tr -d ' ')" = 8
check "a loop on the AC: core $ac, killed by a note" bash -c "grep -A2 -F \"# execac -c $ac /bin/rc -c 'while\" '$L' | grep -q '^core $ac\$' && grep -A2 -F \"# execac -c $ac /bin/rc -c 'while\" '$L' | grep -q '^killed\$'"
check "ctl core $ac moves a process to the AC, core 0 back" bash -c "grep -A2 -F \"# rc -c 'while\" '$L' | tr '\n' ' ' | grep -q 'moved $ac back 0'"
check "execac on a TC fails" test "$(out 'execac -c 1')" = "execac: execac /bin/echo: not an application core"
check "a busy AC is refused" test "$(out "execac -c $ac /bin/sleep 3")" = "execac: execac /bin/echo: application core busy"
check "execac without -c picks a free AC" test "$(out 'execac /bin/echo any')" = any
[ "$NAC" -lt 2 ] || check "nixtest on cpu$((ac-1)) and cpu$ac at once" bash -c "grep -A1 -F '# execac -c $((ac-1)) /tests/nixtest' '$L' | grep -q '^par 13 13\$'"
check "/dev/nixctl: cpu$ac is an AC" bash -c "grep -A$NCPU -F '# cat /dev/nixctl' '$L' | grep -q '^$ac AC '"
check "cpu$rt becomes an AC at run time" test "$(last "echo ac $rt > /dev/nixctl" | tr -d ' ')" -eq $((NAC+1))
check "nixtest on the run-time AC cpu$rt" test "$(out "execac -c $rt /tests/nixtest" | tr -d ' ')" -eq 13
check "the root file system still answers (interrupts moved)" test "$(out 'ls /bin | wc -l' | tr -d ' ')" -gt 100
check "cpu$rt back to a TC" test "$(last "echo tc $rt > /dev/nixctl" | tr -d ' ')" -eq $NAC
check "execac to the former AC fails" test "$(out "execac -c $rt /bin/echo x")" = "execac: execac /bin/echo: not an application core"
kcload(){ grep -A2 -F '# echo kc 1 > /dev/nixctl' "$L" | awk -v w=$1 '$1 == w {print $2}'; }
check "KC cpu1 runs no user loop (load < 300), TC cpu0 does (> 700)" bash -c "[ \"\$(echo $(kcload kc))\" -lt 300 ] && [ \"\$(echo $(kcload tc))\" -gt 700 ]"
check "no kernel panic" bash -c "! grep -qE '^panic:|acexit: no process' '$L'"
done_tests
