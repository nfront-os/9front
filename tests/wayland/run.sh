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
# the libraries for foot (scripts/ape/vendor.sh: freetype, pixman, expat, xkbcommon, fontconfig, fcft), if built
if [ -e "$ROOT/amd64/lib.gcc/ape/libfcft.a" ]; then
	"$CROSS/$TRIPLE-gcc" -std=gnu11 -O2 -Wall -I"$ROOT/sys/include/ape/freetype2" -I"$ROOT/sys/include/ape/pixman-1" \
		-o "$HERE/root/tests/libtest" "$HERE/libtest.c" -lfcft -lfontconfig -lfreetype -lpixman-1 -lexpat -lxkbcommon -lm
fi
# weston's examples (scripts/wayland/weston.sh: simple-shm, simple-egl)
bash "$ROOT/scripts/wayland/weston.sh" build "$HERE/root/tests" >/dev/null
cp "$HERE/wlweston.rc" "$HERE/root/tests/wlweston.rc"
for f in restartA.rc wsysrestart.rc restartdesk.rc restartwin.rc inwin.rc wlnest.rc wlnestwin.rc wlfoot.rc; do
	cp "$HERE/$f" "$HERE/root/tests/$f"
	chmod +x "$HERE/root/tests/$f"
done
chmod +x "$HERE/root/tests/wlweston.rc"
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
	wl=$($M rc "bind -a '#i' /dev; bind -a '#m' /dev; /tests/wsys -x -i /tests/wlsys.rc >/tmp/wsys.log >[2=1]; cat /tmp/wsys.log /tmp/wlclient.log; sed 's/^/shm: /' /tmp/weston-simple-shm.log; sed 's/^/inwin: /' /tmp/inwinwatch.log /tmp/inwin.log" 300) || true
	echo "$wl"
	$M rc "cat /tmp/keymap.xkb" > "$CLI_DIR/wlsys-keymap.xkb" 2>/dev/null || true
	# the libraries for foot on 9front (libtest: xkbcommon compiles wlclient's keymap)
	lt=$($M rc "test ! -x /tests/libtest || /tests/libtest" 120) || true
	echo "$lt"
	# foot (/bin/foot, the ape step's) under wlsys: rc in its pty, foot ends with it
	ft=$($M rc "test ! -x /bin/foot || { bind -a '#i' /dev; bind -a '#m' /dev; /tests/wsys -x -i /tests/wlfoot.rc >/tmp/wsysfoot.log >[2=1]; cat /tmp/foot.tty; grep -e goodbye -e '^exit' -e ' err:' /tmp/foot.log }" 120) || true
	echo "$ft" | sed 's/^/foot: /'
	# the listening sockets' /srv entries (APE: UD.name) gone with their servers
	ud=$($M rc "ls /srv | grep '^/srv/UD\.' || echo none" 30) || true
	# a Wayland client outlives its wsys: wlsys makes its window in the next one
	rs=$($M rc "/tests/wsysrestart.rc" 200) || true
	echo "$rs"
	# the same in a window of a wsys (a desktop, as wlrun on a machine)
	rd=$($M rc "bind -a '#i' /dev; bind -a '#m' /dev; /tests/wsys -x -i /tests/restartdesk.rc >/tmp/wsysD.log >[2=1]; cat /tmp/restartdesk.log" 200) || true
	echo "$rd" | sed 's/^/desk: /'
	# wlclient under a wsys in a window of another (as wlrun on a machine): that
	# screen not at 0 0, the snarf buffer the outer one's
	wn=$($M rc "bind -a '#i' /dev; bind -a '#m' /dev; /tests/wsys -x -i /tests/wlnest.rc >/tmp/wsysO.log >[2=1]; cat /tmp/wlnest.log" 300) || true
	echo "$wn" | grep -E '^(FAIL|wlclient:)' | sed 's/^/nest: /'
	stop
	trap - EXIT
	echo "$out" | grep -q '^wltest: all passed' || { echo "FAIL wltest"; fail=1; }
	echo "$wl" | grep -q '^wlclient: all passed' || { echo "FAIL wlclient"; fail=1; }
	if [ -x "$HERE/root/tests/libtest" ]; then
		echo "$lt" | grep -q '^libtest: all passed' || { echo "FAIL libtest (freetype, pixman, expat, xkbcommon, fontconfig, fcft)"; fail=1; }
	fi
	if [ -x "$ROOT/amd64/bin/foot" ]; then
		echo "$ft" | grep -q '^TERM foot' && echo "$ft" | grep -q '^IOR ' && echo "ok foot: rc in its pty (TERM, the pty's modes)" ||
			{ echo "FAIL foot: rc in its pty: $(echo "$ft" | tr '\n' ' ')"; fail=1; }
		echo "$ft" | grep -q 'goodbye' && echo "ok foot: gone with its shell (SIGCHLD)" ||
			{ echo "FAIL foot: not gone with its shell: $(echo "$ft" | tr '\n' ' ')"; fail=1; }
	fi
	[ "$(echo "$ud" | tr -d '\r' | tail -1)" = none ] && echo "ok no socket left in /srv after wltest and wlsys exited" ||
		{ echo "FAIL sockets left in /srv: $(echo "$ud" | tr '\n' ' ')"; fail=1; }
	echo "$wn" | grep -q '^wlclient: all passed' && echo "ok wlclient under a wsys in a window of wsys" ||
		{ echo "FAIL wlclient under a wsys in a window of wsys"; fail=1; }
	weston "$wl" shm simple-shm
	echo "$wl" | grep -q '^inwin: wlwatch simple-shm: all passed' &&
		echo "ok wlsys started in a window of wsys (it mounts \$wsys)" ||
		{ echo "FAIL wlsys started in a window of wsys"; echo "$wl" | grep '^inwin:' | head -5; fail=1; }
	for w in "wlsys outlives wsys A (exited)" "wlsys outlives wsys B (killed)" "simple-shm gone after the delete in wsys C"; do
		[ "$(echo "$rs" | grep -c "^ok $w")" -ge 1 ] || { echo "FAIL wsys restart: $w"; fail=1; }
	done
	[ "$(echo "$rs" | grep -c '^ok simple-shm: it moves')" = 3 ] ||
		{ echo "FAIL wsys restart: the window drawn and moving in all three wsys"; fail=1; }
	for w in "wlsys outlives wsys A (exited)" "/srv/wsys.[a-z]* mounted: wsys B" "wlsys outlives wsys B (killed)" "/srv/wsys.[a-z]* mounted: wsys C" "simple-shm gone after the delete in wsys C"; do
		[ "$(echo "$rd" | grep -c "^ok $w")" -ge 1 ] || { echo "FAIL wsys restart in a window of wsys: $w"; fail=1; }
	done
	[ "$(echo "$rd" | grep -c '^ok simple-shm: it moves')" = 3 ] ||
		{ echo "FAIL wsys restart in a window of wsys: the window drawn and moving in all three wsys"; fail=1; }
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
	vk=$(timeout 700 python3 "$ROOT/scripts/guest/mcp.py" rc "bind -a '#i' /dev; bind -a '#m' /dev; WSYSDEBUG=layers /tests/wsys -x -i /tests/wlvk.rc >/tmp/wsys.log >[2=1]; cat /tmp/wsys.log /tmp/wlvk.log; sed 's/^/vkcube: /' /tmp/vkcube.log; sed 's/^/wlgl: /' /tmp/wlgl.log; sed 's/^/egl: /' /tmp/weston-simple-egl.log" 660) || true
	echo "wlvk and vkcube: $(($(date +%s) - t0)) s"
	echo "$vk"
	stop
	trap - EXIT
	echo "$vk" | grep -q '^wlvk: all passed' || { echo "FAIL wlvk"; fail=1; }
	# its subsurface of GPU buffers composed by the GPU, the alpha blended (not drawn by draw(3))
	if echo "$vk" | grep -q 'layers composed on the GPU' && ! echo "$vk" | grep -q 'no blending'; then
		echo "ok wlvk: a subsurface's layers composed and blended on the GPU"
	else
		echo "FAIL wlvk: the subsurface's layers not composed on the GPU"; fail=1
	fi
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
