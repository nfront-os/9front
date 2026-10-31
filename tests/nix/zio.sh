#!/bin/bash
# zio: NIX zero-copy I/O (port/syszio.c, port/devzp.c, segment(3)
# kmsg/umsg): boot with *nixac=$NAC on $NCPU processors (default 1
# of 4), run tests/nix/ziotest.c on a TC and on the last AC
# (execac(1); its children, the other ends of the pipes, on a TC).
# amd64 and arm64; on 386 the kernel has no zp(3).
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
case $objtype in 386) skip "zio" "no zp(3) and application cores on 386"; done_tests; exit ;; esac
[ -f "$ROOT/$KBIN" ] || { skip "zio" "no kernel $KBIN, run $BUILDHOW"; done_tests; exit; }
[ -x "$BUILDEXE" ] || { skip "zio" "no build tool $BUILDEXE"; done_tests; exit; }

buildtests(){ (cd "$HERE" && env PLAN9="$ROOT" ARCH=$objtype CC=gcc "$BUILDEXE" build.json); }
check "build tests/nix with the port toolchain" buildtests || { done_tests; exit; }

NCPU=${NCPU:-4} NAC=${NAC:-1}
ac=$((NCPU-1))
NTC=23		# checks of ziotest on a TC, one more on an AC
boot(){
	SMP=$NCPU CMDWAIT=${CMDWAIT:-300} KARGS="${KARGS:+$KARGS }*ncpu=$NCPU *nixac=$NAC" "$ROOT/scripts/guest/boot-cli.sh" \
		'/tests/ziotest' \
		"execac -c $ac /tests/ziotest ac" \
		"ls '#∏'" \
		> "$TMPD/console" 2>&1
	tr -d '\r' < "$TMPD/console" > "$TMPD/log"
}
check "boot with *nixac=$NAC on $NCPU processors" boot || { done_tests; exit; }
L=$TMPD/log
# the output of the command line $1, up to rc's next prompt
block(){ awk -v c="# $1" 'on && /^[^ ]*# / { on = 0 } index($0, c) { on = 1; next } on' "$L"; }

run(){	# where expected-count command
	local where=$1 n=$2 cmd=$3
	block "$cmd" > "$TMPD/$where"
	while read -r st name; do
		case $st in
		ok)	pass "ziotest on the $where: $name" ;;
		FAIL)	fail "ziotest on the $where: $name" ;;
		esac
	done < <(grep -E '^(ok|FAIL) ' "$TMPD/$where")
	check "ziotest on the $where: all checks ran ($n)" test "$(grep -cE '^(ok|FAIL) ' "$TMPD/$where")" -eq $n
	check "ziotest on the $where: done, 0 failed" grep -q '^done 0 failed$' "$TMPD/$where"
}
run TC $NTC /tests/ziotest
run AC $((NTC+1)) "execac -c $ac /tests/ziotest ac"
zpdir(){ test "$(block "ls '#∏'" | grep -cE "^'#∏/data1?'\$")" -eq 2; }
check "#∏ has data and data1" zpdir
check "no kernel panic" bash -c "! grep -qE '^panic:|acexit: no process' '$L'"
done_tests
