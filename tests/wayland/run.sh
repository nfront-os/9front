#!/bin/bash
# libwayland on 9front (../9front-ui PLAN §12, stages 6.4 and 6.5): builds libffi
# (sys/src/libffi), libwayland (sys/src/wayland) and wlsys (sys/src/cmd/wlsys)
# with scripts/wayland/build.sh, then wltest (a server and a client in two
# processes: a wl_shm buffer in shared memory over the socket, the event loop's
# timer) and wlclient, and runs them in a guest of scripts/guest/mcp-guest.sh on a
# VESA screen: wltest alone, wlclient in wsys -x -i under wlsys (a toplevel's
# buffers on wsys' screen, a click, keys, a resize, close). The keymap wlsys made
# of kbdfs' tables is compiled with the host's xkbcommon (xkbcli), if it has one.
# usage: tests/wayland/run.sh   (needs ../9front-gcc's cross gcc; a build with wsys)
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
. "$ROOT/scripts/mesa/common.sh"
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
[ -x "$ROOT/amd64/bin/wsys" ] || { echo "wayland: no amd64/bin/wsys (scripts/build/cargo.sh)"; exit 1; }
mkdir -p "$HERE/root/tests"
bash "$ROOT/scripts/wayland/build.sh" "$HERE/root/tests"
"$CROSS/$TRIPLE-gcc" -std=gnu11 -O2 -Wall -o "$HERE/root/tests/wltest" "$HERE/wltest.c" \
	-lwayland-server -lwayland-client -lffi
"$CROSS/$TRIPLE-gcc" -std=gnu11 -O2 -Wall -Wno-unused-parameter -o "$HERE/root/tests/wlclient" "$HERE/wlclient.c" \
	-lwayland-client -lffi
cp "$ROOT/amd64/bin/wsys" "$HERE/root/tests/wsys"
cp "$HERE/wlsys.rc" "$HERE/root/tests/wlsys.rc"
chmod +x "$HERE/root/tests/wlsys.rc"

UFS_EXTRA=$HERE/root MEM=1024 KARGS="vgasize=1024x768x32 monitor=vesa mouseport=ps2" \
	setsid bash "$ROOT/scripts/guest/mcp-guest.sh" 'aux/realemu' 'aux/vga -m vesa -l 1024x768x32' \
	>"$CLI_DIR/waylandguest.log" 2>&1 &
guest=$!
trap 'kill -TERM -- -$guest 2>/dev/null || kill $guest 2>/dev/null; wait $guest 2>/dev/null || true' EXIT
M="timeout 300 python3 $ROOT/scripts/guest/mcp.py"
$M wait 600 >/dev/null
out=$($M rc '/tests/wltest >[2=1]' 120) || true
echo "$out"
wl=$($M rc "bind -a '#i' /dev; bind -a '#m' /dev; /tests/wsys -x -i /tests/wlsys.rc >/tmp/wsys.log >[2=1]; cat /tmp/wsys.log /tmp/wlclient.log" 240) || true
echo "$wl"
$M rc "cat /tmp/keymap.xkb" > "$CLI_DIR/wlsys-keymap.xkb" 2>/dev/null || true
kill -TERM -- -$guest 2>/dev/null || kill $guest 2>/dev/null
wait $guest 2>/dev/null || true
trap - EXIT
fail=0
echo "$out" | grep -q '^wltest: all passed' || { echo "FAIL wltest"; fail=1; }
echo "$wl" | grep -q '^wlclient: all passed' || { echo "FAIL wlclient"; fail=1; }
if command -v xkbcli >/dev/null; then
	if [ -s "$CLI_DIR/wlsys-keymap.xkb" ] &&
	   xkbcli compile-keymap --keymap "$CLI_DIR/wlsys-keymap.xkb" >/dev/null 2>"$CLI_DIR/wlsys-keymap.err"; then
		echo "ok the keymap of wlsys: xkbcommon compiles it"
	else
		echo "FAIL the keymap of wlsys: xkbcommon: $(head -3 "$CLI_DIR/wlsys-keymap.err" 2>/dev/null)"; fail=1
	fi
fi
[ $fail = 0 ] && { echo "wayland: all passed"; exit 0; }
echo "wayland: failed"
exit 1
