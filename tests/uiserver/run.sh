#!/bin/bash
# uiserver: ui (sys/src/cmd/ui, ../9front-ui PLAN stage 5: a window of widgets
# a client describes through files) in a window of wsys on a VESA screen (and
# uirestart.rc: a ui program outlives its wsys), a
# client in rc (tests/uiserver/uiserver.rc) and one in C through libui
# (uiclient.c), uitodo (sys/src/cmd/uidemo: Rust, uiview) and gocount (Go,
# sys/src/libui/go), in a guest of
# scripts/guest/mcp-guest.sh.
# usage: tests/uiserver/run.sh   (needs ../9front-rust's stage-1 rustc, a build: wsys)
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
. "$ROOT/scripts/lib/paths.sh"	# GCC_DIR, RUST_DIR, GO_DIR, MCP_DIR, DISTFILES (the distribution's paths.env)
. "$ROOT/scripts/mesa/common.sh"
RUSTC=${RUSTC:-$RUST_DIR/rust/build/host/stage1/bin/rustc}
T=x86_64-unknown-plan9
. "$ROOT/scripts/build/cargostd.sh"
cargostd "$BUILD/cargo/rust" $T
(cd "$ROOT/sys/src/rust" && env RUSTC="$RUSTC" CARGO_TARGET_DIR="$BUILD/cargo/rust" \
	CARGO_TARGET_X86_64_UNKNOWN_PLAN9_LINKER="$CROSS/$TRIPLE-gcc" \
	cargo build --offline --release --target $T --quiet -p ui -p uidemo)
mkdir -p "$HERE/root/tests" "$HERE/root/lib/font/ttf"
(cd "$ROOT/tests/ui" && env PLAN9="$ROOT" ARCH=amd64 CC=gcc "$ROOT/sys/src/cmd/build/build.exe" build.json >/dev/null)
(cd "$HERE" && env PLAN9="$ROOT" ARCH=amd64 CC=gcc "$ROOT/sys/src/cmd/build/build.exe" build.json >/dev/null)
for p in ui uitodo uitick uicount; do
	"$CROSS/$TRIPLE-strip" -o "$HERE/root/tests/$p" "$BUILD/cargo/rust/$T/release/$p"
done
# the client in Go, with ../9front-go when it is there (a build cache of its
# own: the toolchain is rebuilt under the same version)
GO=${GO:-$GO_DIR/go/bin/go}
rm -f "$HERE/root/tests/gocount"
if [ -x "$GO" ]; then
	(cd "$ROOT/sys/src/libui/go" && GOOS=plan9 GOARCH=amd64 CGO_ENABLED=0 GOCACHE="$BUILD/gocache" \
		"$GO" build -o "$HERE/root/tests/gocount" ./cmd/gocount)
fi
cp "$ROOT/amd64/bin/wsys" "$ROOT/tests/ui/root/tests/uipeek" "$HERE/root/tests/"
cp "$ROOT/lib/font/ttf/"*.ttf "$HERE/root/lib/font/ttf/"
cp "$HERE/uiserver.rc" "$HERE/root/tests/uiserver"
cp "$HERE/uirestart.rc" "$HERE/root/tests/uirestart"
# a plumber before wsys (its windows, ui among them, find it): plumbtest... to testport
cp "$ROOT/tests/ui/plumbrules" "$HERE/root/tests/plumbrules"
chmod +x "$HERE/root/tests/uiserver" "$HERE/root/tests/uirestart"

UFS_EXTRA=$HERE/root MEM=1024 KARGS="vgasize=1024x768x32 monitor=vesa mouseport=ps2" \
	setsid bash "$ROOT/scripts/guest/mcp-guest.sh" 'aux/realemu' 'aux/vga -m vesa -l 1024x768x32' \
	"bind -a '#i' /dev" "bind -a '#m' /dev" >"$CLI_DIR/uiserverguest.log" 2>&1 &
guest=$!
trap 'kill -TERM -- -$guest 2>/dev/null || kill $guest 2>/dev/null; wait $guest 2>/dev/null || true' EXIT
M="timeout 300 python3 $ROOT/scripts/guest/mcp.py"
$M wait 600 >/dev/null
$M rc 'bind /tests/ui /bin/ui; plumber -p /tests/plumbrules >/dev/null >[2=1] </dev/null; /tests/wsys -x -i /tests/uiserver >/tmp/wsys.log >[2=1] </dev/null' 240 >"$CLI_DIR/uiserver.out" || true
out=$($M rc 'cat /tmp/wsys.log' 20) || true
echo "$out" | grep -v -e '/srv/wsys'
fail=0
echo "$out" | grep -q '^uiserver: all passed' || fail=1
# a ui program outlives its wsys, its state too (from the console: not in a wsys)
ur=$($M rc '/tests/uirestart' 200 2>&1) || true
echo "$ur" | grep -E '^(ok|FAIL)'
echo "$ur" | grep -q '^uirestart: all passed' && echo "ok a ui program outlives its wsys, its count kept" || { echo "FAIL a ui program outlives its wsys"; fail=1; }
kill -TERM -- -$guest 2>/dev/null || kill $guest 2>/dev/null
wait $guest 2>/dev/null || true
trap - EXIT
exit $fail
