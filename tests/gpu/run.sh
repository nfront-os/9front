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
# hostmem bar (gpu(4)); with Mesa built, vktest (Vulkan compute on the
# host's GPU), vkdraw (a swapchain on the screen, VK_PLAN9_surface,
# checked pixel by pixel in the guest) and vktri (a triangle drawn with
# shaders by an ordinary Vulkan program, through the static loader);
# Khronos' vulkaninfo and vkcube (60 frames, then the screen is read);
# vkspeed: the processor reads cached host memory fast (gpu(3)'s
# write-back segment), else every frame would crawl. The checks
# in the guest read the draw(3) screen, so those programs copy
# (WSI_PLAN9_COPY=1). Last, vkdraw without it: its frames go to the
# display directly (gpu(4) scanout); the host's display is read
# through VNC. A third boot: vkcube in a rio window, resized, quit with
# q (riovk.rc).
# A fourth: vktri in a window of wsys, its frames as GPU buffers (wsysvk.rc).
# usage: bash tests/gpu/run.sh   (amd64 only; needs a build)
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
[ "$objtype" = amd64 ] || { skip "gpu tests" "objtype=$objtype: #G is in pc64 only"; done_tests; exit; }
[ -x "$BUILDEXE" ] || { skip "gpu tests" "no build tool $BUILDEXE"; done_tests; exit; }

buildtests(){ (cd "$HERE" && env PLAN9="$ROOT" ARCH=$objtype CC=gcc "$BUILDEXE" build.json); }
check "build tests/gpu" buildtests || { done_tests; exit; }
# the Vulkan tests need ../9front-gcc's cross compiler: build.exe builds
# libvulkan.a from the tree's Mesa (sys/src/mesa/build.json, a minute once)
rm -f "$HERE/root/tests/vktest" "$HERE/root/tests/vkdraw" "$HERE/root/tests/vktri" \
	"$HERE/root/tests/vkcube" "$HERE/root/tests/vulkaninfo" "$HERE/root/tests/vkspeed" \
	"$HERE/root/tests/rsvk"
cross=$(. "$ROOT/scripts/mesa/common.sh"; echo "$CROSS/$TRIPLE-gcc")
if [ -x "$cross" ]; then
	check "build libvulkan.a (build.exe, sys/src/mesa) and the Vulkan tests" bash "$HERE/build-vktest.sh"
else
	skip "Vulkan tests" "no cross compiler $cross (../9front-gcc)"
fi

# segment(3)'s alias and phys segments: removed or revoked, nobody keeps their
# pages (a freed GPU object stays visible to no one); a missing page faults
segrevoke(){
	MEM=1024 UFS_EXTRA="$HERE/root" "$ROOT/scripts/guest/boot-cli.sh" \
		'/tests/segrevoke' > "$TMPD/segrevoke" 2>&1
	grep -q '^segrevoke: ok' "$CLI_DIR/console.log" || { grep -E 'FAIL|ok ' "$CLI_DIR/console.log"; return 1; }
}
check "alias and phys segments revoked (segment(3)): freed objects stay visible to no one" segrevoke

# the LinuxKPI on 9front: aux/i915 -T (segments in procs forked before,
# vmap through alias segments, Linux's dma-buf selftests on our fences)
# with 4 processors: races show (dma_fence_get_rcu revived a freed fence:
# 2 of 10 runs failed so, 1 in many with one)
# with SMBIOS strings of our own (QEMU -smbios): DMI, and the GPU's ACPI
# node by _ADR through aux/acpi (the one interpreter: the driver starts
# it, a second aux/acpi mounts it instead of starting another)
kpitest(){
	SMP=4 MEM=2048 WAITMAX=500 QEMU_EXTRA="-vga none -device virtio-gpu-pci -smbios type=1,manufacturer=9front,product=gputest -smbios type=0,vendor=testbios,version=1.2" \
		"$ROOT/scripts/guest/boot-cli.sh" 'aux/i915 -T' 'aux/acpi' 'ls /dev/aml /dev/pmctl' > "$TMPD/kpitest" 2>&1
	tr -d '\r' < "$CLI_DIR/console.log" > "$TMPD/kpitest.log"
	grep -q "dma-buf selftests: ok" "$CLI_DIR/console.log" && return
	# keep the console of a failure
	cp "$CLI_DIR/console.log" "$CLI_DIR/kpitest-fail.$(date +%s).log"
	grep -E 'selftest|failed|not seen' "$CLI_DIR/console.log"
	return 1
}
check "LinuxKPI selftests (aux/i915 -T: segments, vmap, Linux's dma-buf selftests)" kpitest
check "DMI from SMBIOS (#P/smbios): QEMU's strings" grep -q 'selftest: dmi: sys vendor "9front", product "gputest", bios "testbios" "1.2"' "$TMPD/kpitest.log"
check "ACPI: the GPU's node by _ADR (aux/acpi's aml)" grep -qE 'selftest: acpi: GPU 0000:00:0[0-9a-f]\.0 is .*, _ADR 0x[0-9a-f]+: ok' "$TMPD/kpitest.log"
check "one AML interpreter: a second aux/acpi mounts the first" bash -c "grep -q 'aux/acpi: runs already: /srv/acpi mounted on /dev' '$TMPD/kpitest.log' && grep -q '^/dev/pmctl' '$TMPD/kpitest.log'"

# i915's mock selftests (no GPU: a mock device): GEM, GTT, vma, requests, ...
mocktest(){
	MEM=4096 CMDWAIT=3300 QEMU_EXTRA="-vga none -device virtio-gpu-pci" \
		"$ROOT/scripts/guest/boot-cli.sh" 'aux/i915 -M' > "$TMPD/mocktest" 2>&1
	grep -q "mock selftests: ok" "$CLI_DIR/console.log" || { grep -E 'failed|<3>|mock selftests' "$CLI_DIR/console.log"; return 1; }
}
check "i915's mock selftests (aux/i915 -M)" mocktest

# Linux's module parameters (aux/i915 -p name=value, i915param.rc):
# refusals, and set ones take effect in the mock selftests
i915param(){
	cp "$HERE/i915param.rc" "$HERE/root/tests/i915param"
	MEM=4096 CMDWAIT=900 UFS_EXTRA="$HERE/root" QEMU_EXTRA="-vga none -device virtio-gpu-pci" \
		"$ROOT/scripts/guest/boot-cli.sh" 'rc /tests/i915param' > "$TMPD/i915param" 2>&1
	tr -d '\r' < "$CLI_DIR/console.log" | awk '
		/^>>/ { cmd = $0; n++ }
		/^<</ { st[n] = $0 }
		/st_timeout=2000/ && /Performing/ { timeout = 1 }
		/Running/ { run[n] = run[n] " " $NF }
		END {
			bad = 0
			for (i = 1; i <= 4; i++)
				if (st[i] !~ /error (2|22)$/) { print "case " i " not refused: " st[i]; bad = 1 }
			if (st[5] !~ /^<< status *$/ || !timeout || run[5] != " sanitycheck") { print "case 5: " st[5] " ran:" run[5]; bad = 1 }
			if (st[6] !~ /^<< status *$/ || run[6] != " vma i915_vma_mock_selftests/igt_vma_pin1") { print "case 6: " st[6] " ran:" run[6]; bad = 1 }
			if (n != 6) { print n " cases ran"; bad = 1 }
			exit bad
		}'
}
check "Linux's module parameters (aux/i915 -p)" i915param

# rc/bin/i915live (one run of aux/i915 -L for all groups, a new run only
# after a failure or a hang) against a stand-in driver (i915live.rc)
i915live(){
	cp "$HERE/i915live.rc" "$HERE/root/tests/i915live"
	MEM=1024 CMDWAIT=300 UFS_EXTRA="$HERE/root" \
		"$ROOT/scripts/guest/boot-cli.sh" 'rc /tests/i915live' > "$TMPD/i915live" 2>&1
	tr -d '\r' < "$CLI_DIR/console.log" | grep -q '^i915live: 33 ok, 4 failed: gt_engines gt_contexts gt_lrc gem (3 runs)$' ||
		{ tr -d '\r' < "$CLI_DIR/console.log" | grep '^i915live:'; return 1; }
}
check "i915live: the live selftests in as few runs as failures allow" i915live

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
		# CPU=host: SSE4.1's streaming loads read the host visible memory
		# fast (venus: write-combined on the host); venus needs KVM anyway
		# VNC: at the end, what the host shows while vkdraw runs
		# (QMP's screendump has no surface with a GL display)
		CPU=host MEM=1024 UFS_EXTRA="$HERE/root" VNC=1 SCREENSHOT="$TMPD/shot3d.png" \
		SHOTPOINTS="200,450 500,450 900,100 50,700" \
		QEMU_EXTRA="-vga none -display egl-headless -device virtio-gpu-gl-pci,blob=true,venus=true,hostmem=256M -object memory-backend-memfd,id=mem1,size=1024M,share=on -machine memory-backend=mem1" \
		"$ROOT/scripts/guest/boot-cli.sh" \
			'aux/virtiogpu' \
			'cat /dev/gpu/0/status' \
			'/tests/gpuctxtest' \
			'cat /dev/gpu/0/status' \
			"test ! -x /tests/vktest || /tests/vktest" \
			"bind -a '#i' /dev" \
			"test ! -x /tests/vkdraw || WSI_PLAN9_COPY=1 /tests/vkdraw 3" \
			"test ! -x /tests/vktri || WSI_PLAN9_COPY=1 /tests/vktri 2" \
			"test ! -x /tests/rsvk || WSI_PLAN9_COPY=1 /tests/rsvk 2" \
			"test ! -x /tests/vkshare || {rm -f /tmp/vks /tmp/vks.done; /tests/vkshare export /tmp/vks & /tests/vkshare import /tmp/vks; wait}" \
			"test ! -x /tests/vkspeed || /tests/vkspeed" \
			"test ! -x /tests/vulkaninfo || /tests/vulkaninfo --summary" \
			"test ! -x /tests/vulkaninfo || /tests/vulkaninfo | grep -c 'Surface type *= VK_PLAN9_surface'" \
			"test ! -x /tests/vkcube || {WSI_PLAN9_COPY=1 /tests/vkcube --c 60 >/dev/null >[2=1]; /tests/screenpx 1270,790 640,400}" \
			"test ! -x /tests/vkdraw || {/tests/vkdraw 3 >/tmp/zc >[2=1]; grep frames /tmp/zc | sed 's/^ok/ok zero-copy:/'}" \
			"test ! -x /tests/gltest || /tests/gltest >[2=1]" \
			"test ! -x /tests/gldesk || /tests/gldesk >[2=1]" \
			"test ! -x /tests/glwin || /tests/glwin 60 >[2=1]" \
			"test ! -x /tests/vkdraw || /tests/vkdraw 30 >/dev/null >[2=1] &" \
			"sleep 8" \
			"grep '^scanout' /dev/gpu/0/status" > "$TMPD/console3d" 2>&1
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
	if [ -x "$HERE/root/tests/vktest" ]; then
		check "vktest (Vulkan through venus): all passed" grep -q '^vktest: all passed' "$TMPD/log3d"
		check "vkdraw (VK_PLAN9_surface, swapchain on the screen): all passed" grep -q '^vkdraw: all passed' "$TMPD/log3d"
		check "vktri (shaders, libvulkan's loader): all passed" grep -q '^vktri: all passed' "$TMPD/log3d"
		if [ -x "$HERE/root/tests/rsvk" ]; then
			check "rsvk (vktri in Rust: ash, libvulkan.a): all passed" grep -q '^rsvk: all passed' "$TMPD/log3d"
		else
			skip "rsvk" "not built (no stage-1 rustc of ../9front-rust)"
		fi
		check "vkshare (a GPU buffer exported by one process, imported by another): all passed" \
			bash -c "grep -q '^vkshare export: all passed' '$TMPD/log3d' && grep -q '^vkshare import: all passed' '$TMPD/log3d'"
		check "vkspeed: cached host memory read at 1 GB/s or more" \
			bash -c "grep '^vkspeed: cached streaming read' '$TMPD/log3d' | awk '{ exit !(\$5 >= 1000) }'"
		check "vulkaninfo --summary: the venus device" grep -q 'deviceName *= Virtio-GPU Venus' "$TMPD/log3d"
		check "vulkaninfo: a VK_PLAN9_surface section" bash -c "grep -A1 \"vulkaninfo | grep -c\" '$TMPD/log3d' | sed -n 2p | grep -q '^[1-9]'"
		check "vkcube: clear colour at the edge" grep -q '^1270,790 333333' "$TMPD/log3d"
		check "vkcube: the cube in the middle" bash -c "grep '^640,400 ' '$TMPD/log3d' | grep -vq '333333\$'"
		cp "$TMPD/shot3d.png" "$HERE/screen-vk.png" 2>/dev/null
		check "vkdraw's frames on the display without a copy (ctx scanout)" grep -q '^scanout ctx [0-9]* res [0-9]' "$TMPD/log3d"
		check "display (host side, VNC): vkdraw's red square" grep -q '^200,450 ff0000' "$TMPD/shot3d.txt"
		check "display (host side, VNC): vkdraw's green square" grep -q '^500,450 00ff00' "$TMPD/shot3d.txt"
		check "display (host side, VNC): blue background" bash -c "grep -q '^900,100 000066' '$TMPD/shot3d.txt' && grep -q '^50,700 000066' '$TMPD/shot3d.txt'"
	fi
	if [ -x "$HERE/root/tests/gltest" ]; then
		check "gltest (GLES through zink on venus, a pbuffer): all passed" grep -q '^gltest: all passed' "$TMPD/log3d"
		check "gldesk (desktop GL 4.6: compatibility and core profile): all passed" grep -q '^gldesk: all passed' "$TMPD/log3d"
		check "glwin (EGL's plan9 platform, the frames on the display): all passed" grep -q '^glwin: all passed' "$TMPD/log3d"
	fi
	check "contexts gone after the test" bash -c "! sed -n '/gpuctxtest: /,\$p' '$TMPD/log3d' | grep -q '^ctx '"

	# wsys (../9front-ui stage 2): vktri in a window of wsys hands its
	# frames over as GPU buffers (gpu(4) export, wsys' gpubuffer; Mesa's
	# plan9 WSI), then again by copy for comparison (wsysvk.rc)
	if [ -x "$HERE/root/tests/vktri" ] && [ -x "$ROOT/$objtype/bin/wsys" ]; then
		cp "$ROOT/$objtype/bin/wsys" "$HERE/root/tests/wsys"
		cp "$HERE/wsysvk.rc" "$HERE/root/tests/wsysvk"
		cp "$HERE/wsysvktri.rc" "$HERE/root/tests/wsysvktri"
		cp "$HERE/wsysovl.rc" "$HERE/root/tests/wsysovl"
		chmod +x "$HERE/root/tests/wsysvk" "$HERE/root/tests/wsysvktri" "$HERE/root/tests/wsysovl"
		bootwsys(){
			CMDWAIT=120 CPU=host MEM=1024 UFS_EXTRA="$HERE/root" \
			QEMU_EXTRA="-vga none -display egl-headless -device virtio-gpu-gl-pci,blob=true,venus=true,hostmem=256M -object memory-backend-memfd,id=mem1,size=1024M,share=on -machine memory-backend=mem1" \
			"$ROOT/scripts/guest/boot-cli.sh" 'aux/virtiogpu' "bind -a '#i' /dev" \
				'/tests/wsys -x -i /tests/wsysvk >/tmp/wsys.log >[2=1]; cat /tmp/wsys.log' > "$TMPD/consolewsys" 2>&1
		}
		check "boot with virtio-gpu-gl, vktri in a window of wsys" bootwsys
		tr -d '\r' < "$TMPD/consolewsys" > "$TMPD/logwsys"
		cp "$TMPD/logwsys" "$HERE/console-wsys.log"
		check "wsys: the WSI hands vktri's frames to wsys as GPU buffers" grep -q '^gpu: wsi plan9: wsys has the buffers' "$TMPD/logwsys"
		check "wsys: vktri through GPU buffers, its pixels in the window: all passed" grep -q '^gpu: vktri: all passed' "$TMPD/logwsys"
		check "wsys: vktri by copy (WSI_PLAN9_COPY=1): all passed" grep -q '^copy: vktri: all passed' "$TMPD/logwsys"
		grep -E '^(gpu|copy): ok [0-9]+ frames' "$TMPD/logwsys" | sed 's/^/  /'
	else
		skip "wsys with GPU buffers" "no vktri or no $objtype/bin/wsys"
	fi

	# third boot: vkcube in a rio window (riovk.rc, riocube.rc), the window
	# resized after 5 s; then the host's display through VNC, and q typed
	# on the serial line, which kbdfs gives rio, rio the window.
	# After rio, rc gets nothing from the serial line: the guest's
	# scripts log to $home/riovk.log, which is on the host (ufsroot).
	if [ -x "$HERE/root/tests/vkcube" ]; then
		cp "$HERE/riovk.rc" "$HERE/root/tests/riovk"
		cp "$HERE/riocube.rc" "$HERE/root/tests/riocube"
		cp "$HERE/riorsvk.rc" "$HERE/root/tests/riorsvk"
		chmod +x "$HERE/root/tests/riovk" "$HERE/root/tests/riocube" "$HERE/root/tests/riorsvk"
		riolog=$CLI_DIR/ufsroot/usr/glenda/riovk.log
		rm -f "$riolog"
		CMDWAIT=5 WAITFOR='never shown' WAITMAX=300 VNC=1 CPU=host MEM=1024 UFS_EXTRA="$HERE/root" \
		QEMU_EXTRA="-vga none -display egl-headless -device virtio-gpu-gl-pci,blob=true,venus=true,hostmem=256M -object memory-backend-memfd,id=mem1,size=1024M,share=on -machine memory-backend=mem1" \
		"$ROOT/scripts/guest/boot-cli.sh" 'aux/virtiogpu' "bind -a '#i' /dev" "bind -a '#m' /dev" \
			'aux/mouse ps2' 'rio -i /tests/riovk' > "$TMPD/consolerio" 2>&1 &
		riopid=$!
		for _ in $(seq 150); do
			grep -q '^riovk: resized' "$riolog" 2>/dev/null && break
			sleep 2
		done
		python3 "$ROOT/scripts/debug/vncshot.py" "$CLI_DIR/vnc.sock" "$TMPD/shotrio.png" \
			150,400 210,400 500,400 850,400 > "$TMPD/shotrio.txt" 2>&1
		cp "$TMPD/shotrio.png" "$HERE/screen-rio.png" 2>/dev/null
		pts=$(grep -o '/dev/pts/[0-9]*' "$CLI_DIR/qemu.log" | head -1)
		[ -n "$pts" ] && printf q > "$pts"
		for _ in $(seq 30); do
			grep -q '^riovk: vkcube exit' "$riolog" 2>/dev/null && break
			sleep 1
		done
		kill $riopid 2>/dev/null
		wait $riopid 2>/dev/null
		cp "$riolog" "$HERE/riovk.log" 2>/dev/null
		if [ -x "$HERE/root/tests/rsvk" ]; then
			check "rio: rsvk (Rust, ash) in a window: all passed" grep -q '^rsvk: all passed' "$riolog"
		fi
		check "rio: vkcube in a window" grep -q '^Selected WSI platform: plan9' "$riolog"
		check "rio: window resized" grep -q '^riovk: resized' "$riolog"
		check "rio (VNC): the cube in the resized window" bash -c "grep -q '^210,400 333333' '$TMPD/shotrio.txt' && grep '^500,400 ' '$TMPD/shotrio.txt' | grep -vq '333333\$'"
		check "rio (VNC): rio's background where the window was" bash -c "grep -q '^150,400 777777' '$TMPD/shotrio.txt' && grep -q '^850,400 777777' '$TMPD/shotrio.txt'"
		check "rio: q quits vkcube (status empty)" grep -qx 'riovk: vkcube exit *' "$riolog"
	fi
else
	skip "3d tests" "no render node or no virtio-gpu-gl-pci in $QEMU"
fi
done_tests
