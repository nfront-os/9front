#!/bin/bash
# big: segments of 2 MB pages (segattach(2) class "big", port/fault.c
# putmmubig): boot with *nixac=$NAC on $NCPU processors (default 1 of 4),
# run tests/nix/bigtest.c on a TC and on the last AC (execac(1); the
# forked child on a TC). amd64 and arm64; 386 has no 2 MB pages.
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
case $objtype in 386) skip "big" "no 2 MB pages on 386"; done_tests; exit ;; esac
[ -f "$ROOT/$KBIN" ] || { skip "big" "no kernel $KBIN, run $BUILDHOW"; done_tests; exit; }
[ -x "$BUILDEXE" ] || { skip "big" "no build tool $BUILDEXE"; done_tests; exit; }

buildtests(){ (cd "$HERE" && env PLAN9="$ROOT" ARCH=$objtype CC=gcc "$BUILDEXE" build.json); }
check "build tests/nix with the port toolchain" buildtests || { done_tests; exit; }

NCPU=${NCPU:-4} NAC=${NAC:-1}
ac=$((NCPU-1))
N=14		# checks of bigtest
boot(){
	SMP=$NCPU KARGS="${KARGS:+$KARGS }*ncpu=$NCPU *nixac=$NAC" "$ROOT/scripts/guest/boot-cli.sh" \
		'/tests/bigtest' \
		"execac -c $ac /tests/bigtest" \
		'/tests/bigtest > /dev/null && echo again ok' \
		> "$TMPD/console" 2>&1
	tr -d '\r' < "$TMPD/console" > "$TMPD/log"
	[ -z "$KEEPLOG" ] || cp "$TMPD/log" "$KEEPLOG"
}
check "boot with *nixac=$NAC on $NCPU processors" boot || { done_tests; exit; }
L=$TMPD/log
# the output of the command line $1, up to rc's next prompt
block(){ awk -v c="# $1" 'on && /^[^ ]*# / { on = 0 } index($0, c) { on = 1; next } on' "$L"; }

run(){	# where command
	local where=$1 cmd=$2
	block "$cmd" > "$TMPD/$where"
	while read -r st name; do
		case $st in
		ok)	pass "bigtest on the $where: $name" ;;
		FAIL)	fail "bigtest on the $where: $name" ;;
		esac
	done < <(grep -E '^(ok|FAIL) ' "$TMPD/$where")
	check "bigtest on the $where: all checks ran ($N)" test "$(grep -cE '^(ok|FAIL) ' "$TMPD/$where")" -eq $N
	check "bigtest on the $where: done, 0 failed" grep -q '^done 0 failed$' "$TMPD/$where"
}
run TC /tests/bigtest
run AC "execac -c $ac /tests/bigtest"
check "a second run: the memory came back" bash -c "grep -q '^again ok\$' '$L'"
check "no kernel panic" bash -c "! grep -qE '^panic:|acexit: no process|suicide' '$L'"
done_tests
