#!/bin/bash
# nix pmc: NIX performance counters (port/devpmc.c, pmc(3), pmc(2)): boot
# with *nixac=1 on 4 processors (cpu3 the AC), run tests/nix/pmctest.c on
# TC 1 (wired) and on the AC, each also reading and setting a counter of
# another core, and two processes reading the AC's counter at once.
# x86 guests get KVM's virtual counters (boot-cli.sh PMU=1), then a
# second boot without them must show none; arm64 (TCG) has PMUv3 always.
# SKIP where the virtual processor has no counters. 386 has no NIX.
# The readers count the AC's user mode (its process): arm64's counters
# are 32 bits, the AC's idle loop in the kernel would wrap one in 4 s.
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
case $objtype in 386) skip "nix pmc" "no application cores on 386"; done_tests; exit ;; esac
[ -f "$ROOT/$KBIN" ] || { skip "nix pmc" "no kernel $KBIN, run $BUILDHOW"; done_tests; exit; }
[ -x "$BUILDEXE" ] || { skip "nix pmc" "no build tool $BUILDEXE"; done_tests; exit; }

buildtests(){ (cd "$HERE" && env PLAN9="$ROOT" ARCH=$objtype CC=gcc "$BUILDEXE" build.json); }
check "build tests/nix with the port toolchain" buildtests || { done_tests; exit; }

NCPU=4 ac=3
boot(){
	PMU=1 SMP=$NCPU KARGS="${KARGS:+$KARGS }*ncpu=$NCPU *nixac=1" "$ROOT/scripts/guest/boot-cli.sh" \
		"ls '#ε' | wc -l" \
		"cat '#ε/ctrdesc' | wc -l" \
		"/tests/pmctest tc $ac -s" \
		"execac -c $ac /tests/pmctest ac 0" \
		"d='#ε/core000$ac'; echo reset > \$d/ctr00ctl; echo user > \$d/ctr00ctl; echo set cycles > \$d/ctr00ctl; echo enable > \$d/ctr00ctl; execac -c $ac /tests/pmctest stress $ac 1000 > /tmp/s1 & /tests/pmctest stress $ac 1000 > /tmp/s2; wait; echo stress \`{cat /tmp/s1 /tmp/s2}" \
		"execac -c $ac /tests/pmctest quiet 0" \
		"/tests/pmctest stress $ac 1500 > /tmp/s3 & sleep 0.2; execac -c $ac /tests/pmctest quiet 1000; wait; cat /tmp/s3" \
		> "$TMPD/console" 2>&1
	tr -d '\r' < "$TMPD/console" > "$TMPD/log"
}
check "boot with *nixac=1 on $NCPU processors" boot || { done_tests; exit; }
L=$TMPD/log
[ -z "$PMCLOG" ] || cp "$L" "$PMCLOG"	# env PMCLOG: keep the console log
out(){ grep -A1 -F "# $1" "$L" | tail -1; }

check "#ε lists ctrdesc, core and the $NCPU cores" test "$(out "ls '#ε'" | tr -d ' ')" -eq $((NCPU+2))
if grep -q '^SKIP pmc:' "$L"; then
	skip "nix pmc" "$(grep -m1 '^SKIP pmc:' "$L" | sed 's/^SKIP pmc: //') on this virtual processor"
else
	grep -E '^(counters|event) ' "$L" | sort -u | sed 's/^/       /'
	for who in tc ac; do
		sect=$(awk -v w="pmctest $who" 'index($0, w) && /^fe80|^[^ ]*# /{p=1; next} p && /^[^ ]*# /{exit} p' "$L")
		while read -r st name; do
			case $st in
			ok)	pass "pmctest on the ${who^^}: $name" ;;
			FAIL)	fail "pmctest on the ${who^^}: $name" ;;
			SKIP)	skip "pmctest on the ${who^^}: $name" "" ;;
			esac
		done < <(grep -E '^(ok|FAIL|SKIP) ' <<< "$sect")
		check "pmctest on the ${who^^}: all checks ran (23)" test "$(grep -cE '^(ok|FAIL) ' <<< "$sect")" -eq 23
	done
	check "two processes read the AC's counter at once" bash -c "grep -qE '^stress ok stress [0-9]+ ok stress [0-9]+\$' '$L'"
	# the AC serves the requests without a round trip of its process
	base=$(out "execac -c $ac /tests/pmctest quiet 0" | awk '{print $2}')
	sect=$(grep -A3 -F "execac -c $ac /tests/pmctest quiet 1000" "$L")
	rt=$(grep -m1 '^roundtrips' <<< "$sect" | awk '{print $2}')
	nreq=$(grep -m1 '^ok stress' <<< "$sect" | awk '{print $3}')
	echo "       roundtrips $base idle, $rt during $nreq requests"
	check "requests for the AC: no round trips of its process" test -n "$rt" -a -n "$base" -a -n "$nreq" -a "${rt:-99}" -le "$((${base:-0}+2))" -a "${nreq:-0}" -ge 100
fi
check "no kernel panic" bash -c "! grep -qE '^panic:|acexit: no process' '$L'"

# without KVM's virtual counters: the device is there, with no counters
if [ $objtype = amd64 ]; then
	boot0(){
		SMP=$NCPU KARGS="${KARGS:+$KARGS }*ncpu=$NCPU *nixac=1" "$ROOT/scripts/guest/boot-cli.sh" \
			"ls '#ε/core0000' | wc -l" \
			"cat '#ε/ctrdesc' | wc -l" \
			"/tests/pmctest tc $ac" \
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
