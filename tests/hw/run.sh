#!/bin/bash
# hw: the hardware test stick (tests/hw/README.md) dry run in QEMU: the
# image from scripts/hw/mkstick.sh (configs.qemu, serial console) boots
# from a USB stick on xHCI with KVM, runs the configurations with resets
# between them, survives a power-off during a step (HANG), and leaves
# the logs on its 9fat, read back with scripts/hw/collect.py.
#	env: STICK=image: use this image instead of making one
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
case $objtype in amd64) ;; *) skip "hw" "amd64 only"; done_tests; exit ;; esac
[ -w /dev/kvm ] || { fail "hw: /dev/kvm not writable"; done_tests; exit; }
# the image and the logs stay in $CLI_DIR for a look afterwards
IMG=${STICK:-$CLI_DIR/hwstick-dryrun.img}
LOGS=$CLI_DIR/hwtest-dryrun
if [ -z "$STICK" ]; then
	mk(){ CONSOLE=1 CONFIGS="$HERE/configs.qemu" bash "$ROOT/scripts/hw/mkstick.sh" "$IMG"; }
	check "make the stick image (scripts/hw/mkstick.sh)" mk || { done_tests; exit; }
fi
# an internal disk, writable (as a laptop's): cat/disk.rc must not write to it, its checksum is compared below
DISK=$CLI_DIR/hwtest-dryrun-disk.img
python3 - "$DISK" <<'PY'
import sys
# 256 MB with an MBR: one partition (type 0x83), a pattern in every MB
f = open(sys.argv[1], 'wb')
mbr = bytearray(512)
mbr[446:462] = bytes([0, 0, 0, 0, 0x83, 0, 0, 0]) + (2048).to_bytes(4, 'little') + (522240).to_bytes(4, 'little')
mbr[510:512] = b'\x55\xaa'
f.write(mbr)
for i in range(256):
	f.seek(i*1024*1024 + 4096)
	f.write(b'hwtest %d\n' % i)
f.truncate(256*1024*1024)
PY
DISKSUM=$(md5sum < "$DISK")
QPID=
stopvm(){ [ -n "$QPID" ] && kill "$QPID" 2>/dev/null; wait "$QPID" 2>/dev/null; QPID=; }
trap 'stopvm; rm -rf "$TMPD"' EXIT
# boot the stick until the console shows $2 (at most $3 seconds), then power off
bootuntil(){
	: > "$1"
	qemu-system-x86_64 -accel kvm -cpu qemu64,+rdrand -m 1024 -smp 4 -display none \
		-device qemu-xhci,id=xhci -drive if=none,id=stick,file="$IMG",format=raw \
		-device usb-storage,bus=xhci.0,drive=stick,bootindex=0 \
		-drive file="$DISK",if=ide,format=raw \
		-nic user,model=e1000 \
		-serial file:"$1" -monitor none > "$1.qemu" 2>&1 &
	QPID=$!
	for i in $(seq "$3"); do
		grep -qE "$2" "$1" 2>/dev/null && break
		kill -0 "$QPID" 2>/dev/null || break
		sleep 1
	done
	grep -qE "$2" "$1"; r=$?
	stopvm
	return $r
}
check "boot from the stick: default, then hang: power off in hangme" \
	bootuntil "$LOGS.con1" 'hwtest: hang: hangme' 1200
check "power on again: HANG logged, the run goes on to the end" \
	bootuntil "$LOGS.con2" 'hwtest: finished run' 600
coll(){ rm -rf "$LOGS"; python3 "$ROOT/scripts/hw/collect.py" "$IMG" "$LOGS"; }
check "collect the logs from the 9fat (scripts/hw/collect.py)" coll || { done_tests; exit; }
S=$LOGS/hwtest/run1
has(){ grep -qF "$2" "$S/$1/summary.txt"; }
check "default: sysinfo" has default 'sysinfo: 1 passed, 0 failed'
check "default: boot checks (8)" has default 'boot: 8 passed, 0 failed'
step(){ grep -qE "^$2: $3 passed, 0 failed, $4 skipped" "$S/$1/summary.txt"; }
check "default: guest unit tests, none failed" step default guest '[1-9][0-9]*' 0
check "default: disk: the IDE disk read (5; QEMU's empty CD skipped)" step default disk 5 '[01]'
check "default: usb (3)" step default usb 3 0
check "default: net: e1000, ping, tcp (3)" step default net 3 0
check "default: stress (2, and the clock if QEMU gives APERF)" step default stress '[234]' 0
check "default: gcc: gccmk builds and runs a program" step default gcc 1 0
check "nixac1: nix on the AC, none failed" step nixac1 nix '[1-9][0-9]*' '[0-9]*'
check "nixac1: big, none failed (26)" step nixac1 big 26 0
check "the internal disk is unchanged" test "$(md5sum < "$DISK")" = "$DISKSUM"
check "hang: hangme logged as HANG" has hang 'HANG hangme'
check "hang: the next step ran after the power-off" has hang 'boot: 8 passed, 0 failed'
check "again: booted after the reset into it" has again 'boot: 8 passed, 0 failed'
check "state finished" grep -q finished "$LOGS/hwtest/state"
check "kmesg saved" test -s "$S/again/kmesg.txt"
off=$(python3 "$ROOT/scripts/hw/collect.py" --offset "$IMG")
check "plan9.ini set back: no hwtest" bash -c "! (MTOOLS_SKIP_CHECK=1 mtype -i '$IMG@@$off' ::plan9.ini | grep -q '^hwtest=')"
check "the root came from a USB disk" grep -q 'root from a USB disk' "$S/default/boot.log"
[ $nfail -eq 0 ] || for f in "$S"/*/summary.txt; do echo "== $f"; sed 's/^/	/' "$f"; done
done_tests
