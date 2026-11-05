#!/bin/bash
# ui: wsys (sys/src/cmd/wsys, ../9front-ui stage 3), the window system's processor
# backend, on a VESA screen without a GPU driver (aux/realemu, aux/vga -m vesa):
# wsys -x -i runs tests/ui/wsystest, a client in plain Plan 9 C. It makes two
# windows through /mnt/wsys/clone, writes their pixels into the segments ctl names,
# presents them and reads the screen back through draw(3): colours, border, focus,
# stacking, a click (/dev/mousein), a key (/dev/kbdin, through kbdfs's cons: wsys
# runs in the foreground, the console's rc does not read meanwhile), a resize (a new
# segment), part of a window presented, a window gone with its clone fid. Their output
# goes to files until the end: the kernel's console draws on the screen the test reads.
# A second boot runs the same in a rio window.
# Needs ../9front-rust's stage-1 rustc for wsys (scripts/build/cargo.sh).
# usage: bash tests/ui/run.sh   (amd64)
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
[ "$objtype" = amd64 ] || { skip "ui tests" "objtype=$objtype: cargo.sh builds amd64 and 386 only"; done_tests; exit; }
[ -f "$ROOT/amd64/bin/9pc64" ] || { skip "ui" "no kernel, run $BUILDHOW"; done_tests; exit; }

check "build wsys (scripts/build/cargo.sh)" bash "$ROOT/scripts/build/cargo.sh" "$ROOT/sys/src/cmd/wsys" wsys || { done_tests; exit; }
buildtests(){ (cd "$HERE" && env PLAN9="$ROOT" ARCH=$objtype CC=gcc "$BUILDEXE" build.json); }
check "build tests/ui" buildtests || { done_tests; exit; }
mkdir -p "$HERE/root/tests"
cp "$ROOT/amd64/bin/wsys" "$HERE/root/tests/wsys"

boot(){
	UFS_EXTRA="$HERE/root" MEM=1024 KARGS="vgasize=1024x768x32 monitor=vesa mouseport=ps2" \
	SCREENSHOT="$TMPD/screen.png" \
		"$ROOT/scripts/guest/boot-cli.sh" \
		'aux/realemu' 'aux/vga -m vesa -l 1024x768x32' "bind -a '#i' /dev" "bind -a '#m' /dev" \
		"/tests/wsys -x -i '/tests/wsystest >/tmp/wsystest.log >[2=1]' >/tmp/wsys.log >[2=1]; cat /tmp/wsys.log /tmp/wsystest.log" \
		> "$TMPD/console" 2>&1
	tr -d '\r' < "$TMPD/console" > "$TMPD/log"
}
check "boot with VESA, run wsys -x -i wsystest" boot
cp "$TMPD/log" "$HERE/console.log" 2>/dev/null
while read -r st rest; do
	case $st in
	ok)	pass "wsystest: $rest" ;;
	FAIL)	fail "wsystest: $rest" ;;
	esac
done < <(grep -E '^(ok|FAIL) ' "$TMPD/log")
check "wsystest: all passed" grep -q '^wsystest: all passed' "$TMPD/log"
check "wsys posted /srv/wsys.glenda.pid on the 1024x768 screen" grep -qE '^wsys: /srv/wsys.glenda.[0-9]+ 0 0 1024 768' "$TMPD/log"
check "no kernel panic" bash -c "! grep -q '^panic:' '$TMPD/log'"

# second boot: the same test with wsys in a rio window (rioinit.rc, wsysrio.rc): its
# screen is the window, mouse and keys come through rio. After rio, rc gets nothing
# from the serial line: the scripts log to $home, which is on the host (ufsroot).
cp "$HERE/rioinit.rc" "$HERE/root/tests/rioinit"
cp "$HERE/wsysrio.rc" "$HERE/root/tests/wsysrio"
chmod +x "$HERE/root/tests/rioinit" "$HERE/root/tests/wsysrio"
out=$CLI_DIR/ufsroot/usr/glenda/wsysrio.out
log=$CLI_DIR/ufsroot/usr/glenda/wsysrio.log
rm -f "$out" "$log"
UFS_EXTRA="$HERE/root" MEM=1024 KARGS="vgasize=1024x768x32 monitor=vesa mouseport=ps2" \
CMDWAIT=5 WAITFOR='never shown' WAITMAX=300 \
	"$ROOT/scripts/guest/boot-cli.sh" \
	'aux/realemu' 'aux/vga -m vesa -l 1024x768x32' "bind -a '#i' /dev" "bind -a '#m' /dev" \
	'rio -i /tests/rioinit' > "$TMPD/consolerio" 2>&1 &
riopid=$!
for _ in $(seq 150); do
	grep -q '^done' "$out" 2>/dev/null && break
	kill -0 $riopid 2>/dev/null || break
	sleep 2
done
kill $riopid 2>/dev/null
wait $riopid 2>/dev/null
cp "$log" "$HERE/rio.log" 2>/dev/null
check "rio: wsys in a window, wsystest: all passed" grep -q '^wsystest: all passed' "$log"
check "rio: wsys draws inside the window" grep -qE '^ok wsys draws in \[1[0-9] 1[0-9]\]' "$log"
grep '^FAIL' "$log" 2>/dev/null | sed 's/^/       /'
done_tests
