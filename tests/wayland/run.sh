#!/bin/bash
# libwayland on 9front (../9front-ui PLAN §12, stage 6.4): builds libffi
# (sys/src/libffi) and libwayland (sys/src/wayland) with build.exe and the
# cross gcc, then wltest (a server and a client in two processes: wl_shm
# buffer in shared memory over the socket, the event loop's timer), and runs
# it in a guest of scripts/guest/mcp-guest.sh.
# usage: tests/wayland/run.sh   (needs ../9front-gcc's cross gcc; a build)
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
. "$ROOT/scripts/mesa/common.sh"
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
for m in sys/src/libffi/build.json sys/src/wayland/build.json; do
	CC=$CROSS/$TRIPLE-gcc ARCH=amd64 "$ROOT/sys/src/cmd/build/build.exe" -D "$ROOT/$m" > "$CLI_DIR/wayland-build.log" 2>&1 ||
		{ tail -20 "$CLI_DIR/wayland-build.log"; exit 1; }
done
mkdir -p "$HERE/root/tests"
"$CROSS/$TRIPLE-gcc" -std=gnu11 -O2 -Wall -o "$HERE/root/tests/wltest" "$HERE/wltest.c" \
	-lwayland-server -lwayland-client -lffi

UFS_EXTRA=$HERE/root MEM=1024 setsid bash "$ROOT/scripts/guest/mcp-guest.sh" >"$CLI_DIR/waylandguest.log" 2>&1 &
guest=$!
trap 'kill -TERM -- -$guest 2>/dev/null || kill $guest 2>/dev/null; wait $guest 2>/dev/null || true' EXIT
M="timeout 200 python3 $ROOT/scripts/guest/mcp.py"
$M wait 600 >/dev/null
out=$($M rc '/tests/wltest >[2=1]' 120) || true
echo "$out"
kill -TERM -- -$guest 2>/dev/null || kill $guest 2>/dev/null
wait $guest 2>/dev/null || true
trap - EXIT
echo "$out" | grep -q '^wltest: all passed' && { echo "wayland: all passed"; exit 0; }
echo "wayland: failed"
exit 1
