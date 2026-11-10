#!/bin/bash
# drawfs (sys/src/cmd/drawfs, ../9front-ui PLAN stage 4c) against the kernel's
# devdraw: drawcmp's operations on images off the screen in a VESA guest of
# scripts/guest/mcp-guest.sh, once on #i, once on a drawfs mounted on /dev in
# a namespace of its own; every result's SHA-1 must be the same.
# usage: tests/draw/run.sh   (needs a build: amd64/bin/drawfs)
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
[ -x "$ROOT/amd64/bin/drawfs" ] || { echo "draw: no amd64/bin/drawfs"; exit 1; }
(cd "$HERE" && CC=gcc ARCH=amd64 "$ROOT/sys/src/cmd/build/build.exe" build.json >/dev/null 2>&1) || { echo "FAIL draw: building drawcmp"; exit 1; }
M="timeout 300 python3 $ROOT/scripts/guest/mcp.py"
UFS_EXTRA=$HERE/root MEM=1024 KARGS="vgasize=1024x768x32 monitor=vesa mouseport=ps2" \
	setsid bash "$ROOT/scripts/guest/mcp-guest.sh" 'aux/realemu' 'aux/vga -m vesa -l 1024x768x32' \
	>"$CLI_DIR/drawguest.log" 2>&1 &
guest=$!
stop(){ kill -TERM -- -$guest 2>/dev/null || kill $guest 2>/dev/null; wait $guest 2>/dev/null || true; }
trap stop EXIT
timeout 660 python3 "$ROOT/scripts/guest/mcp.py" wait 600 >/dev/null
k=$($M rc "bind -a '#i' /dev; /tests/drawcmp >[2=1]" 120) || true
d=$($M rc "rfork n; drawfs -r 640 480; cat /dev/drawfs/screen; /tests/drawcmp >[2=1]" 120) || true
stop
trap - EXIT
fail=0
echo "$d" | grep -q '^drawfs\.[0-9]* x8r8g8b8 0 0 640 480 640' && echo "ok drawcmp's second run on drawfs (its screen)" ||
	{ echo "FAIL drawfs not mounted: $(echo "$d" | head -2 | tr '\n' ' ')"; fail=1; }
echo "$k" | grep -q '^drawcmp: done' || { echo "FAIL drawcmp on #i: $(echo "$k" | tail -3 | tr '\n' ' ')"; fail=1; }
echo "$d" | grep -q '^drawcmp: done' || { echo "FAIL drawcmp on drawfs: $(echo "$d" | tail -3 | tr '\n' ' ')"; fail=1; }
while read -r name sum; do
	[ "$name" = drawcmp: ] && continue
	case $name in drawfs.*) continue;; esac
	other=$(echo "$d" | awk -v n="$name" '$1 == n {print $2}')
	if [ "$other" = "$sum" ]; then echo "ok $name"; else echo "FAIL $name: #i $sum, drawfs ${other:-none}"; fail=1; fi
done <<< "$k"
[ $fail = 0 ] && echo "draw: all passed" || echo "draw: failed"
exit $fail
