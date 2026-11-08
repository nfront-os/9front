#!/bin/bash
# Boot the gcc-built pc64 kernel (objtype=386: pc) in QEMU into the system, with the root
# file system served by the build host: an rc on the serial console
# (service=cpu) or rio (service=terminal, see KARGS).
#
# Guest boot path, 9front's own: initcode -> /boot/boot -> bootrc with
# nobootprompt=tcp: ipconfig (QEMU's DHCP), srv tcp!fs!564 -- QEMU
# forwards fs (10.0.2.100) port 564 to the host's ufs -- factotum,
# init -> newns(/lib/namespace) -> cpurc or termrc.
#
# Host side: builds the root tree (scripts/guest/gen-ufs-root.sh) and a GRUB
# multiboot ISO in $CLI_DIR, starts Harvey's ufs on :5640, runs QEMU.
#
# usage: scripts/guest/boot-cli.sh               interactive (Ctrl-A x quits)
#        scripts/guest/boot-cli.sh cmd [cmd ...]  type each argument as one line
#                                          at the rc prompt, print the
#                                          console log, quit
#   env:  CLI_DIR     work dir (default ${XDG_CACHE_HOME:-~/.cache}/9front-cli)
#         UFS         ufs binary (default: ufs in PATH, else ~/go/bin/ufs)
#         SERVICE     cpu (default: cpurc, console rc) or terminal
#         SCREENSHOT  PNG file: screen dump after the commands (QMP)
#         VNC         1: QEMU serves VNC on $CLI_DIR/vnc.sock and SCREENSHOT
#                     comes from there (scripts/debug/vncshot.py): what
#                     the host shows, also with a GL display, where QMP
#                     has no surface; SHOTPOINTS "x,y ..." writes their
#                     colours to the PNG's name with .txt
#         SRC9        a directory the guest can mount: a second ufs serves
#                     it as tcp!10.0.2.101!5641 (srv tcp!10.0.2.101!5641 src9)
#         DISK        raw disk image attached as the first IDE disk (sdC0)
#         WAITFOR     after the commands, wait until the console shows this
#                     (grep -E), at most WAITMAX seconds (3600)
#         BOOTDISK    boot this disk image instead (scripts/guest/mkdisk.sh): no
#                     host root, the image's boot loader, kernel and hjfs
#         BOOTISO     boot this CD image instead (scripts/guest/mkiso.sh)
#         UEFI        1: UEFI firmware (OVMF, $OVMF_CODE) instead of the BIOS
#         KVM         0: no KVM acceleration; otherwise x86 guests need /dev/kvm
#                     (an arm64 guest on an x86 host is always emulated)
#         KARGS       more kernel parameters (plan9.ini variables), e.g.
#                     "vgasize=1024x768x16 monitor=vesa mouseport=ps2"
#         MEM NIC     QEMU memory (256) and NIC model (e1000)
#         PMU         1: x86 guests get KVM's virtual performance counters
#                     (pmu=on; +perfctr-core: AMD's 6 core counters); qemu64
#                     has none by default. arm64 (TCG) always has PMUv3
#         SMP         QEMU processors (QEMU's default: 1; tests/lib.sh sets 4)
#         CPU         QEMU CPU model (amd64: qemu64,+rdrand; e.g. host with KVM)
#         QEMU_EXTRA  extra QEMU flags (e.g. "-s -S" for gdb)
#         BOOTKERNEL  the kernel to boot (default: the build's
#                     $objtype/bin/$KNAME; e.g. one built with kencc);
#                     on arm64 its u-boot image (default arm64/9qemu.u)
#         UFS_EXTRA   directory whose contents are added to the root
#                     (tests/guest puts its test programs in /tests)
#         BOOTWAIT    seconds at most for the boot: until rc prompts (300);
#                     each line waits for rc's prompt (PROMPTWAIT, 300 s); meanwhile
#                     factotum's nvram prompts get AUTHID (glenda),
#                     AUTHDOM (9front), PASSWORD (gccport9), empty
#                     secstore key and the p9sk1 default
#         CMDWAIT     seconds at most per line: until rc prompts after its
#                     output (60; input typed into a rio window gets no prompt
#                     on the serial line, there it is the pause)
#         HOLD        1: after the commands keep the guest running (until
#                     this script is killed); scripts/guest/guest-cmd.sh types
#                     further lines into it
set -e

ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
# objtype: the system to boot, amd64 (pc64) or 386 (pc)
case ${objtype:=amd64} in
amd64)	KNAME=9pc64 QEMU=qemu-system-x86_64 CPU=${CPU:-qemu64,+rdrand} ;;
386)	KNAME=9pc QEMU=qemu-system-i386 CPU=${CPU:-qemu32,+rdrand} ;;
arm64)	KNAME=9qemu QEMU=qemu-system-aarch64 CPU=${CPU:-cortex-a72} ;;
*)	echo "boot-cli: objtype=$objtype: no kernel" >&2; exit 1 ;;
esac
[ "${PMU:-0}" = 0 ] || [ $objtype = arm64 ] || CPU=$CPU,pmu=on,+perfctr-core	# pmc(3) tests (tests/pmc)
export objtype
KERNEL=${BOOTKERNEL:-$ROOT/$objtype/bin/$KNAME}
UFS=${UFS:-$(command -v ufs 2>/dev/null || echo "$HOME/go/bin/ufs")}
UFS_PORT=${UFS_PORT:-5640}	# host ports of the ufs servers (another boot-cli may run)
SERVICE=${SERVICE:-cpu}
MEM=${MEM:-256}
NIC=${NIC:-e1000}
# bootrc's tcp root: fs!564, which QEMU forwards to the host's ufs
FSADDR=10.0.2.100
SRC9ADDR=10.0.2.101
# KVM always for x86 guests (KVM=0: emulation); same CPU model either way
accel=()
if [ "${KVM:-1}" != 0 ] && [ $objtype != arm64 ]; then
	[ -w /dev/kvm ] || { echo "boot-cli: /dev/kvm not writable (KVM=0 to emulate)" >&2; exit 1; }
	accel=(-accel kvm)
fi
SRC9_PORT=${SRC9_PORT:-5641}	# host port of the src9 ufs; the guest's is always 5641

die(){ echo "boot-cli: $*" >&2; exit 1; }
port_open(){ timeout 2 bash -c "exec 3<>/dev/tcp/127.0.0.1/$UFS_PORT" 2>/dev/null; }

cmds=("$@")

[ -f "$KERNEL" ] || die "$KERNEL missing, run CC=gcc ARCH=$objtype sys/src/cmd/build/build.exe"
command -v $QEMU >/dev/null || die "$QEMU missing"
mkdir -p "$CLI_DIR"
pids=()
cleanup(){ [ ${#pids[@]} -gt 0 ] && kill "${pids[@]}" 2>/dev/null; wait 2>/dev/null; return 0; }
trap cleanup EXIT

fw=()
if [ "${UEFI:-0}" != 0 ]; then
	OVMF_CODE=${OVMF_CODE:-/usr/share/edk2/x64/OVMF_CODE.4m.fd}
	[ -f "$OVMF_CODE" ] || die "no OVMF firmware ($OVMF_CODE)"
	cp "${OVMF_CODE%CODE*}VARS${OVMF_CODE##*CODE}" "$CLI_DIR/ovmf_vars.fd" || die "no OVMF variable store"
	fw=(-drive "if=pflash,format=raw,readonly=on,file=$OVMF_CODE"
		-drive "if=pflash,format=raw,file=$CLI_DIR/ovmf_vars.fd")
fi
if [ -n "$BOOTDISK$BOOTISO" ]; then
	# an installed system (scripts/guest/mkdisk.sh) or the CD (scripts/guest/mkiso.sh):
	# its own boot loader and root
	if [ -n "$BOOTISO" ]; then
		[ -f "$BOOTISO" ] || die "$BOOTISO missing"
		media=(-drive "file=$BOOTISO,format=raw,if=ide,index=2,media=cdrom")
	else
		[ -f "$BOOTDISK" ] || die "$BOOTDISK missing"
		media=(-drive "file=$BOOTDISK,format=raw,if=ide,index=0")
	fi
	qemu=($QEMU "${fw[@]}" "${media[@]}" -m "$MEM" ${SMP:+-smp "$SMP"}
		-display none -no-reboot -monitor none
		-netdev user,id=net0 -device "$NIC",netdev=net0
		-cpu $CPU "${accel[@]}")
else
[ $objtype = arm64 ] || command -v grub-mkrescue >/dev/null || die "grub-mkrescue missing"
[ -x "$UFS" ] || die "ufs missing (go install github.com/Harvey-OS/ninep/cmd/ufs@latest, or set UFS)"
# a ufs left over from an earlier run would serve a stale root
port_open && die "port $UFS_PORT is in use (old ufs still running?)"

# root file system
bash "$ROOT/scripts/guest/gen-ufs-root.sh" "$CLI_DIR/ufsroot" > /dev/null
[ -z "$UFS_EXTRA" ] || cp -r "$UFS_EXTRA"/. "$CLI_DIR/ufsroot/"

if [ $objtype = arm64 ]; then
	# 9front's arm64 qemu boot (sys/lib/dist/mkfile, %.arm64.qcow2):
	# u-boot (scripts/guest/build-uboot.sh) runs boot.scr from the FAT
	# partition, which loads plan9.ini and the u-boot image 9qemu.u
	# (scripts/guest/build-boot.sh makes both with aux/txt2uimage, aux/aout2uimage)
	# BOOTKERNEL: another u-boot image (e.g. build-kencc.sh arm64's)
	UIMAGE=${BOOTKERNEL:-$ROOT/arm64/9qemu.u}
	for f in "$ROOT/sys/src/boot/qemu/boot.scr" "$UIMAGE"; do
		[ -s "$f" ] || die "$f missing, run scripts/guest/build-boot.sh"
	done
	# 9qemu.u is made from 9qemu by build-boot.sh: an older one would
	# boot the kernel of an earlier build, silently
	[ -n "$BOOTKERNEL" ] || [ ! "$ROOT/arm64/9qemu" -nt "$ROOT/arm64/9qemu.u" ] \
		|| die "arm64/9qemu.u is older than arm64/9qemu, run scripts/guest/build-boot.sh"
	bash "$ROOT/scripts/guest/build-uboot.sh" > "$CLI_DIR/uboot.log" 2>&1 \
		|| die "u-boot build failed (log: $CLI_DIR/uboot.log)"
	{
		echo console=0
		echo service=$SERVICE
		echo nobootprompt=tcp
		echo fs=$FSADDR
		echo auth=$FSADDR
		for a in $KARGS; do echo "$a"; done
	} > "$CLI_DIR/plan9.ini"
	FAT=$CLI_DIR/arm64boot.img
	rm -f "$FAT"
	truncate -s 64M "$FAT"
	echo 'start=2048, type=c, bootable' | sfdisk -q "$FAT" > /dev/null
	mformat -i "$FAT@@1M" -F :: > /dev/null
	cp "$UIMAGE" "$CLI_DIR/9qemu.u"
	mcopy -i "$FAT@@1M" "$ROOT/sys/src/boot/qemu/boot.scr" "$CLI_DIR/plan9.ini" "$CLI_DIR/9qemu.u" ::
else
	# GRUB multiboot ISO; GRUB itself also talks on the serial line, so a
	# boot that stops at the grub> prompt is visible
	rm -rf "$CLI_DIR/iso"
	mkdir -p "$CLI_DIR/iso/boot/grub"
	cp "$KERNEL" "$CLI_DIR/iso/boot/$KNAME"
	cat > "$CLI_DIR/iso/boot/grub/grub.cfg" <<-EOF
	set timeout=0
	serial --speed=115200 --unit=0 --word=8 --parity=no --stop=1
	terminal_output serial console
	multiboot /boot/$KNAME console=0 service=$SERVICE nobootprompt=tcp fs=$FSADDR auth=$FSADDR $KARGS
	boot
	EOF
	grub-mkrescue -o "$CLI_DIR/$KNAME.iso" "$CLI_DIR/iso" > "$CLI_DIR/grub-mkrescue.log" 2>&1 \
		|| die "grub-mkrescue failed (log: $CLI_DIR/grub-mkrescue.log)"
fi

"$UFS" -root="$CLI_DIR/ufsroot" -addr=":$UFS_PORT" > "$CLI_DIR/ufs.log" 2>&1 &
pids+=($!)
for _ in $(seq 50); do port_open && break; sleep 0.2; done
port_open || die "ufs did not come up (log: $CLI_DIR/ufs.log)"
fwd=guestfwd=tcp:$FSADDR:564-tcp:127.0.0.1:$UFS_PORT
if [ -n "$SRC9" ]; then
	"$UFS" -root="$SRC9" -addr=":$SRC9_PORT" > "$CLI_DIR/ufs-src9.log" 2>&1 &
	pids+=($!)
	# the guest always dials tcp!10.0.2.101!5641 (not 564 too: QEMU
	# names the forwards by guest port); SRC9_PORT is the host's side
	fwd=$fwd,guestfwd=tcp:$SRC9ADDR:5641-tcp:127.0.0.1:$SRC9_PORT
fi
disk=()
# the kernel CD boots first: the BIOS would try a disk with a boot
# signature (any MBR, a GPT's protective one) before the CD
[ -z "$DISK" ] || disk=(-drive "file=$DISK,format=raw,if=ide,index=0,media=disk" -boot order=d)

if [ $objtype = arm64 ]; then
	# pciqemu.c expects the low PCIe ECAM window (0x3f000000): highmem=off
	[ -z "$DISK" ] || disk=(-drive "file=$DISK,format=raw,if=none,id=d0" -device virtio-blk-pci,drive=d0,disable-legacy=on)
	qemu=($QEMU -M virt,gic-version=3,highmem=off -cpu $CPU -m "$MEM" ${SMP:+-smp "$SMP"}
		-bios "$CLI_DIR/uboot/u-boot.bin"
		-drive "file=$FAT,format=raw,if=none,id=fat" -device virtio-blk-pci,drive=fat,disable-legacy=on
		-display none -no-reboot -monitor none
		-netdev user,id=net0,$fwd -device virtio-net-pci,netdev=net0,disable-legacy=on "${disk[@]}")	# virtio 1.0 only (ethervirtio10.c, sdvirtio10.c)
else
qemu=($QEMU "${fw[@]}" -cdrom "$CLI_DIR/$KNAME.iso" -m "$MEM" ${SMP:+-smp "$SMP"}
	-display none -no-reboot -monitor none
	-netdev user,id=net0,$fwd -device "$NIC",netdev=net0 "${disk[@]}"
	-cpu $CPU "${accel[@]}")	# RDRAND: kernel entropy fast path, dial() needs it
fi
fi

if [ ${#cmds[@]} -eq 0 ]; then
	echo "boot-cli: starting QEMU (Ctrl-A x quits)" >&2
	"${qemu[@]}" -serial mon:stdio $QEMU_EXTRA
	exit
fi

# scripted: serial console on a pty, type the commands, collect output
qmp=()
[ -z "$SCREENSHOT" ] || { rm -f "$CLI_DIR/qmp.sock"; qmp=(-qmp "unix:$CLI_DIR/qmp.sock,server,nowait"); }
[ "$VNC" != 1 ] || { rm -f "$CLI_DIR/vnc.sock"; qmp+=(-vnc "unix:$CLI_DIR/vnc.sock"); }
"${qemu[@]}" -serial pty "${qmp[@]}" $QEMU_EXTRA > "$CLI_DIR/qemu.log" 2>&1 &
pids+=($!)
qemupid=$!
# the guest powered itself off (fshalt, scram): nothing more to wait for
running(){ kill -0 $qemupid 2>/dev/null; }
# up to 30 s: with VFIO, QEMU pins all of the guest's memory first (slow
# when the host must drop its page cache for it)
for _ in $(seq 300); do
	pts=$(grep -oE "/dev/pts/[0-9]+" "$CLI_DIR/qemu.log" | head -1)
	[ -n "$pts" ] && break
	running || break
	sleep 0.1
done
[ -n "$pts" ] || die "no QEMU serial pty (log: $CLI_DIR/qemu.log)"
# QEMU gone at once (seen now and then right after another guest): how
sleep 0.2
if ! running || ! stty -F "$pts" raw -echo 2>/dev/null; then
	wait $qemupid 2>/dev/null
	st=$?
	echo "boot-cli: QEMU ended at the start (status $st); its log:" >&2
	cat "$CLI_DIR/qemu.log" >&2
	echo "boot-cli: QEMUs still running:" >&2
	ps -eo pid,etime,args | grep '[q]emu-system' >&2
	die "QEMU did not start"
fi
# empty now, not in the background job: the prompt checks below must
# not see the previous boot's log
: > "$CLI_DIR/console.log"
cat "$pts" >> "$CLI_DIR/console.log" &
pids+=($!)
# type a line only when rc prompts for it (term% , sysname# ): input that
# arrives while init and cpurc/termrc still run gets lost
prompted(){
	local log
	log=$(tr -d '\r' < "$CLI_DIR/console.log")
	case ${log##*$'\n'} in
	*'% '|*'# ')	return 0 ;;
	esac
	return 1
}
# rc prompted since the console log had $1 bytes: its prompt $2 begins a
# line after the echoed input line. Anywhere, not only at the end: a
# command in the background (cmd &) may write after the prompt.
replied(){
	local out
	out=$(tail -c +$(($1 + 1)) "$CLI_DIR/console.log" | tr -d '\r')
	case $out in
	*$'\n'"$2"*)	return 0 ;;
	esac
	return 1
}
# while booting, answer factotum's nvram questions (readnvram(2): no
# nvram in QEMU) as the console user would; each prompt once
answered=0
end=$((SECONDS + ${BOOTWAIT:-300}))
while [ $SECONDS -lt $end ] && ! prompted; do
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
		*'bootargs is ('*')['*'] ')	ans= ;;	# the default bootrc found
		esac
	fi
	if [ "$ans" != - ]; then
		printf '%s\r' "$ans" > "$pts"
		answered=${#log}
	fi
	sleep 0.5
done
ready=0	# rc prompted after the last line (replied)
prompt=
for c in "${cmds[@]}"; do
	end=$((SECONDS + ${PROMPTWAIT:-300}))
	until [ $ready = 1 ] || prompted || [ $SECONDS -ge $end ] || ! running; do
		sleep 1
	done
	running || break
	# rc's prompt as it stands at the end of the log (it may change)
	if [ $ready = 0 ] && prompted; then
		prompt=$(tr -d '\r' < "$CLI_DIR/console.log")
		prompt=${prompt##*$'\n'}
	fi
	ready=0
	# in pieces: the serial input buffer drops what exceeds about 128
	# bytes at once (the rest of a long line, its newline with it)
	start=$(stat -c %s "$CLI_DIR/console.log")
	l=$c$'\r'
	while [ -n "$l" ]; do
		printf '%s' "${l:0:32}" > "$pts"
		l=${l:32}
		sleep 0.2
	done
	end=$((SECONDS + ${CMDWAIT:-60}))
	until replied $start "$prompt" || [ $SECONDS -ge $end ] || ! running; do
		sleep 0.5
	done
	replied $start "$prompt" && ready=1
done
if [ -n "$WAITFOR" ]; then
	end=$((SECONDS + ${WAITMAX:-3600}))
	until tr -d '\r' < "$CLI_DIR/console.log" | grep -qE "$WAITFOR" || [ $SECONDS -ge $end ] || ! running; do
		sleep 2
	done
fi
running || echo "boot-cli: the guest powered off" >> "$CLI_DIR/console.log"
sleep 1
if [ -n "$SCREENSHOT" ] && [ "$VNC" = 1 ]; then
	python3 "$ROOT/scripts/debug/vncshot.py" "$CLI_DIR/vnc.sock" "$SCREENSHOT" $SHOTPOINTS \
		> "${SCREENSHOT%.png}.txt" || exit 1
elif [ -n "$SCREENSHOT" ]; then	# QMP screendump (PPM), stored as PNG
	python3 - "$CLI_DIR/qmp.sock" "$CLI_DIR/screen.ppm" "$SCREENSHOT" <<'PY'
import json, os, socket, struct, sys, time, zlib
# no stale picture: the dump is new or there is none. With a GL display
# (egl-headless, virtio-gpu-gl) QEMU writes it asynchronously: wait for it.
if os.path.exists(sys.argv[2]):
	os.remove(sys.argv[2])
s = socket.socket(socket.AF_UNIX)
s.connect(sys.argv[1])
f = s.makefile('rw')
f.readline()
for cmd in ({'execute': 'qmp_capabilities'},
		{'execute': 'screendump', 'arguments': {'filename': sys.argv[2]}}):
	f.write(json.dumps(cmd) + '\n')
	f.flush()
	while True:	# the reply, past asynchronous events
		r = json.loads(f.readline())
		if 'return' in r or 'error' in r:
			break
	if 'error' in r:
		sys.exit('screendump: %s' % r['error'].get('desc', r['error']))
def complete(path):
	try:
		d = open(path, 'rb').read()
		magic, w, h, mx, px = d.split(maxsplit=4)
		return d if len(px) >= int(w)*int(h)*3 else None
	except (OSError, ValueError):
		return None
for i in range(150):
	d = complete(sys.argv[2])
	if d is not None:
		break
	time.sleep(0.1)
else:
	sys.exit('screendump: no picture in %s' % sys.argv[2])
magic, w, h, mx, px = d.split(maxsplit=4)
w, h = int(w), int(h)
raw = b''.join(b'\0' + px[y*w*3:(y+1)*w*3] for y in range(h))
def chunk(t, b):
	return struct.pack('>I', len(b)) + t + b + struct.pack('>I', zlib.crc32(t + b))
open(sys.argv[3], 'wb').write(b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 2, 0, 0, 0))
	+ chunk(b'IDAT', zlib.compress(raw)) + chunk(b'IEND', b''))
PY
fi
if [ -n "$HOLD" ]; then
	echo "$pts" > "$CLI_DIR/pts"
	echo "guest running on $pts: scripts/guest/guest-cmd.sh 'cmd'"
	wait
fi
cleanup
pids=()
tr -d '\r' < "$CLI_DIR/console.log"
