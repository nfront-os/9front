#!/bin/bash
# libwayland on 9front (../9front-ui PLAN §12, stages 6.4 and 6.5): builds libffi
# (sys/src/libffi), libwayland (sys/src/wayland) and wlsys (sys/src/cmd/wlsys)
# with scripts/wayland/build.sh, then wltest (a server and a client in two
# processes: a wl_shm buffer in shared memory over the socket, the event loop's
# timer) and wlclient, and runs them in a guest of scripts/guest/mcp-guest.sh on a
# VESA screen: wltest alone, wlclient in wsys -x -i under wlsys (a toplevel's
# buffers on wsys' screen, a click, keys, a resize, close; weston's simple-shm,
# which outlives its wsys: wsysrestart.rc). The keymap wlsys made
# of kbdfs' tables is compiled with the host's xkbcommon (xkbcli), if it has one.
# Then, with libvulkan.a (build.exe, sys/src/mesa) and a host that runs venus
# (virtio-gpu-gl), wlvk in a second guest: GPU buffers by zwp_linux_dmabuf_v1,
# then a swapchain of Mesa's Wayland WSI, vkcube --wsi wayland, and GLES on
# EGL's Wayland platform (wlgl), weston's simple-egl; the VESA guest weston's
# simple-shm (scripts/wayland/weston.sh)
# (VKONLY=1: only that guest; NOVK=1: not it).
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
"$CROSS/$TRIPLE-gcc" -std=gnu11 -O2 -Wall -o "$HERE/root/tests/wlwatch" "$HERE/wlwatch.c"
# weston's examples (scripts/wayland/weston.sh: simple-shm, simple-egl)
bash "$ROOT/scripts/wayland/weston.sh" build "$HERE/root/tests" >/dev/null
cp "$HERE/wlweston.rc" "$HERE/root/tests/wlweston.rc"
cp "$HERE/wsysrestart.rc" "$HERE/root/tests/wsysrestart.rc"
chmod +x "$HERE/root/tests/wlweston.rc" "$HERE/root/tests/wsysrestart.rc"
cp "$ROOT/amd64/bin/wsys" "$HERE/root/tests/wsys"
cp "$HERE/wlsys.rc" "$HERE/root/tests/wlsys.rc"
cp "$HERE/wlvk.rc" "$HERE/root/tests/wlvk.rc"
chmod +x "$HERE/root/tests/wlsys.rc" "$HERE/root/tests/wlvk.rc"
fail=0
M="timeout 300 python3 $ROOT/scripts/guest/mcp.py"
# weston's example: its lines (prefix p), watched by wlwatch, gone when deleted
weston(){ # output prefix title
	echo "$1" | grep "^$2: \(ok\|FAIL\|wlwatch\)" | sed "s/^$2: //"
	if echo "$1" | grep -q "^$2: wlwatch $3: all passed" && echo "$1" | grep -q "^$2: weston-simple-$2: gone after the delete"; then
		echo "ok weston-simple-$2"
	else
		echo "FAIL weston-simple-$2"; fail=1
	fi
}
stop(){ kill -TERM -- -$guest 2>/dev/null || kill $guest 2>/dev/null; wait $guest 2>/dev/null || true; }

if [ "${VKONLY:-0}" = 0 ]; then
	UFS_EXTRA=$HERE/root MEM=1024 KARGS="vgasize=1024x768x32 monitor=vesa mouseport=ps2" \
		setsid bash "$ROOT/scripts/guest/mcp-guest.sh" 'aux/realemu' 'aux/vga -m vesa -l 1024x768x32' \
		>"$CLI_DIR/waylandguest.log" 2>&1 &
	guest=$!
	trap stop EXIT
	timeout 660 python3 "$ROOT/scripts/guest/mcp.py" wait 600 >/dev/null
	out=$($M rc '/tests/wltest >[2=1]' 120) || true
	echo "$out"
	wl=$($M rc "bind -a '#i' /dev; bind -a '#m' /dev; /tests/wsys -x -i /tests/wlsys.rc >/tmp/wsys.log >[2=1]; cat /tmp/wsys.log /tmp/wlclient.log; sed 's/^/shm: /' /tmp/weston-simple-shm.log" 300) || true
	echo "$wl"
	$M rc "cat /tmp/keymap.xkb" > "$CLI_DIR/wlsys-keymap.xkb" 2>/dev/null || true
	# a Wayland client outlives its wsys: wlsys makes its window in the next one
	rs=$($M rc "/tests/wsysrestart.rc" 200) || true
	echo "$rs"
	stop
	trap - EXIT
	echo "$out" | grep -q '^wltest: all passed' || { echo "FAIL wltest"; fail=1; }
	echo "$wl" | grep -q '^wlclient: all passed' || { echo "FAIL wlclient"; fail=1; }
	weston "$wl" shm simple-shm
	for w in "wlsys outlives wsys A" "simple-shm: a window of wsys with its title" "simple-shm: it moves" "simple-shm gone after the delete in wsys B"; do
		[ "$(echo "$rs" | grep -c "^ok $w")" -ge 1 ] || { echo "FAIL wsys restart: $w"; fail=1; }
	done
	[ "$(echo "$rs" | grep -c '^ok simple-shm: a window of wsys with its title')" = 2 ] ||
		{ echo "FAIL wsys restart: the window in both wsys"; fail=1; }
	if command -v xkbcli >/dev/null; then
		if [ -s "$CLI_DIR/wlsys-keymap.xkb" ] &&
		   xkbcli compile-keymap --keymap "$CLI_DIR/wlsys-keymap.xkb" >/dev/null 2>"$CLI_DIR/wlsys-keymap.err"; then
			echo "ok the keymap of wlsys: xkbcommon compiles it"
		else
			echo "FAIL the keymap of wlsys: xkbcommon: $(head -3 "$CLI_DIR/wlsys-keymap.err" 2>/dev/null)"; fail=1
		fi
	fi
fi

# GPU buffers: wlvk on venus (Mesa's dma-buf fds, wsys' gpubuffer)
if [ "${NOVK:-0}" = 0 ]; then
	CC=$CROSS/$TRIPLE-gcc ARCH=amd64 "$ROOT/sys/src/cmd/build/build.exe" -D "$ROOT/sys/src/mesa/build.json" \
		>"$CLI_DIR/wayland-libvulkan.log" 2>&1 || { tail -20 "$CLI_DIR/wayland-libvulkan.log"; exit 1; }
	"$CROSS/$TRIPLE-gcc" -std=gnu11 -O2 -Wall -Wno-unused-parameter -c -o "$CLI_DIR/wlvk.o" "$HERE/wlvk.c"
	"$CROSS/$TRIPLE-g++" -o "$HERE/root/tests/wlvk" "$CLI_DIR/wlvk.o" -Wl,--gc-sections \
		-lvulkan -ldraw -l9 -lfmt -lutf -lwayland-client -lffi
	# wlgl: GLES on EGL's Wayland platform (zink, kopper)
	"$CROSS/$TRIPLE-gcc" -std=gnu11 -O2 -Wall -Wno-unused-parameter -c -o "$CLI_DIR/wlgl.o" "$HERE/wlgl.c"
	"$CROSS/$TRIPLE-g++" -o "$HERE/root/tests/wlgl" "$CLI_DIR/wlgl.o" -Wl,--gc-sections \
		-lvulkan -ldraw -l9 -lfmt -lutf -lwayland-egl -lwayland-client -lffi
	# vkcube (scripts/mesa/vktools.sh: Wayland too)
	. "$ROOT/scripts/mesa/common.sh"
	bash "$ROOT/scripts/mesa/vktools.sh" build >/dev/null
	cp "$BUILD/vktools/vkcube" "$HERE/root/tests/vkcube"
	UFS_EXTRA=$HERE/root MEM=1024 CPU=host \
	QEMU_EXTRA="-vga none -display egl-headless -device virtio-gpu-gl-pci,blob=true,venus=true,hostmem=256M -object memory-backend-memfd,id=mem1,size=1024M,share=on -machine memory-backend=mem1" \
		setsid bash "$ROOT/scripts/guest/mcp-guest.sh" 'aux/virtiogpu' \
		>"$CLI_DIR/waylandvkguest.log" 2>&1 &
	guest=$!
	trap stop EXIT
	timeout 660 python3 "$ROOT/scripts/guest/mcp.py" wait 600 >/dev/null
	t0=$(date +%s)
	vk=$(timeout 700 python3 "$ROOT/scripts/guest/mcp.py" rc "bind -a '#i' /dev; bind -a '#m' /dev; /tests/wsys -x -i /tests/wlvk.rc >/tmp/wsys.log >[2=1]; cat /tmp/wsys.log /tmp/wlvk.log; sed 's/^/vkcube: /' /tmp/vkcube.log; sed 's/^/wlgl: /' /tmp/wlgl.log; sed 's/^/egl: /' /tmp/weston-simple-egl.log" 660) || true
	echo "wlvk and vkcube: $(($(date +%s) - t0)) s"
	echo "$vk"
	stop
	trap - EXIT
	echo "$vk" | grep -q '^wlvk: all passed' || { echo "FAIL wlvk"; fail=1; }
	echo "$vk" | grep -q '^wlgl: wlgl: all passed' || { echo "FAIL wlgl (EGL's Wayland platform)"; fail=1; }
	weston "$vk" egl simple-egl
	# vkcube on Mesa's Wayland WSI: its window, its frames presented, a clean exit
	frames=$(echo "$vk" | grep -cE '^vkcube: wlsys: ctl [0-9]+: present' || true)
	if echo "$vk" | grep -qE '^vkcube: wlsys: window [0-9]+ for 500x500' && [ "$frames" -ge 100 ] &&
	   echo "$vk" | grep -q '^vkcube: vkcube: exit $'; then
		echo "ok vkcube --wsi wayland: a window, $frames frames"
	else
		echo "FAIL vkcube --wsi wayland"; fail=1
	fi
fi
[ $fail = 0 ] && { echo "wayland: all passed"; exit 0; }
echo "wayland: failed"
exit 1
