#!/bin/bash
# ui: wsys (sys/src/cmd/wsys, ../9front-ui stage 3), the window system's processor
# backend, on a VESA screen without a GPU driver (aux/realemu, aux/vga -m vesa):
# wsys -x -i runs tests/ui/wsystest, a client in plain Plan 9 C. It makes two
# windows through /mnt/wsys/clone, writes their pixels into the segments ctl names,
# presents them and reads the screen back through draw(3): colours, border, focus,
# stacking, a click (/dev/mousein), a key (/dev/kbdin, through kbdfs's cons: wsys
# runs in the foreground, the console's rc does not read meanwhile), a resize (a new
# segment), part of a window presented, a window gone with its clone fid. Then rio4.rc:
# stage 4, rio(4)'s files: drawtest (event(2)) started with window(1), its click,
# key, resize (wctl) and delete, and rio in a window of wsys with a shell typed into
# (uipeek -k: kbd(3) messages). text.rc: wsys' own text windows, a shell in one
# typed into (its output on the screen: uipeek -n counts the text's pixels),
# backspace, Del, drawtest started from it and quit. sel.rc: button 1 selects (the
# pale on the screen), button 2's menu snarfs, pastes, cuts, sends. menu3.rc: button
# 3's menu: New with a shell, Move, Resize, Hide and back, Delete. scroll.rc: double
# clicks (a word, a line, in brackets) and the scroll bar. hold.rc: look, hold mode
# (Esc), plumb (a plumber with plumbrules, started before wsys). term.rc: term
# (sys/src/cmd/term) with rc on a pty of ptyfs, typed into (its screen's text
# from TERMDEBUG=screen), button 1, 2 and the wheel; termrestart.rc: term in a
# window of a wsys killed twice goes on in the next one (p9win); theme.rc: the
# theme changed through /mnt/wsys/theme, the screen's colours read back. Their output
# goes to files until the end: the kernel's console draws on the screen the test reads.
# A second boot runs the same in a rio window.
# Needs ../9front-rust's stage-1 rustc for wsys (scripts/build/cargo.sh).
# usage: [objtype=386] bash tests/ui/run.sh   (amd64, 386: a 9pc guest; the
# programs without their GPU features there, as the build's step rust makes them)
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
case $objtype in amd64|386) ;; *) skip "ui tests" "objtype=$objtype: no VESA screen in its guest"; done_tests; exit ;; esac
[ -f "$ROOT/$KBIN" ] || { skip "ui" "no kernel, run $BUILDHOW"; done_tests; exit; }
# -n where the build makes no Mesa (scripts/gen/genbuild.py)
nogpu=$(python3 -c "import sys; sys.path.insert(0, '$ROOT/scripts/gen'); import apeprogs; print('' if '$objtype' in apeprogs.OBJTYPES else '-n')")

check "build wsys (scripts/build/cargo.sh)" bash "$ROOT/scripts/build/cargo.sh" $nogpu "$ROOT/sys/src/cmd/wsys" wsys || { done_tests; exit; }
check "build term (scripts/build/cargo.sh)" bash "$ROOT/scripts/build/cargo.sh" $nogpu "$ROOT/sys/src/cmd/term" term || { done_tests; exit; }
check "build wbar (scripts/build/cargo.sh)" bash "$ROOT/scripts/build/cargo.sh" $nogpu "$ROOT/sys/src/cmd/wbar" wbar || { done_tests; exit; }
check "build wdock (scripts/build/cargo.sh)" bash "$ROOT/scripts/build/cargo.sh" $nogpu "$ROOT/sys/src/cmd/wdock" wdock || { done_tests; exit; }
buildtests(){ (cd "$HERE" && env PLAN9="$ROOT" ARCH=$objtype CC=gcc "$BUILDEXE" build.json); }
check "build tests/ui" buildtests || { done_tests; exit; }
mkdir -p "$HERE/root/tests"
cp "$ROOT/$objtype/bin/wsys" "$HERE/root/tests/wsys"
cp "$ROOT/$objtype/bin/term" "$HERE/root/tests/term"
cp "$HERE/term.rc" "$HERE/root/tests/termtest"
cp "$HERE/termrestart.rc" "$HERE/root/tests/termrestart"
cp "$HERE/theme.rc" "$HERE/root/tests/themetest"
cp "$HERE/desk.rc" "$HERE/root/tests/desktest"
cp "$HERE/title.rc" "$HERE/root/tests/titletest"
cp "$HERE/wbar.rc" "$HERE/root/tests/wbartest"
cp "$HERE/full.rc" "$HERE/root/tests/fulltest"
cp "$ROOT/$objtype/bin/wbar" "$HERE/root/tests/wbar"
cp "$HERE/wdock.rc" "$HERE/root/tests/wdocktest"
cp "$ROOT/$objtype/bin/wdock" "$HERE/root/tests/wdock"
cp "$HERE/rio4.rc" "$HERE/root/tests/rio4"
cp "$HERE/text.rc" "$HERE/root/tests/text"
cp "$HERE/sel.rc" "$HERE/root/tests/sel"
cp "$HERE/menu3.rc" "$HERE/root/tests/menu3"
cp "$HERE/scroll.rc" "$HERE/root/tests/scroll"
cp "$HERE/hold.rc" "$HERE/root/tests/hold"
cp "$HERE/plumbrules" "$HERE/root/tests/plumbrules"
chmod +x "$HERE/root/tests/termtest" "$HERE/root/tests/termrestart" "$HERE/root/tests/themetest" "$HERE/root/tests/desktest" "$HERE/root/tests/titletest" "$HERE/root/tests/wbartest" "$HERE/root/tests/fulltest" "$HERE/root/tests/wdocktest" "$HERE/root/tests/rio4" "$HERE/root/tests/text" "$HERE/root/tests/sel" "$HERE/root/tests/menu3" "$HERE/root/tests/scroll" "$HERE/root/tests/hold"

# WSYSFLAGS: more flags for wsys (-d: its own devdraw)
boot(){
	UFS_EXTRA="$HERE/root" MEM=1024 KARGS="vgasize=1024x768x32 monitor=vesa mouseport=ps2" \
	SCREENSHOT="$TMPD/screen.png" \
		"$ROOT/scripts/guest/boot-cli.sh" \
		'aux/realemu' 'aux/vga -m vesa -l 1024x768x32' "bind -a '#i' /dev" "bind -a '#m' /dev" \
		"/tests/wsys ${WSYSFLAGS:-} -x -i '/tests/wsystest >/tmp/wsystest.log >[2=1]' >/tmp/wsys.log >[2=1]; cat /tmp/wsys.log /tmp/wsystest.log" \
		"/tests/wsys ${WSYSFLAGS:-} -x -i /tests/rio4 >/tmp/rio4.log >[2=1]; cat /tmp/rio4.log" \
		"/tests/wsys ${WSYSFLAGS:-} -x -i /tests/text >/tmp/text.log >[2=1]; cat /tmp/text.log" \
		"/tests/wsys ${WSYSFLAGS:-} -x -i /tests/sel >/tmp/sel.log >[2=1]; cat /tmp/sel.log" \
		"/tests/wsys ${WSYSFLAGS:-} -x -i /tests/menu3 >/tmp/menu3.log >[2=1]; cat /tmp/menu3.log" \
		"/tests/wsys ${WSYSFLAGS:-} -x -i /tests/scroll >/tmp/scroll.log >[2=1]; cat /tmp/scroll.log" \
		"plumber -p /tests/plumbrules >/dev/null >[2=1] </dev/null; /tests/wsys ${WSYSFLAGS:-} -x -i /tests/hold >/tmp/hold.log >[2=1] </dev/null; cat /tmp/hold.log" \
		"/tests/wsys ${WSYSFLAGS:-} -x -i /tests/termtest >/tmp/termtest.log >[2=1]; cat /tmp/termtest.log" \
		"/tests/termrestart >/tmp/termrestart.log >[2=1]; cat /tmp/termrestart.log" \
		"/tests/wsys ${WSYSFLAGS:-} -x -i /tests/themetest >/tmp/themetest.log >[2=1]; cat /tmp/themetest.log" \
		"/tests/wsys ${WSYSFLAGS:-} -x -i /tests/desktest >/tmp/desktest.log >[2=1]; cat /tmp/desktest.log" \
		"/tests/wsys ${WSYSFLAGS:-} -x -i /tests/titletest >/tmp/titletest.log >[2=1]; cat /tmp/titletest.log" \
		"/tests/wsys ${WSYSFLAGS:-} -x -i /tests/wbartest >/tmp/wbartest.log >[2=1]; cat /tmp/wbartest.log" \
		"/tests/wsys ${WSYSFLAGS:-} -x -i /tests/fulltest >/tmp/fulltest.log >[2=1]; cat /tmp/fulltest.log" \
		"/tests/wsys ${WSYSFLAGS:-} -x -i /tests/wdocktest >/tmp/wdocktest.log >[2=1]; cat /tmp/wdocktest.log" \
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
check "rio4 (draw(3) programs, window(1), rio in wsys): all passed" grep -q '^rio4: all passed' "$TMPD/log"
check "text windows (a shell typed into, backspace, Del, a draw(3) program and back): all passed" grep -q '^text: all passed' "$TMPD/log"
check "selecting and snarfing (button 1, button 2's cut paste snarf send): all passed" grep -q '^sel: all passed' "$TMPD/log"
check "button 3's menu (New, Move, Resize, Hide and back, Delete): all passed" grep -q '^menu3: all passed' "$TMPD/log"
check "double clicks (word, line, brackets) and the scroll bar (1 held, 3, 2): all passed" grep -q '^scroll: all passed' "$TMPD/log"
check "look, hold mode and plumb (the plumber started before wsys): all passed" grep -q '^hold: all passed' "$TMPD/log"
check "term: rc on a pty typed into, colours, size, Del, alternate screen, select, paste, scroll back, exit: all passed" grep -q '^term: all passed' "$TMPD/log"
check "term outlives its wsys: its window again in the next (p9win), twice: all passed" grep -q '^termrestart: all passed' "$TMPD/log"
check "theme: classic, modern written to /mnt/wsys/theme at once, read back, a wrong line refused, one line: all passed" grep -q '^theme: all passed' "$TMPD/log"
check "desk: wsys/ in a window, winwatch, Mod4+Tab and back, Tab without Mod4: all passed" bash -c "grep -q '^desk: all passed' '$TMPD/log' || { grep -E '^(ok|FAIL) ' '$TMPD/log' | tail -5; false; }"
check "title bars: frame and content, moved by the bar, double click and max, min, x, title 0 and 1: all passed" bash -c "grep -q '^title: all passed' '$TMPD/log' || { grep -E '^(ok|FAIL) ' '$TMPD/log' | tail -5; false; }"
check "wbar: a bar on top, its windows (current, hide, back), maximized below it, Mod4+Tab passes it, its menu: all passed" bash -c "grep -q '^wbar: all passed' '$TMPD/log' || { grep -E '^(ok|FAIL) ' '$TMPD/log' | tail -5; false; }"
check "wdock: a bar along the bottom, its starters in the middle (button 1 runs, current, hides, back, the next; button 2 another), maximized above it: all passed" bash -c "grep -q '^wdock: all passed' '$TMPD/log' || { grep -E '^(ok|FAIL) ' '$TMPD/log' | tail -5; false; }"
check "fullscreen: all of the screen, above the bar, another window over it, unfullscreen gives the frame back: all passed" bash -c "grep -q '^full: all passed' '$TMPD/log' || { grep -E '^(ok|FAIL) ' '$TMPD/log' | tail -5; false; }"
check "wsys posted /srv/wsys.glenda.pid on the 1024x768 screen" grep -qE '^wsys: /srv/wsys.glenda.[0-9]+ 0 0 1024 768' "$TMPD/log"
check "no kernel panic" bash -c "! grep -q '^panic:' '$TMPD/log'"

# a terminal boot with desktop=wsys: the profile starts wsys instead of rio, with
# riostart (wbar, stats, a shell, the console's window: boot-cli sets console); on a
# 16-bit VESA screen. The command is typed on the serial line, through kbdfs and
# wsys into the window last made (wctl is read once: a second read waits).
desk(){
	SERVICE=terminal KARGS="vgasize=1024x768x16 monitor=vesa mouseport=ps2 desktop=wsys" \
	SCREENSHOT="$TMPD/desk.png" CMDWAIT=30 \
		"$ROOT/scripts/guest/boot-cli.sh" \
		"sleep 3;{ps;dd -if /dev/wctl -bs 128 -count 1 -quiet 1;echo;mount \$wsys /tmp none;ls /tmp}>'#c/cons'" \
		> "$TMPD/consoledesk" 2>&1
	tr -d '\r' < "$TMPD/consoledesk" > "$TMPD/logdesk"
}
check "terminal boot with desktop=wsys" desk
cp "$TMPD/logdesk" "$HERE/desk.log" 2>/dev/null
cp "$TMPD/desk.png" "$HERE/desk.png" 2>/dev/null
check "desktop: wsys running, rio not" bash -c "grep -qE ' wsys\$' '$TMPD/logdesk' && ! grep -qE ' rio\$' '$TMPD/logdesk'"
check "desktop: riostart's stats in a window" grep -qE ' stats$' "$TMPD/logdesk"
check "desktop: the command typed into the focused window ran (its wctl: current)" grep -qE '^ +[0-9]+ +[0-9]+ +[0-9]+ +[0-9]+ +current +visible' "$TMPD/logdesk"
check "desktop: riostart's wbar running" grep -qE ' wbar$' "$TMPD/logdesk"
check "desktop: four windows (wbar, stats, a shell, the console)" bash -c "grep -c '^/tmp/[0-9]' '$TMPD/logdesk' | grep -qx 4"
check "desktop: screen dump 1024x768" python3 -c "
import struct, sys
d = open('$TMPD/desk.png', 'rb').read(24)
sys.exit(struct.unpack('>II', d[16:24]) != (1024, 768))"
check "no kernel panic" bash -c "! grep -q '^panic:' '$TMPD/logdesk'"

# second boot: the same test with wsys in a rio window (rioinit.rc, wsysrio.rc): its
# screen is the window, mouse and keys come through rio. After rio, rc gets nothing
# from the serial line: the scripts log to $home, which is on the host (ufsroot).
cp "$HERE/rioinit.rc" "$HERE/root/tests/rioinit"
cp "$HERE/wsysrio.rc" "$HERE/root/tests/wsysrio"
cp "$HERE/wsysresize.rc" "$HERE/root/tests/wsysresize"
chmod +x "$HERE/root/tests/rioinit" "$HERE/root/tests/wsysrio" "$HERE/root/tests/wsysresize"
out=$CLI_DIR/ufsroot/usr/glenda/wsysrio.out
log=$CLI_DIR/ufsroot/usr/glenda/wsysrio.log
rlog=$CLI_DIR/ufsroot/usr/glenda/wsysresize.log
H=$CLI_DIR/ufsroot/usr/glenda
rm -f "$out" "$log" "$rlog" "$H/wsysrioz.log" "$H/wsysresizez.log" "$H/wsysresizek.log"
UFS_EXTRA="$HERE/root" MEM=1024 KARGS="vgasize=1024x768x32 monitor=vesa mouseport=ps2" \
CMDWAIT=5 WAITFOR='never shown' WAITMAX=600 \
	"$ROOT/scripts/guest/boot-cli.sh" \
	'aux/realemu' 'aux/vga -m vesa -l 1024x768x32' "bind -a '#i' /dev" "bind -a '#m' /dev" \
	'rio -i /tests/rioinit' > "$TMPD/consolerio" 2>&1 &
riopid=$!
for _ in $(seq 300); do
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
cp "$rlog" "$HERE/resize.log" 2>/dev/null
check "rio: wsys follows its window (smaller: the bar, a window back on it; bigger; moved): all passed" grep -q '^resize: all passed' "$rlog"
grep '^FAIL' "$rlog" 2>/dev/null | sed 's/^/       /'
check "rio, -z (the screen copied compressed, as to drawterm): wsystest: all passed" grep -q '^wsystest: all passed' "$H/wsysrioz.log"
check "rio, -z: wsys follows its window: all passed" grep -q '^resize: all passed' "$H/wsysresizez.log"
grep -h '^FAIL' "$H/wsysrioz.log" "$H/wsysresizez.log" 2>/dev/null | sed 's/^/       /'
# what -z saved: the pixels' bytes and the messages' (WSYSDEBUG=compress), each run
ratio(){
	grep '^wsys: compressed' "$out" | awk '{ if ($8 == 0) exit 1; r = $3 / $8; printf "%d into %d: %.1f to 1\n", $3, $8, r; if (r < 4) bad = 1 } END { exit bad }'
}
check "rio, -z: compressed 4 to 1 at least" ratio
ratio | sed 's/^/       /'
check "rio, -k (the kernel's devdraw): wsys follows its window: all passed" grep -q '^resize: all passed' "$H/wsysresizek.log"
grep '^FAIL' "$H/wsysresizek.log" 2>/dev/null | sed 's/^/       /'
# a third boot: wsys through rcpu (rcpu.rc, rcpuwin.rc), terminal and cpu
# server in one guest; keys for an auth server of its own from a test password
mkdir -p "$HERE/root/tests/rcpukeys"
python3 - "$HERE/root/tests/rcpukeys" <<'PY'
import hashlib, sys
d, pw = sys.argv[1], 'rcputest'
dom, user = b'rcpu-test', b'glenda'
# passtodeskey of at most 8 characters (no DES rounds), passtoaeskey
buf = bytearray(b' ' * 28); p = pw.encode(); buf[:len(p)] = p; buf[len(p)] = 0
des = bytes(((buf[i] >> i) + (buf[i + 1] << (8 - (i + 1)))) & 0xFF for i in range(7))
aes = hashlib.pbkdf2_hmac('sha1', p, b'Plan 9 key derivation', 9001, 16)
cs = lambda b: bytes([(9 + sum(b)) & 0xFF])
pad = lambda b, n: b + bytes(n - len(b))
# Nvrsafe: machkey authkey config authid authdom aesmachkey, each with its sum
nv = des + cs(des) + des + cs(des) + bytes(14) + cs(bytes(14)) + pad(user, 28) + cs(pad(user, 28)) + pad(dom, 48) + cs(pad(dom, 48)) + aes + cs(aes)
for name, data in (('nvram', nv), ('deskey', des), ('aeskey', aes), ('pw', p)):
    open(f'{d}/{name}', 'wb').write(data)
PY
cp "$HERE/rcpu.rc" "$HERE/root/tests/rcputest"
cp "$HERE/rcpuwin.rc" "$HERE/root/tests/rcpuwin"
chmod +x "$HERE/root/tests/rcputest" "$HERE/root/tests/rcpuwin"
rcpuboot(){
	UFS_EXTRA="$HERE/root" MEM=1024 KARGS="vgasize=1024x768x32 monitor=vesa mouseport=ps2" CMDWAIT=10 \
		"$ROOT/scripts/guest/boot-cli.sh" \
		'aux/realemu' 'aux/vga -m vesa -l 1024x768x32' "bind -a '#i' /dev" "bind -a '#m' /dev" \
		"/tests/rcputest >/tmp/rcputest.log >[2=1]; cat /tmp/rcputest.log" \
		> "$TMPD/consolercpu" 2>&1
	tr -d '\r' < "$TMPD/consolercpu" > "$TMPD/logrcpu"
}
check "boot for rcpu (terminal and cpu server in one)" rcpuboot
cp "$TMPD/logrcpu" "$HERE/rcpu.log" 2>/dev/null
check "rcpu: wsys on the terminal's screen through /mnt/term, compressed, follows its 800x600: all passed" bash -c "grep -q '^rcpu: all passed' '$TMPD/logrcpu' || { grep -E '^(ok|FAIL) ' '$TMPD/logrcpu' | tail -8; false; }"
done_tests
