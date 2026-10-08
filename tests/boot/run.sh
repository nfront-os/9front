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
done_tests
