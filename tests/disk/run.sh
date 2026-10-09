#!/bin/bash
# disk: the install image (scripts/mkdisk.sh: 9front's %.amd64.qcow2
# recipe run in the guest, hjfs root, 9front's boot code mbr, pbs,
# 9bootfat) boots on its own -- no host root -- and serves the
# installed system. Builds the image (minutes)
# unless DISKIMG names one.
. "$(dirname "$0")/../lib.sh"
[ -f "$ROOT/amd64/bin/9pc64" ] || { skip "disk" "no kernel, run scripts/build-cli.sh"; done_tests; exit; }

IMG=${DISKIMG:-$TMPD/9front.amd64.img}
if [ -z "$DISKIMG" ]; then
	check "scripts/mkdisk.sh builds the image" bash "$ROOT/scripts/mkdisk.sh" "$IMG" || { done_tests; exit; }
	check "install: no kernel process shortage" bash -c "! grep -q 'no procs for kproc' '$CLI_DIR/mkdisk.log'"
fi

boot(){
	BOOTDISK=$IMG MEM=1024 "$ROOT/scripts/boot-cli.sh" \
		'echo $bootargs' 'ls -d /sys/src/9/pc64 /amd64/bin/rc /usr/glenda/lib/profile' \
		'sleep 10; ps | grep -c closeproc' 'ls /srv | grep -c hjfs' \
		'echo lib `{ls /amd64/lib | wc -l} gcc `{ls /amd64/lib.gcc/libc.a}' > "$TMPD/console" 2>&1
	tr -d '\r' < "$TMPD/console" > "$TMPD/log"
}
check "boot the image" boot || { done_tests; exit; }
L=$TMPD/log
out(){ grep -A1 -F "% $1" "$L" | tail -1; }

# 9bootfat prints plan9.ini, its load progress (a backspaced spinner)
# and "boot" when it jumps to the a.out kernel, which prints "Plan 9"
check "mbr -> pbs -> 9bootfat -> a.out kernel" bash -c "grep -a -B2 -m1 '^Plan 9\$' '$L' | grep -aq 'boot\$'"
check "kernel -> bootrc -> hjfs root -> rc" grep -q '^hjfs: fs is /dev/sdC0/fs' "$L"
check "booted from local!/dev/sdC0/fs" test "$(out 'echo $bootargs')" = 'local!/dev/sdC0/fs'
check "system installed (sources, binaries, home)" bash -c "grep -A3 -F '% ls -d /sys/src' '$L' | grep -c '^/' | grep -qx 3"
check "/amd64/lib empty as in 9front, gcc archives in /amd64/lib.gcc" \
	test "$(out 'echo lib')" = 'lib 1 gcc /amd64/lib.gcc/libc.a'
# closeprocs come and go while rio starts; count them once it is idle
check "at most a few closeprocs" test "$(out 'sleep 10; ps | grep -c closeproc' | tr -d ' ')" -le 4
check "no kernel panic" bash -c "! grep -q '^panic:' '$L'"
done_tests
