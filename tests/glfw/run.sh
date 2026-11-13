#!/bin/bash
# GLFW on 9front (sys/src/glfw): glfwtest in a window of wsys on a VESA screen
# without a GPU driver, in a guest of scripts/guest/mcp-guest.sh; the host
# types into it through QEMU (QMP) while glfwtest -e logs the events.  Its
# OpenGL and Vulkan (-g) run on the card: tests/gpu/amdglfw.rc. Then
# glfwagain.rc: a GLFW window that outlives its wsys, killed twice.
# usage: tests/glfw/run.sh   (needs libglfw3.a built: scripts/glfw/vendor.sh, build.exe)
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
. "$ROOT/scripts/mesa/common.sh"
. "$ROOT/scripts/lib/paths.sh"	# CLI_DIR, CACHE (dist/paths.env: nfront/cache/cli)
mkdir -p "$HERE/root/tests"
"$CROSS/$TRIPLE-gcc" -std=gnu11 -O2 -Wall -o "$HERE/root/tests/glfwtest" "$HERE/glfwtest.c" -lglfw3 -ldraw -l9 -lfmt -lutf -lm
"$CROSS/$TRIPLE-gcc" -std=gnu11 -O2 -Wall -o "$HERE/root/tests/glfwagain" "$HERE/glfwagain.c" -lglfw3 -ldraw -l9 -lfmt -lutf -lm
cp "$HERE/glfwagain.rc" "$HERE/root/tests/glfwagain.rc"
chmod +x "$HERE/root/tests/glfwagain.rc"
cp "$ROOT/amd64/bin/wsys" "$HERE/root/tests/wsys"
cp "$HERE/glfw.rc" "$HERE/root/tests/glfw"
chmod +x "$HERE/root/tests/glfw"

UFS_EXTRA=$HERE/root MEM=1024 KARGS="vgasize=1024x768x32 monitor=vesa mouseport=ps2" \
	setsid bash "$ROOT/scripts/guest/mcp-guest.sh" 'aux/realemu' 'aux/vga -m vesa -l 1024x768x32' \
	"bind -a '#i' /dev" "bind -a '#m' /dev" >"$CLI_DIR/glfwguest.log" 2>&1 &
guest=$!
trap 'kill -TERM -- -$guest 2>/dev/null || kill $guest 2>/dev/null; wait $guest 2>/dev/null || true' EXIT
M="python3 $ROOT/scripts/guest/mcp.py"
$M wait 600 >/dev/null
$M rc '/tests/wsys -x -i /tests/glfw >/tmp/wsys.log >[2=1] </dev/null' 180 >"$CLI_DIR/glfw.out" &
run=$!
# the keys, once glfwtest -e waits for them
for i in $(seq 300); do $M rc 'test -e /tmp/glfwe.ready' 10 >/dev/null 2>&1 && break; sleep 0.2; done
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
$M rc 'cat /tmp/wsys.log' 10 | grep -v -e '/srv/wsys' -e '^event' -e '^ok' -e '^glfw' || true
out=$(cat "$CLI_DIR/glfw.out"; printf '%s\n' 'cat /tmp/glfw.log' "grep '^event' /tmp/glfwe.log" | $M rc - 10) || true
echo "$out" | grep -E '^(ok|FAIL|glfwtest|glfw error)'
[ -n "$GLFWDEBUG" ] && echo "$out"
fail=0
check() { if echo "$out" | grep -q -- "$2"; then echo "ok $1"; else echo "FAIL $1 (no \"$2\")"; fail=1; fi; }
check "glfwtest" '^glfwtest: all passed'
check "mouse: motion in window coordinates" '^event motion 196,146'
check "mouse: button 1 (left)" '^event button 0 down at 196,146'
check "mouse: button 3 (right)" '^event button 1 down at 216,156'
check "mouse: the wheel" '^event scroll 1'
check "keyboard: a down" '^event key down 65 a'
check "keyboard: char a" '^event char a'
check "keyboard: shift" '^event key down 340 - '
check "keyboard: char B" '^event char B'
check "keyboard: B with shift" '^event key down 66 b mods 0x1'
check "keyboard: return up" '^event key up 257 '
# a GLFW window that outlives its wsys (libdraw's wsysagain): glfwagain.rc from
# the console, wsys killed twice
ag=$($M rc '/tests/glfwagain.rc' 200 2>&1) || true
echo "$ag" | grep -E '^(ok|FAIL)' | sed 's/^/  /'
if echo "$ag" | grep -q '^glfwagain.rc: all passed'; then echo "ok a GLFW window outlives its wsys (twice)"; else echo "FAIL a GLFW window outlives its wsys"; fail=1; fi
kill -TERM -- -$guest 2>/dev/null || kill $guest 2>/dev/null
wait $guest 2>/dev/null || true
trap - EXIT
[ $fail = 0 ] && echo "glfw: all passed" || echo "glfw: failed"
exit $fail
