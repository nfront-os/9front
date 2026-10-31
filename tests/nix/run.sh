#!/bin/bash
# nix: NIX application cores (port/nix.c): boot with *nixac=1 on 4
# processors, run tests/nix/nixtest.c and some commands on the AC
# (execac(1)), move a process there and back with /proc/n/ctl.
# amd64 and arm64; 386 has no application cores (port/nixnone.c).
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
case $objtype in 386) skip "nix" "no application cores on 386"; done_tests; exit ;; esac
[ -f "$ROOT/$KBIN" ] || { skip "nix" "no kernel $KBIN, run $BUILDHOW"; done_tests; exit; }
[ -x "$BUILDEXE" ] || { skip "nix" "no build tool $BUILDEXE"; done_tests; exit; }

buildtests(){ (cd "$HERE" && env PLAN9="$ROOT" ARCH=$objtype CC=gcc "$BUILDEXE" build.json); }
check "build tests/nix with the port toolchain" buildtests || { done_tests; exit; }

ac=3
boot(){
	SMP=4 KARGS="${KARGS:+$KARGS }*ncpu=4 *nixac=1" "$ROOT/scripts/guest/boot-cli.sh" \
		'cat /dev/sysstat | wc -l' \
		"execac -c $ac /tests/nixtest $ac" \
		"execac -c $ac /bin/hoc -e 'sqrt(2)*3'" \
		"execac -c $ac /bin/rc -c 'for(i in 1 2 3 4 5 6 7 8) echo \$i | wc -c' | wc -l" \
		"execac -c $ac /bin/rc -c 'while(){}' & sleep 2; echo core \`{cat /proc/\$apid/core}; echo kill > /proc/\$apid/note; sleep 1; ls /proc/\$apid >[2]/dev/null || echo killed" \
		"rc -c 'while(){}' & sleep 1; echo core $ac > /proc/\$apid/ctl; sleep 1; echo moved \`{cat /proc/\$apid/core}; echo core 0 > /proc/\$apid/ctl; sleep 1; echo back \`{cat /proc/\$apid/core}; echo kill > /proc/\$apid/note" \
		'execac -c 1 /bin/echo x' \
		"execac -c $ac /bin/sleep 3 & sleep 1; execac -c $ac /bin/echo y; wait" \
		'execac /bin/echo any' \
		> "$TMPD/console" 2>&1
	tr -d '\r' < "$TMPD/console" > "$TMPD/log"
}
check "boot with *nixac=1 on 4 processors" boot || { done_tests; exit; }
L=$TMPD/log
out(){ grep -A1 -F "# $1" "$L" | tail -1; }

check "cpu$ac: application core" grep -q "^cpu$ac: application core" "$L"
check "/dev/sysstat lists the 3 time-sharing cores" test "$(out 'cat /dev/sysstat | wc -l' | tr -d ' ')" -eq 3
while read -r st name; do
	case $st in
	ok)	pass "nixtest on the AC: $name" ;;
	FAIL)	fail "nixtest on the AC: $name" ;;
	esac
done < <(grep -E '^(ok|FAIL) ' "$L")
check "nixtest: all checks ran (6)" test "$(grep -cE '^(ok|FAIL) ' "$L")" -eq 6
check "hoc (floating point) on the AC" test "$(out "execac -c $ac /bin/hoc")" = 4.24264068712
check "rc forks and waits on the AC" test "$(out "execac -c $ac /bin/rc -c 'for" | tr -d ' ')" = 8
check "a loop on the AC: core $ac, killed by a note" bash -c "grep -A2 -F \"# execac -c $ac /bin/rc -c 'while\" '$L' | grep -q '^core $ac\$' && grep -A2 -F \"# execac -c $ac /bin/rc -c 'while\" '$L' | grep -q '^killed\$'"
check "ctl core $ac moves a process to the AC, core 0 back" bash -c "grep -A2 -F \"# rc -c 'while\" '$L' | tr '\n' ' ' | grep -q 'moved $ac back 0'"
check "execac on a TC fails" test "$(out 'execac -c 1')" = "execac: execac /bin/echo: not an application core"
check "a busy AC is refused" test "$(out "execac -c $ac /bin/sleep 3")" = "execac: execac /bin/echo: application core busy"
check "execac without -c picks a free AC" test "$(out 'execac /bin/echo any')" = any
check "no kernel panic" bash -c "! grep -qE '^panic:|acexit: no process' '$L'"
done_tests
