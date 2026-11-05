#!/bin/bash
# pmc: performance counters (port/devpmc.c, pmc(3), pmc(2)): boot on 4
# processors, run tests/pmc/pmctest.c wired to cpu1, also reading and
# setting a counter of cpu3 while a loop runs there, and two processes
# reading cpu3's counter at once (one on cpu3 itself, one elsewhere).
# x86 guests get KVM's virtual counters (boot-cli.sh PMU=1), then a
# second boot without them must show none; arm64 (TCG) has PMUv3 always.
# SKIP where the virtual processor has no counters. 386 and riscv64 have
# no pmc.
# After tests/nix/pmc.sh on the nix branch.
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
case $objtype in amd64|arm64) ;; *) skip "pmc" "no pmc(3) on $objtype"; done_tests; exit ;; esac
[ -f "$ROOT/$KBIN" ] || { skip "pmc" "no kernel $KBIN, run $BUILDHOW"; done_tests; exit; }
[ -x "$BUILDEXE" ] || { skip "pmc" "no build tool $BUILDEXE"; done_tests; exit; }

buildtests(){ (cd "$HERE" && env PLAN9="$ROOT" ARCH=$objtype CC=gcc "$BUILDEXE" build.json); }
check "build tests/pmc with the port toolchain" buildtests || { done_tests; exit; }

NCPU=4 other=3
# a loop wired to cpu $other
loop="rc -c 'while(){}' & l=\$apid; echo wired $other > /proc/\$l/ctl"
boot(){
	PMU=1 SMP=$NCPU KARGS="${KARGS:+$KARGS }*ncpu=$NCPU" "$ROOT/scripts/guest/boot-cli.sh" \
		"ls '#ε' | wc -l" \
		"cat '#ε/ctrdesc' | wc -l" \
		"$loop; /tests/pmctest $other -s; echo kill > /proc/\$l/note" \
		"d='#ε/core000$other'; echo reset > \$d/ctr00ctl; echo os > \$d/ctr00ctl; echo user > \$d/ctr00ctl; echo set cycles > \$d/ctr00ctl; echo enable > \$d/ctr00ctl; /tests/pmctest stress $other 1000 $other > /tmp/s1 & /tests/pmctest stress $other 1000 0 > /tmp/s2; wait; echo stress \`{cat /tmp/s1 /tmp/s2}" \
		"for(i in 0 1 2 3) /tests/pmctest core \$i" \
		> "$TMPD/console" 2>&1
	tr -d '\r' < "$TMPD/console" > "$TMPD/log"
}
check "boot on $NCPU processors" boot || { done_tests; exit; }
L=$TMPD/log
[ -z "$PMCLOG" ] || cp "$L" "$PMCLOG"	# env PMCLOG: keep the console log
out(){ grep -A1 -F "# $1" "$L" | tail -1; }

check "#ε lists ctrdesc, core and the $NCPU cores" test "$(out "ls '#ε'" | tr -d ' ')" -eq $((NCPU+2))
for i in 0 1 2 3; do
	check "#ε/core on cpu$i" grep -qx "core $i $i" "$L"
done
if grep -q '^SKIP pmc:' "$L"; then
	skip "pmc" "$(grep -m1 '^SKIP pmc:' "$L" | sed 's/^SKIP pmc: //') on this virtual processor"
else
	grep -E '^(counters|event) ' "$L" | sort -u | sed 's/^/       /'
	sect=$(awk '/pmctest '$other' -s/ && /# /{p=1; next} p && /^[^ ]*# /{exit} p' "$L")
	while read -r st name; do
		case $st in
		ok)	pass "pmctest: $name" ;;
		FAIL)	fail "pmctest: $name" ;;
		SKIP)	skip "pmctest: $name" "" ;;
		esac
	done < <(grep -E '^(ok|FAIL|SKIP) ' <<< "$sect")
	check "pmctest: all checks ran (23)" test "$(grep -cE '^(ok|FAIL) ' <<< "$sect")" -eq 23
	check "two processes read cpu$other's counter at once" bash -c "grep -qE '^stress ok stress [0-9]+ ok stress [0-9]+\$' '$L'"
fi
check "no kernel panic" bash -c "! grep -qE '^panic:' '$L'"

# without KVM's virtual counters: the device is there, with no counters
if [ $objtype = amd64 ]; then
	boot0(){
		SMP=$NCPU KARGS="${KARGS:+$KARGS }*ncpu=$NCPU" "$ROOT/scripts/guest/boot-cli.sh" \
			"ls '#ε/core0000' | wc -l" \
			"cat '#ε/ctrdesc' | wc -l" \
			"/tests/pmctest $other" \
			> "$TMPD/console0" 2>&1
		tr -d '\r' < "$TMPD/console0" > "$TMPD/log0"
	}
	check "boot without virtual counters (pmu=off)" boot0 && {
		check "pmu=off: no counters" test "$(grep -A1 -F "# ls '#ε/core0000'" "$TMPD/log0" | tail -1 | tr -d ' ')" = 0
		check "pmu=off: empty ctrdesc" test "$(grep -A1 -F "# cat '#ε/ctrdesc'" "$TMPD/log0" | tail -1 | tr -d ' ')" = 0
		check "pmu=off: pmctest skips" grep -q '^SKIP pmc: no counters' "$TMPD/log0"
	}
fi
done_tests
