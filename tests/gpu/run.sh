#!/bin/bash
# gpu: PoC of the user-space GPU driver on virtio-gpu (QEMU -device
# virtio-gpu-pci, no other display). Boots the built pc64 kernel (#G,
# vga type gpu), starts aux/virtiogpu, draws with /tests/gputest (no
# rio), switches the mode through /dev/gpu/0, and checks QEMU's screen
# dump: size of the new mode, the three rectangles at their places.
# gputest -l keeps redrawing them: the kernel console also writes on
# the screen (see gputest.c).
# Second boot, if the host can render (/dev/dri/renderD128): virtio-gpu-gl
# with virgl and venus; /tests/gpuctxtest checks capsets, contexts,
# submissions with fences and a venus host blob mapped through the
# hostmem bar (gpu(4)).
# usage: bash tests/gpu/run.sh   (amd64 only; needs a build)
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
[ "$objtype" = amd64 ] || { skip "gpu tests" "objtype=$objtype: #G is in pc64 only"; done_tests; exit; }
[ -x "$BUILDEXE" ] || { skip "gpu tests" "no build tool $BUILDEXE"; done_tests; exit; }

buildtests(){ (cd "$HERE" && env PLAN9="$ROOT" ARCH=$objtype CC=gcc "$BUILDEXE" build.json); }
check "build tests/gpu" buildtests || { done_tests; exit; }

shot=$TMPD/screen.png
boot(){
	MEM=1024 UFS_EXTRA="$HERE/root" SCREENSHOT="$shot" \
	QEMU_EXTRA="-vga none -device virtio-gpu-pci" \
	"$ROOT/scripts/guest/boot-cli.sh" \
		"cat '#G/0/ctl'" \
		'aux/virtiogpu' \
		'cat /dev/gpu/0/status' \
		'cat /dev/gpu/0/outputs/Virtual-1/modes' \
		'echo 1024x768 >/dev/gpu/0/outputs/Virtual-1/mode' \
		'cat /dev/gpu/0/outputs/Virtual-1/mode' \
		"bind -a '#i' /dev" \
		'/tests/gputest -l &' \
		'sleep 2' \
		"cat '#G/0/ctl'" \
		'sleep 1' > "$TMPD/console" 2>&1
}
check "boot with virtio-gpu, start aux/virtiogpu, draw" boot
tr -d '\r' < "$TMPD/console" > "$TMPD/log"
cp "$TMPD/log" "$HERE/console.log"
[ -f "$shot" ] && cp "$shot" "$HERE/screen.png"

check "#G lists the device" grep -q '^id 1af4/1050' "$TMPD/log"
check "driver started" grep -q '^aux/virtiogpu: [0-9]*x[0-9]*, fb ' "$TMPD/log"
check "status served under /dev/gpu/0" grep -q '^device virtio-gpu' "$TMPD/log"
check "mode switched to 1024x768" grep -q '^1024x768$' "$TMPD/log"
check "gputest drew on the whole screen" grep -q 'gputest: \[0 0\] \[1024 768\] chan x8r8g8b8' "$TMPD/log"
check "interrupts arrive" grep -Eq '^irq [0-9]+ on [1-9][0-9]*' "$TMPD/log"

pixels(){
	python3 - "$shot" <<'PY'
import sys, zlib, struct
d = open(sys.argv[1], 'rb').read()
assert d[:8] == b'\x89PNG\r\n\x1a\n'
pos, idat, w = 8, b'', 0
while pos < len(d):
	n, t = struct.unpack('>I4s', d[pos:pos+8])
	c = d[pos+8:pos+8+n]
	if t == b'IHDR':
		w, h, bd, ct = struct.unpack('>IIBB', c[:10])
		assert bd == 8 and ct in (2, 6), (bd, ct)
		bpp = 3 if ct == 2 else 4
	elif t == b'IDAT':
		idat += c
	pos += 12+n
raw = zlib.decompress(idat)
stride = w*bpp
rows, prev = [], bytearray(stride)
for y in range(h):	# undo the PNG filters
	f, line = raw[y*(stride+1)], bytearray(raw[y*(stride+1)+1:(y+1)*(stride+1)])
	for i in range(stride):
		a = line[i-bpp] if i >= bpp else 0
		b, c = prev[i], prev[i-bpp] if i >= bpp else 0
		if f == 1: line[i] = (line[i]+a) & 255
		elif f == 2: line[i] = (line[i]+b) & 255
		elif f == 3: line[i] = (line[i]+(a+b)//2) & 255
		elif f == 4:
			p = a+b-c; pa, pb, pc = abs(p-a), abs(p-b), abs(p-c)
			line[i] = (line[i]+(a if pa <= pb and pa <= pc else b if pb <= pc else c)) & 255
	rows.append(line); prev = line
px = lambda x, y: tuple(rows[y][x*bpp:x*bpp+3])
want = {(200, 450): (255, 0, 0), (500, 450): (0, 255, 0), (800, 450): (0, 0, 255), (700, 200): (0, 0, 0)}
print('size', w, h)
ok = (w, h) == (1024, 768)
for p, c in want.items():
	print(p, px(*p), 'want', c)
	ok &= px(*p) == c
sys.exit(0 if ok else 1)
PY
}
if [ -f "$shot" ]; then
	check "screen dump: 1024x768, red/green/blue rectangles" pixels
else
	fail "screen dump (no $shot)"
fi

if [ -r /dev/dri/renderD128 ] && [ -w /dev/dri/renderD128 ] &&
   $QEMU -device help 2>/dev/null | grep -q '"virtio-gpu-gl-pci"'; then
	boot3d(){
		MEM=1024 UFS_EXTRA="$HERE/root" \
		QEMU_EXTRA="-vga none -display egl-headless -device virtio-gpu-gl-pci,blob=true,venus=true,hostmem=256M -object memory-backend-memfd,id=mem1,size=1024M,share=on -machine memory-backend=mem1" \
		"$ROOT/scripts/guest/boot-cli.sh" \
			'aux/virtiogpu' \
			'cat /dev/gpu/0/status' \
			'/tests/gpuctxtest' \
			'cat /dev/gpu/0/status' > "$TMPD/console3d" 2>&1
	}
	check "boot with virtio-gpu-gl (virgl, venus), run gpuctxtest" boot3d
	tr -d '\r' < "$TMPD/console3d" > "$TMPD/log3d"
	cp "$TMPD/log3d" "$HERE/console3d.log"
	check "features virgl, blob, context init" grep -q '^features virgl edid blob ctxinit' "$TMPD/log3d"
	check "host visible memory found" grep -q '^hostmem gpu0bar' "$TMPD/log3d"
	while read -r st rest; do
		case $st in
		ok)	pass "3d: $rest" ;;
		FAIL)	fail "3d: $rest" ;;
		esac
	done < <(grep -E '^(ok|FAIL) ' "$TMPD/log3d")
	check "gpuctxtest: all passed" grep -q '^gpuctxtest: all passed' "$TMPD/log3d"
	check "contexts gone after the test" bash -c "! sed -n '/gpuctxtest: /,\$p' '$TMPD/log3d' | grep -q '^ctx '"
else
	skip "3d tests" "no render node or no virtio-gpu-gl-pci in $QEMU"
fi
done_tests
