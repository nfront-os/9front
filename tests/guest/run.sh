#!/bin/bash
# guest: libc/libthread unit tests built with the port toolchain, run
# inside 9front under QEMU via scripts/boot-cli.sh. The same tests can
# be started by hand in a running system: /tests/run [test...]
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
[ -x "$CLI_DIR/build.exe" ] || { skip "guest tests" "no build tool, run scripts/build-cli.sh"; done_tests; exit; }

buildtests(){
	(cd "$HERE" && env PLAN9="$ROOT" ARCH=amd64 CPUS=$(nproc) CC=gcc TOOLPREFIX= TOOLPFX= \
		"$CLI_DIR/build.exe" build.json) &&
	install -m 755 "$HERE/run.rc" "$HERE/root/tests/run"
}
check "build tests/guest with the port toolchain" buildtests || { done_tests; exit; }

boot(){ BOOTWAIT=${BOOTWAIT:-45} CMDWAIT=${CMDWAIT:-20} "$ROOT/scripts/boot-cli.sh" /tests/run > "$TMPD/console" 2>&1; }
check "boot and run /tests/run" boot || { done_tests; exit; }
tr -d '\r' < "$TMPD/console" | sed -n '/^== /,/^tests: /p' > "$TMPD/log"

# report every check of the guest programs
while read -r st name; do
	case $st in
	ok)	pass "guest: $name" ;;
	FAIL)	fail "guest: $name" ;;
	esac
done < <(grep -E '^(ok|FAIL) ' "$TMPD/log")
grep -q '^tests: all passed' "$TMPD/log" || { fail "guest: /tests/run summary"; grep -E '^(tests|libctest|threadtest):' "$TMPD/log" | sed 's/^/       /'; }
done_tests
