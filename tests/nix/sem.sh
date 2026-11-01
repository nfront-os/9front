#!/bin/bash
# nix sem: NIX semaphores and tubes (port/syssem.c, port/devtube.c,
# upsem(2), tsend(2), tube(3)): boot with *nixac=$NAC on $NCPU processors
# (default 1 of 4), run tests/nix/semtest.c on a TC and on the last AC
# (its children stay on TCs), kill a process blocked in downsem from rc.
# amd64 and arm64; 386 has no NIX semaphores (port/nixnone.c).
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
case $objtype in 386) skip "nix sem" "no NIX semaphores on 386"; done_tests; exit ;; esac
[ -f "$ROOT/$KBIN" ] || { skip "nix sem" "no kernel $KBIN, run $BUILDHOW"; done_tests; exit; }
[ -x "$BUILDEXE" ] || { skip "nix sem" "no build tool $BUILDEXE"; done_tests; exit; }

buildtests(){ (cd "$HERE" && env PLAN9="$ROOT" ARCH=$objtype CC=gcc "$BUILDEXE" build.json); }
check "build tests/nix with the port toolchain" buildtests || { done_tests; exit; }

NCPU=${NCPU:-4} NAC=${NAC:-1}
ac=$((NCPU-1))
# with two ACs or more: semtest on two of them at once
par=()
[ "$NAC" -lt 2 ] || par=("execac -c $((ac-1)) /tests/semtest ac1 > /tmp/par1 & execac -c $ac /tests/semtest ac2 > /tmp/par2; wait; echo par \`{grep -c '^ok' /tmp/par1} \`{grep -c '^ok' /tmp/par2} \`{cat /tmp/par1 /tmp/par2 | grep -c '^FAIL'}; grep '^FAIL' /tmp/par1 /tmp/par2")
boot(){
	SMP=$NCPU KARGS="${KARGS:+$KARGS }*ncpu=$NCPU *nixac=$NAC" CMDWAIT=${CMDWAIT:-300} "$ROOT/scripts/guest/boot-cli.sh" \
		'/tests/semtest tc' \
		"execac -c $ac /tests/semtest ac" \
		"${par[@]}" \
		"ls '#='" \
		> "$TMPD/console" 2>&1
	tr -d '\r' < "$TMPD/console" > "$TMPD/log"
}
check "boot with *nixac=$NAC on $NCPU processors" boot || { done_tests; exit; }
L=$TMPD/log
out(){ grep -A1 -F "# $1" "$L" | tail -1; }

while read -r st where name; do
	case $st in
	ok)	pass "semtest on the $where: $name" ;;
	FAIL)	fail "semtest on the $where: $name" ;;
	esac
done < <(grep -E '^(ok|FAIL) (tc|ac) ' "$L")
n=$(grep -cE '^(ok|FAIL) tc ' "$L")
check "semtest on the TC: all checks ran (26)" test "$n" -eq 26
n=$(grep -cE '^(ok|FAIL) ac ' "$L")
check "semtest on the AC: all checks ran (26)" test "$n" -eq 26
check "semtest on the AC ran on core $ac" grep -q "^semtest ac on core $ac\$" "$L"
[ "$NAC" -lt 2 ] || check "semtest on cpu$((ac-1)) and cpu$ac at once" bash -c "grep -A12 -F '# execac -c $((ac-1)) /tests/semtest' '$L' | grep -E '^par|FAIL' > '$TMPD/par'; cat '$TMPD/par'; grep -q '^par 26 26 0\$' '$TMPD/par'"
check "#= lists the clone file" test "$(out "ls '#='")" = "'#=/ctl'"
check "no kernel panic" bash -c "! grep -qE '^panic:|acexit: no process' '$L'"
done_tests
