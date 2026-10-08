#!/bin/bash
# Boot the gcc-built pc64 kernel in QEMU into the system, with the root
# file system served by the build host: an rc on the serial console
# (service=cpu) or rio (service=terminal, see KARGS).
#
# Guest boot path, 9front's own: initcode -> /boot/boot -> bootrc with
# nobootprompt=tcp: ipconfig (QEMU's DHCP), srv tcp!fs!564 -- QEMU
# forwards fs (10.0.2.100) port 564 to the host's ufs -- factotum,
# init -> newns(/lib/namespace) -> cpurc or termrc.
#
# Host side: builds the root tree (scripts/gen-ufs-root.sh) and a GRUB
# multiboot ISO in $CLI_DIR, starts Harvey's ufs on :5640, runs QEMU.
#
# usage: scripts/boot-cli.sh               interactive (Ctrl-A x quits)
#        scripts/boot-cli.sh cmd [cmd ...]  type each argument as one line
#                                          at the rc prompt, print the
#                                          console log, quit
#   env:  CLI_DIR     work dir (default ${XDG_CACHE_HOME:-~/.cache}/9front-cli)
#         UFS         ufs binary (default: ufs in PATH, else ~/go/bin/ufs)
#         SERVICE     cpu (default: cpurc, console rc) or terminal
#         SCREENSHOT  PNG file: screen dump after the commands (QMP)
#         SRC9        a directory the guest can mount: a second ufs serves
#                     it as tcp!10.0.2.101!5641 (srv tcp!10.0.2.101!5641 src9)
#         DISK        raw disk image attached as the first IDE disk (sdC0)
#         WAITFOR     after the commands, wait until the console shows this
#                     (grep -E), at most WAITMAX seconds (3600)
#         BOOTDISK    boot this disk image instead (scripts/mkdisk.sh): no
#                     host root, the image's GRUB, kernel and hjfs
#         KARGS       more kernel parameters (plan9.ini variables), e.g.
#                     "vgasize=1024x768x16 monitor=vesa mouseport=ps2"
#         MEM NIC     QEMU memory (256) and NIC model (e1000)
#         QEMU_EXTRA  extra QEMU flags (e.g. "-s -S" for gdb)
#         UFS_EXTRA   directory whose contents are added to the root
#                     (tests/guest puts its test programs in /tests)
#         BOOTWAIT    seconds before typing the first line (15); meanwhile
#                     factotum's nvram prompts get AUTHID (glenda),
#                     AUTHDOM (9front), PASSWORD (gccport9), empty
#                     secstore key and the p9sk1 default
#         CMDWAIT     seconds per line (2)
set -e

ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
KERNEL=$ROOT/amd64/bin/9pc64
UFS=${UFS:-$(command -v ufs 2>/dev/null || echo "$HOME/go/bin/ufs")}
UFS_PORT=5640
SERVICE=${SERVICE:-cpu}
MEM=${MEM:-256}
NIC=${NIC:-e1000}
# bootrc's tcp root: fs!564, which QEMU forwards to the host's ufs
FSADDR=10.0.2.100
SRC9ADDR=10.0.2.101
SRC9_PORT=5641

die(){ echo "boot-cli: $*" >&2; exit 1; }
port_open(){ timeout 2 bash -c "exec 3<>/dev/tcp/127.0.0.1/$UFS_PORT" 2>/dev/null; }

cmds=("$@")

[ -f "$KERNEL" ] || die "$KERNEL missing, run scripts/build-cli.sh"
command -v qemu-system-x86_64 >/dev/null || die "qemu-system-x86_64 missing"
mkdir -p "$CLI_DIR"
pids=()
cleanup(){ [ ${#pids[@]} -gt 0 ] && kill "${pids[@]}" 2>/dev/null; wait 2>/dev/null; return 0; }
trap cleanup EXIT

if [ -n "$BOOTDISK" ]; then
	# an installed system (scripts/mkdisk.sh): its own boot loader and root
	[ -f "$BOOTDISK" ] || die "$BOOTDISK missing"
	qemu=(qemu-system-x86_64 -drive "file=$BOOTDISK,format=raw,if=ide,index=0" -m "$MEM"
		-display none -no-reboot -monitor none
		-netdev user,id=net0 -device "$NIC",netdev=net0
		-cpu qemu64,+rdrand)
else
command -v grub-mkrescue >/dev/null || die "grub-mkrescue missing"
[ -x "$UFS" ] || die "ufs missing (go install github.com/Harvey-OS/ninep/cmd/ufs@latest, or set UFS)"
# a ufs left over from an earlier run would serve a stale root
port_open && die "port $UFS_PORT is in use (old ufs still running?)"

# root file system
bash "$ROOT/scripts/gen-ufs-root.sh" "$CLI_DIR/ufsroot" > /dev/null
[ -z "$UFS_EXTRA" ] || cp -r "$UFS_EXTRA"/. "$CLI_DIR/ufsroot/"

# GRUB multiboot ISO; GRUB itself also talks on the serial line, so a
# boot that stops at the grub> prompt is visible
rm -rf "$CLI_DIR/iso"
mkdir -p "$CLI_DIR/iso/boot/grub"
cp "$KERNEL" "$CLI_DIR/iso/boot/9pc64"
cat > "$CLI_DIR/iso/boot/grub/grub.cfg" <<EOF
set timeout=0
serial --speed=115200 --unit=0 --word=8 --parity=no --stop=1
terminal_output serial console
multiboot /boot/9pc64 console=0 service=$SERVICE nobootprompt=tcp fs=$FSADDR auth=$FSADDR $KARGS
boot
EOF
grub-mkrescue -o "$CLI_DIR/9pc64.iso" "$CLI_DIR/iso" > "$CLI_DIR/grub-mkrescue.log" 2>&1 \
	|| die "grub-mkrescue failed (log: $CLI_DIR/grub-mkrescue.log)"

"$UFS" -root="$CLI_DIR/ufsroot" -addr=":$UFS_PORT" > "$CLI_DIR/ufs.log" 2>&1 &
pids+=($!)
for _ in $(seq 50); do port_open && break; sleep 0.2; done
port_open || die "ufs did not come up (log: $CLI_DIR/ufs.log)"
fwd=guestfwd=tcp:$FSADDR:564-tcp:127.0.0.1:$UFS_PORT
if [ -n "$SRC9" ]; then
	"$UFS" -root="$SRC9" -addr=":$SRC9_PORT" > "$CLI_DIR/ufs-src9.log" 2>&1 &
	pids+=($!)
	fwd=$fwd,guestfwd=tcp:$SRC9ADDR:$SRC9_PORT-tcp:127.0.0.1:$SRC9_PORT	# not 564 too: QEMU names the forwards by guest port
fi
disk=()
[ -z "$DISK" ] || disk=(-drive "file=$DISK,format=raw,if=ide,index=0,media=disk")

qemu=(qemu-system-x86_64 -cdrom "$CLI_DIR/9pc64.iso" -m "$MEM"
	-display none -no-reboot -monitor none
	-netdev user,id=net0,$fwd -device "$NIC",netdev=net0 "${disk[@]}"
	-cpu qemu64,+rdrand)	# RDRAND: kernel entropy fast path, dial() needs it
fi

if [ ${#cmds[@]} -eq 0 ]; then
	echo "boot-cli: starting QEMU (Ctrl-A x quits)" >&2
	"${qemu[@]}" -serial mon:stdio $QEMU_EXTRA
	exit
fi

# scripted: serial console on a pty, type the commands, collect output
qmp=()
[ -z "$SCREENSHOT" ] || { rm -f "$CLI_DIR/qmp.sock"; qmp=(-qmp "unix:$CLI_DIR/qmp.sock,server,nowait"); }
"${qemu[@]}" -serial pty "${qmp[@]}" $QEMU_EXTRA > "$CLI_DIR/qemu.log" 2>&1 &
pids+=($!)
for _ in $(seq 50); do
	pts=$(grep -oE "/dev/pts/[0-9]+" "$CLI_DIR/qemu.log" | head -1)
	[ -n "$pts" ] && break
	sleep 0.1
done
[ -n "$pts" ] || die "no QEMU serial pty (log: $CLI_DIR/qemu.log)"
stty -F "$pts" raw -echo
cat "$pts" > "$CLI_DIR/console.log" &
pids+=($!)
# while booting, answer factotum's nvram questions (readnvram(2): no
# nvram in QEMU) as the console user would; each prompt once
answered=0
end=$((SECONDS + ${BOOTWAIT:-15}))
while [ $SECONDS -lt $end ]; do
	log=$(tr -d '\r' < "$CLI_DIR/console.log")
	last=${log##*$'\n'}
	ans=-
	if [ ${#log} -gt $answered ]; then
		case $last in
		*'authid: ')			ans=${AUTHID:-glenda} ;;
		*'authdom: ')			ans=${AUTHDOM:-9front} ;;
		*'secstore key: ')		ans= ;;
		*'password: ')			ans=${PASSWORD:-gccport9} ;;	# and confirm password
		*'enable legacy p9sk1'*': ')	ans= ;;
		*'user['*']: ')		ans= ;;	# -u: hostowner, default
		esac
	fi
	if [ "$ans" != - ]; then
		printf '%s\r' "$ans" > "$pts"
		answered=${#log}
	fi
	sleep 0.5
done
for c in "${cmds[@]}"; do
	printf '%s\r' "$c" > "$pts"
	sleep "${CMDWAIT:-2}"
done
if [ -n "$WAITFOR" ]; then
	end=$((SECONDS + ${WAITMAX:-3600}))
	until tr -d '\r' < "$CLI_DIR/console.log" | grep -qE "$WAITFOR" || [ $SECONDS -ge $end ]; do
		sleep 2
	done
fi
sleep 2
if [ -n "$SCREENSHOT" ]; then	# QMP screendump (PPM), stored as PNG
	python3 - "$CLI_DIR/qmp.sock" "$CLI_DIR/screen.ppm" "$SCREENSHOT" <<'PY'
import json, socket, struct, sys, zlib
s = socket.socket(socket.AF_UNIX)
s.connect(sys.argv[1])
f = s.makefile('rw')
f.readline()
for cmd in ({'execute': 'qmp_capabilities'},
		{'execute': 'screendump', 'arguments': {'filename': sys.argv[2]}}):
	f.write(json.dumps(cmd) + '\n')
	f.flush()
	f.readline()
d = open(sys.argv[2], 'rb').read()
magic, w, h, mx, px = d.split(maxsplit=4)
w, h = int(w), int(h)
raw = b''.join(b'\0' + px[y*w*3:(y+1)*w*3] for y in range(h))
def chunk(t, b):
	return struct.pack('>I', len(b)) + t + b + struct.pack('>I', zlib.crc32(t + b))
open(sys.argv[3], 'wb').write(b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 2, 0, 0, 0))
	+ chunk(b'IDAT', zlib.compress(raw)) + chunk(b'IEND', b''))
PY
fi
cleanup
pids=()
tr -d '\r' < "$CLI_DIR/console.log"
