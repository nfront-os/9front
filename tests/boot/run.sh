#!/bin/bash
# boot: CLI boot smoke test (scripts/boot-cli.sh, service=cpu): bootrc
# stand-in, init -c, cpurc, console rc
. "$(dirname "$0")/../lib.sh"
[ -f "$ROOT/amd64/bin/9pc64" ] || { skip "boot" "no kernel, run scripts/build-cli.sh"; done_tests; exit; }

boot(){
	"$ROOT/scripts/boot-cli.sh" \
		'echo hello' 'ls /bin | wc -l' 'date -i' 'ps | wc -l' 'cat /dev/sysname; echo' \
		'cat /proc/$pid/segment | wc -l' \
		"bind -a '#v' /dev; aux/realemu && aux/vga -m vesa -l 1024x768x16 && grep size /dev/vgactl" \
		> "$TMPD/console" 2>&1
	tr -d '\r' < "$TMPD/console" > "$TMPD/log"
}
check "boot to the console" boot || { done_tests; exit; }
L=$TMPD/log
# the command echo line is followed by its output
out(){ grep -A1 -F "# $1" "$L" | tail -1; }

check "cpurc finished, console rc prompt" grep -q '^init: starting /bin/rc' "$L"
# readnvram: QEMU has no nvram, factotum asks (scripts/boot-cli.sh answers)
check "cpurc: no errors besides the missing nvram" \
	bash -c "! sed -n '/root is/,/init: starting/p' '$L' | grep -iE 'error|fail|can.t|null list|suicide|panic' | grep -vE 'nvram|unknown device in # filename'"
check "echo" test "$(out 'echo hello')" = hello
check "ls /bin | wc -l > 100" test "$(out 'ls /bin | wc -l' | tr -d ' ')" -gt 100
check "date: RTC/timesync year >= 2025" bash -c "[ \$(echo '$(out 'date -i')' | cut -c1-4) -ge 2025 ]"
check "ps lists every process (> 20)" test "$(out 'ps | wc -l' | tr -d ' ')" -gt 20
check "sysname set (cs or cpurc)" test -n "$(out 'cat /dev/sysname; echo')"
# boot payload segments (userinit.c) must not be inherited across exec
check "rc has only stack, text, data, bss segments" test "$(out 'cat /proc/$pid/segment | wc -l' | tr -d ' ')" -eq 4
check "aux/realemu + aux/vga -m vesa: 1024x768x16" bash -c "grep -q '^size 1024x768x16' '$L'"
check "no kernel panic" bash -c "! grep -q '^panic:' '$L'"

# sd53c8xx with its microcode (sd53c8xx.i, aux/na) on QEMU's NCR 53C810
SCSI=$TMPD/scsi.img
truncate -s 64M "$SCSI"
printf 'SD53C8XX-MARKER' | dd of="$SCSI" conv=notrunc 2>/dev/null
scsi(){
	QEMU_EXTRA="-device lsi53c810,id=scsi0 -drive if=none,id=sd0,file=$SCSI,format=raw -device scsi-hd,bus=scsi0.0,drive=sd0" \
	MEM=1024 "$ROOT/scripts/boot-cli.sh" 'grep inquiry /dev/sd00/ctl' \
		'dd -if /dev/sd00/data -count 1 -quiet 1 | sed 1q' \
		'echo written-by-9front | dd -of /dev/sd00/data -oseek 8 -quiet 1' \
		'dd -if /dev/sd00/data -of /dev/null -count 4096 -bs 8192' > "$TMPD/scsi-console" 2>&1
	tr -d '\r' < "$TMPD/scsi-console" > "$TMPD/scsi"
}
check "boot with an NCR 53C810 SCSI disk" scsi || { done_tests; exit; }
L=$TMPD/scsi
check "sd53c8xx: controller found" grep -q '^sd53c8xx: NCR53C810' "$L"
check "sd53c8xx: inquiry" test "$(out 'grep inquiry')" = 'inquiry QEMU    QEMU HARDDISK   2.5+'
check "sd53c8xx: read" test "$(out 'dd -if /dev/sd00/data -count 1')" = SD53C8XX-MARKER
check "sd53c8xx: write (seen by the host)" bash -c "dd if='$SCSI' bs=512 skip=8 count=1 2>/dev/null | head -1 | grep -qx written-by-9front"
check "sd53c8xx: 32 MB read" bash -c "grep -q '^4096+0 records out' '$L'"
done_tests
