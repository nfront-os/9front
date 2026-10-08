#!/bin/bash
# gui: terminal boot with graphics (scripts/boot-cli.sh, service=terminal,
# vgasize/monitor/mouseport as plan9.ini would set them): screenrc runs
# aux/realemu and aux/vga -m vesa, the profile starts rio with riostart.
# The commands are typed into rio's window; they write their output to
# the kernel console ('#c/cons', the serial line) so it can be checked.
. "$(dirname "$0")/../lib.sh"
[ -f "$ROOT/amd64/bin/9pc64" ] || { skip "gui" "no kernel, run scripts/build-cli.sh"; done_tests; exit; }

boot(){
	SERVICE=terminal KARGS="vgasize=1024x768x16 monitor=vesa mouseport=ps2" \
	SCREENSHOT="$TMPD/screen.png" BOOTWAIT=${BOOTWAIT:-130} CMDWAIT=${CMDWAIT:-8} \
		"$ROOT/scripts/boot-cli.sh" \
		"{echo GUI; grep size /dev/vgactl; ps | awk '{print \"PS\", \$NF}'} > '#c/cons'" \
		> "$TMPD/console" 2>&1
	tr -d '\r' < "$TMPD/console" > "$TMPD/log"
}
check "terminal boot" boot || { done_tests; exit; }
L=$TMPD/log

check "vesa mode 1024x768x16" grep -q '^size 1024x768x16' "$L"
for p in rio stats plumber webfs; do
	check "$p running" grep -q "^PS $p\$" "$L"
done
check "screen dump is 1024x768" python3 -c "
import struct, sys
d = open('$TMPD/screen.png', 'rb').read(24)
sys.exit(struct.unpack('>II', d[16:24]) != (1024, 768))"
check "no kernel panic" bash -c "! grep -q '^panic:' '$L'"
check "no suicide" bash -c "! grep -q 'suicide' '$L'"
[ -z "$GUI_SCREENSHOT" ] || cp "$TMPD/screen.png" "$GUI_SCREENSHOT"
done_tests
