#!/bin/bash
# disk: the install image (scripts/guest/mkdisk.sh: 9front's %.amd64.qcow2
# recipe run in the guest, hjfs root, 9front's boot code mbr, pbs,
# 9bootfat) boots on its own -- no host root -- and serves the
# installed system. Builds the image (minutes)
# unless DISKIMG names one.
. "$(dirname "$0")/../lib.sh"
# 9front makes disk images for amd64 only (sys/lib/dist/mkfile: %.amd64.qcow2)
case $objtype in
amd64)	[ -f "$ROOT/amd64/bin/9pc64" ] || { skip "disk" "no kernel, run $BUILDHOW"; done_tests; exit; } ;;
# as 9front's arm64 image: u-boot, boot.scr on a FAT partition (scripts/guest/build-boot.sh)
riscv64)	[ -f "$ROOT/riscv64/9virt.u" ] || { skip "disk" "no riscv64/9virt.u, run scripts/guest/build-boot.sh"; done_tests; exit; } ;;
*)	skip "disk" "objtype=$objtype: no $objtype disk image"; done_tests; exit ;;
esac

IMG=${DISKIMG:-$TMPD/9front.$objtype.img}
if [ -z "$DISKIMG" ]; then
	check "scripts/guest/mkdisk.sh builds the image" bash "$ROOT/scripts/guest/mkdisk.sh" "$IMG" || { done_tests; exit; }
	check "install: no kernel process shortage" bash -c "! grep -q 'no procs for kproc' '$CLI_DIR/mkdisk.log'"
fi

# installed by mkfile install rules beside the programs' $BIN (rc
# scripts, wrappers), elsewhere ($BIN/telnet: /bin/telnet), by other
# subsystems (/acme) or with generated sources (gs, blabs)
case $objtype in
amd64)	K=pc64 SD=sdC0 VMX=/bin/vmxgdb ;;
riscv64)	K=riscv64 SD=sdF0 VMX= ;;	# virtio disk; vmx is pc's
esac
INSTALLED="/bin/gs /bin/telnet /bin/passwd $VMX /bin/git/clone /bin/replica/pull
	/bin/upas/spam /bin/auth/status /bin/ape/psh /bin/aux/cropmarks /bin/games/blabs
	/acme/bin/$objtype/winfs /acme/bin/$objtype/News"
INSTALLED=$(echo $INSTALLED)
boot(){
	BOOTDISK=$IMG MEM=1024 "$ROOT/scripts/guest/boot-cli.sh" \
		'echo $bootargs' "ls -d /sys/src/9/$K /$objtype/bin/rc /usr/glenda/lib/profile" \
		'sleep 10; ps | grep -c closeproc' 'ls /srv | grep -c hjfs' \
		"echo lib \`{ls /$objtype/lib | wc -l} gcc \`{ls /$objtype/lib.gcc/libc.a}" \
		"ls $INSTALLED | wc -l" "echo '3 4 add = quit' | gs -q -dNODISPLAY -" \
		'git/init /tmp/gt && test -f /tmp/gt/.git/HEAD && echo git/init ok' \
		"ape/psh -c 'echo psh ok'" > "$TMPD/console" 2>&1
	tr -d '\r' < "$TMPD/console" > "$TMPD/log"
}
check "boot the image" boot || { done_tests; exit; }
L=$TMPD/log
# the line after the command (rc's prompt: "% " on a terminal, "sysname# " on a cpu server)
out(){ grep -A1 -F -e "% $1" -e "# $1" "$L" | tail -1; }

# 9bootfat prints plan9.ini, its load progress (a backspaced spinner)
# and "boot" when it jumps to the a.out kernel, which prints "Plan 9"
case $objtype in
amd64)	check "mbr -> pbs -> 9bootfat -> a.out kernel" bash -c "grep -a -B2 -m1 '^Plan 9\$' '$L' | grep -aq 'boot\$'" ;;
riscv64)	check "OpenSBI -> u-boot -> boot.scr -> 9virt.u" bash -c "grep -aq '^Found U-Boot script /boot.scr' '$L' && grep -aq 'Booting kernel from Legacy Image' '$L' && grep -aq '^Plan 9\$' '$L'" ;;
esac
check "kernel -> bootrc -> hjfs root -> rc" grep -q "^hjfs: fs is /dev/$SD/fs" "$L"
check "booted from local!/dev/$SD/fs" test "$(out 'echo $bootargs')" = "local!/dev/$SD/fs"
check "system installed (sources, binaries, home)" bash -c "grep -A3 -F -e '% ls -d /sys/src' -e '# ls -d /sys/src' '$L' | grep -c '^/' | grep -qx 3"
check "/$objtype/lib empty as in 9front, gcc archives in /$objtype/lib.gcc" \
	test "$(out 'echo lib')" = "lib 1 gcc /$objtype/lib.gcc/libc.a"
# closeprocs come and go while rio starts; count them once it is idle
check "at most a few closeprocs" test "$(out 'sleep 10; ps | grep -c closeproc' | tr -d ' ')" -le 4
check "all of mk install: scripts, acme, gs, blabs ($(echo $INSTALLED | wc -w) samples)" \
	test "$(out "ls $INSTALLED" | tr -d ' ')" = "$(echo $INSTALLED | wc -w)"
check "gs runs (3 4 add = 7)" test "$(out "echo '3 4 add")" = 7
check "git/init (an rc script of git's install rule)" test "$(out 'git/init /tmp/gt')" = 'git/init ok'
check "ape/psh" test "$(out "ape/psh -c")" = 'psh ok'
check "no kernel panic" bash -c "! grep -q '^panic:' '$L'"
done_tests
