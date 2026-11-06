#!/bin/bash
# SDL3 on 9front (sys/src/sdl3): sdltest in a window of wsys on a VESA screen
# without a GPU driver, in a guest of scripts/guest/mcp-guest.sh; the host
# types into it through QEMU (QMP) while sdltest -e logs the events.
# usage: tests/sdl/run.sh   (needs libSDL3.a built: scripts/sdl3/vendor.sh, build.exe)
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
. "$ROOT/scripts/mesa/common.sh"
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
mkdir -p "$HERE/root/tests"
"$CROSS/$TRIPLE-gcc" -std=gnu11 -O2 -Wall -o "$HERE/root/tests/sdltest" "$HERE/sdltest.c" -lSDL3 -ldraw -l9 -lfmt -lutf -lm
cp "$ROOT/amd64/bin/wsys" "$HERE/root/tests/wsys"
cp "$HERE/sdl.rc" "$HERE/root/tests/sdl"
chmod +x "$HERE/root/tests/sdl"

UFS_EXTRA=$HERE/root MEM=1024 KARGS="vgasize=1024x768x32 monitor=vesa mouseport=ps2" \
	setsid bash "$ROOT/scripts/guest/mcp-guest.sh" 'aux/realemu' 'aux/vga -m vesa -l 1024x768x32' \
	"bind -a '#i' /dev" "bind -a '#m' /dev" >"$CLI_DIR/sdlguest.log" 2>&1 &
guest=$!
trap 'kill -TERM -- -$guest 2>/dev/null || kill $guest 2>/dev/null; wait $guest 2>/dev/null || true' EXIT
M="python3 $ROOT/scripts/guest/mcp.py"
$M wait 600 >/dev/null
$M rc '/tests/wsys -x -i /tests/sdl >/tmp/wsys.log >[2=1] </dev/null' 180 >"$CLI_DIR/sdl.out" &
run=$!
# the keys, once sdltest -e waits for them
for i in $(seq 300); do $M rc 'test -e /tmp/sdle.ready' 10 >/dev/null 2>&1 && break; sleep 0.2; done
python3 - "$CLI_DIR/qmp.sock" <<'PY'
import json, socket, sys, time
s = socket.socket(socket.AF_UNIX); s.connect(sys.argv[1]); f = s.makefile('rw'); f.readline()
def cmd(c):
    f.write(json.dumps(c) + '\n'); f.flush()
    while True:
        r = json.loads(f.readline())
        if 'return' in r or 'error' in r: return r
cmd({'execute': 'qmp_capabilities'})
for keys in (['a'], ['shift', 'b'], ['ret']):
    cmd({'execute': 'send-key', 'arguments': {'keys': [{'type': 'qcode', 'data': k} for k in keys]}})
    time.sleep(0.2)
PY
wait $run || true
$M rc 'cat /tmp/wsys.log' 10 | grep -v -e '/srv/wsys' -e '^event' -e '^ok' -e '^sdl' || true
# (rc: ^ joins words, the pattern quoted)
out=$(cat "$CLI_DIR/sdl.out"; printf '%s\n' 'cat /tmp/sdl.log' "grep '^event' /tmp/sdle.log" | $M rc - 10) || true
echo "$out" | grep -E '^(ok|FAIL|sdltest)'
[ -n "$SDLDEBUG" ] && echo "$out"
fail=0
check() { if echo "$out" | grep -q -- "$2"; then echo "ok $1"; else echo "FAIL $1 (no \"$2\")"; fail=1; fi; }
check "sdltest" '^sdltest: all passed'
check "mouse: motion in window coordinates" '^event motion 196,146'
check "mouse: button 1" '^event button 1 down at 196,146'
check "mouse: button 3" '^event button 3 down at 216,156'
check "mouse: the wheel" '^event wheel 1'
check "keyboard: a down" '^event key down A'
check "keyboard: text a" '^event text a'
check "keyboard: shift" '^event key down Left Shift'
check "keyboard: text B" '^event text B'
check "keyboard: return up" '^event key up Return'
[ $fail = 0 ] && echo "sdl: all passed" || echo "sdl: failed"
exit $fail
