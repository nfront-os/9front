#!/bin/bash
# winit on 9front (sys/src/rust/crates: winit, softbuffer, raw-window-handle with
# plan9 backends; p9win): winittest and eguitest in windows of wsys on a VESA
# screen without a GPU driver, in a guest of scripts/guest/mcp-guest.sh; the
# host types into them through QEMU (QMP) while their -e runs log the events.
# usage: tests/winit/run.sh   (needs ../9front-rust's stage-1 rustc, cargo; a build: wsys)
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
. "$ROOT/scripts/mesa/common.sh"
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
RUSTC=${RUSTC:-$ROOT/../9front-rust/rust/build/host/stage1/bin/rustc}
T=x86_64-unknown-plan9
(cd "$HERE" && env RUSTC="$RUSTC" CARGO_TARGET_DIR="$BUILD/cargo/tests_winit" \
	CARGO_TARGET_X86_64_UNKNOWN_PLAN9_LINKER="$CROSS/$TRIPLE-gcc" \
	cargo build --offline --release --target $T --quiet)
mkdir -p "$HERE/root/tests"
for p in winittest eguitest; do cp "$BUILD/cargo/tests_winit/$T/release/$p" "$HERE/root/tests/"; done
cp "$ROOT/amd64/bin/wsys" "$HERE/root/tests/wsys"
cp "$HERE/winit.rc" "$HERE/root/tests/winit"
chmod +x "$HERE/root/tests/winit"

UFS_EXTRA=$HERE/root MEM=1024 KARGS="vgasize=1024x768x32 monitor=vesa mouseport=ps2" \
	setsid bash "$ROOT/scripts/guest/mcp-guest.sh" 'aux/realemu' 'aux/vga -m vesa -l 1024x768x32' \
	"bind -a '#i' /dev" "bind -a '#m' /dev" >"$CLI_DIR/winitguest.log" 2>&1 &
guest=$!
trap 'kill -TERM -- -$guest 2>/dev/null || kill $guest 2>/dev/null; wait $guest 2>/dev/null || true' EXIT
M="python3 $ROOT/scripts/guest/mcp.py"
$M wait 600 >/dev/null
$M rc '/tests/wsys -x -i /tests/winit >/tmp/wsys.log >[2=1] </dev/null' 240 >"$CLI_DIR/winit.out" &
run=$!
keys() { # ready-file key...
	for i in $(seq 400); do $M rc "test -e $1" 10 >/dev/null 2>&1 && break; sleep 0.2; done
	shift
	python3 - "$CLI_DIR/qmp.sock" "$@" <<'PY'
import json, socket, sys, time
s = socket.socket(socket.AF_UNIX); s.connect(sys.argv[1]); f = s.makefile('rw'); f.readline()
def cmd(c):
    f.write(json.dumps(c) + '\n'); f.flush()
    while True:
        r = json.loads(f.readline())
        if 'return' in r or 'error' in r: return r
cmd({'execute': 'qmp_capabilities'})
for k in sys.argv[2:]:
    cmd({'execute': 'send-key', 'arguments': {'keys': [{'type': 'qcode', 'data': x} for x in k.split('+')]}})
    time.sleep(0.2)
PY
}
keys /tmp/winite.ready a shift+b ret
keys /tmp/eguie.ready h shift+i
wait $run || true
$M rc 'cat /tmp/wsys.log' 10 | grep -v -e '/srv/wsys' -e '^event' -e '^ok' -e '^winit' -e '^egui' || true
out=$(cat "$CLI_DIR/winit.out"; printf '%s\n' 'cat /tmp/winit.log /tmp/egui.log' "grep '^event' /tmp/winite.log" \
	"grep '^(event|clicks|button)' /tmp/eguie.log" | $M rc - 10) || true
echo "$out" | grep -E '^(ok|FAIL|winittest|eguitest)' || true
[ -n "$WINITDEBUG" ] && echo "$out"
fail=0
check() { if echo "$out" | grep -q -- "$2"; then echo "ok $1"; else echo "FAIL $1 (no \"$2\")"; fail=1; fi; }
check "winittest" '^winittest: all passed'
check "eguitest" '^eguitest: all passed'
check "winit: motion in window coordinates" '^event motion 196,146'
check "winit: button 1" '^event button Left down'
check "winit: button 3" '^event button Right down'
check "winit: the wheel" '^event wheel 1'
check "winit: a down, text a" '^event key down Code(KeyA) Character("a") text "a"'
check "winit: shift" '^event key down Code(ShiftLeft) Named(Shift)'
check "winit: modifiers" '^event modifiers ModifiersState(SHIFT)'
check "winit: B with shift, text B" '^event key down Code(KeyB) Character("B") text "B"'
check "winit: return" '^event key down Code(Enter) Named(Enter)'
check "egui: button clicked" '^event clicked 1'
check "egui: text typed" '^event text hI'
kill -TERM -- -$guest 2>/dev/null || kill $guest 2>/dev/null
wait $guest 2>/dev/null || true
trap - EXIT
[ $fail = 0 ] && echo "winit: all passed" || echo "winit: failed"
exit $fail
