#!/bin/bash
# winit on 9front (sys/src/rust/crates: winit, softbuffer, raw-window-handle with
# plan9 backends; p9win): winittest, eguitest, icedtest and eframetest (eframe
# falls back to the processor: no GPU) in windows of wsys on a VESA
# screen without a GPU driver, in a guest of scripts/guest/mcp-guest.sh; the
# host types into them through QEMU (QMP) while their -e runs log the events.
# usage: tests/winit/run.sh   (needs ../9front-rust's stage-1 rustc, cargo; a build: wsys)
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
. "$ROOT/scripts/mesa/common.sh"
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
RUST_DIR=${RUST_DIR:-$ROOT/../9front-rust}
RUSTC=${RUSTC:-$RUST_DIR/rust/build/host/stage1/bin/rustc}
T=x86_64-unknown-plan9
# the workspace in sys/src/rust (its vendor/): winittest's package
(cd "$ROOT/sys/src/rust" && env RUSTC="$RUSTC" CARGO_TARGET_DIR="$BUILD/cargo/rust" \
	CARGO_TARGET_X86_64_UNKNOWN_PLAN9_LINKER="$CROSS/$TRIPLE-gcc" \
	cargo build --offline --release --target $T --quiet -p winittest)
mkdir -p "$HERE/root/tests"
for p in winittest eguitest; do cp "$BUILD/cargo/rust/$T/release/$p" "$HERE/root/tests/"; done
# icedtest (iced: tiny-skia, window_clipboard, iced_graphics with plan9) is
# ../9front-rust's (tests/crates/icedtest: crates.io's crates, not vendored here)
I=$RUST_DIR/tests/crates/icedtest
rm -f "$HERE/root/tests/icedtest"
if [ -d "$I" ]; then
	(cd "$I" && env RUSTC="$RUSTC" CARGO_TARGET_DIR="$BUILD/cargo/icedtest" \
		CARGO_TARGET_X86_64_UNKNOWN_PLAN9_LINKER="$CROSS/$TRIPLE-gcc" \
		cargo --config "$RUST_DIR/cargo-plan9.toml" build --release --target $T --quiet 2>/dev/null) &&
	cp "$BUILD/cargo/icedtest/$T/release/icedtest" "$HERE/root/tests/"
fi
# eframetest (eframe: wgpu, the processor without an adapter) and
# eframetestsw (eframe without wgpu: no Vulkan linked), ../9front-rust's too
E=$RUST_DIR/tests/crates/eframetest
rm -f "$HERE/root/tests/eframetest" "$HERE/root/tests/eframetestsw"
if [ -d "$E" ]; then
	for v in eframetest:: eframetestsw:--no-default-features:; do
		n=${v%%:*} f=${v#*:}; f=${f%:}
		l="-l draw -l 9 -l fmt -l utf"; [ -z "$f" ] && l="$l -l stdc++"
		(cd "$E" && env RUSTC="$RUSTC" CARGO_TARGET_DIR="$BUILD/cargo/$n" \
			CARGO_TARGET_X86_64_UNKNOWN_PLAN9_LINKER="$CROSS/$TRIPLE-gcc" \
			RUSTFLAGS="$l -C link-arg=-Wl,--gc-sections" \
			cargo --config "$RUST_DIR/cargo-plan9.toml" build --release --target $T --quiet $f 2>/dev/null) &&
		"$CROSS/$TRIPLE-strip" -o "$HERE/root/tests/$n" "$BUILD/cargo/$n/$T/release/eframetest"
	done
fi
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
keys /tmp/icede.ready x shift+y
wait $run || true
$M rc 'cat /tmp/wsys.log' 10 | grep -v -e '/srv/wsys' -e '^event' -e '^ok' -e '^winit' -e '^egui' || true
out=$(cat "$CLI_DIR/winit.out"; printf '%s\n' 'cat /tmp/winit.log /tmp/egui.log /tmp/iced.log /tmp/eframetest*.log' "grep '^event' /tmp/winite.log" \
	"grep '^(event|clicks|button)' /tmp/eguie.log" "grep '^(event|clicks)' /tmp/icede.log | sed 's/^/iced /'" | $M rc - 10) || true
echo "$out" | grep -E '^(ok|FAIL|winittest|eguitest|eframetest)' || true
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
for n in eframetest eframetestsw; do
	[ -f "$HERE/root/tests/$n" ] || continue
	check "$n (no GPU: the processor)" "^$n: all passed"
done
if [ -f "$HERE/root/tests/icedtest" ]; then
	check "icedtest" '^icedtest: all passed'
	check "iced: text typed" '^iced event text xY'
	# open: iced's button takes no click (its press yes, the release finds it not pressed)
	echo "$out" | grep -q '^iced event clicked 1' && echo "ok iced: button clicked" || echo "open iced: button clicked"
fi
kill -TERM -- -$guest 2>/dev/null || kill $guest 2>/dev/null
wait $guest 2>/dev/null || true
trap - EXIT
[ $fail = 0 ] && echo "winit: all passed" || echo "winit: failed"
exit $fail
