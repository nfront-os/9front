#!/bin/bash
# Run the gcc port's tests by category.
#
#   static   source checks, no build needed
#   asm      host unit tests of the GAS ports of libc's amd64 assembly
#   syscall  generated system call stubs (scripts/build/gen-9syscall.sh)
#   build    results of the build tool (libraries, commands, kernel)
#   guest    libc unit tests built with the port toolchain, run in 9front
#            under QEMU (needs a build; boots once)
#   boot     CLI boot smoke test via scripts/guest/boot-cli.sh (boots once)
#   gui      terminal boot: VESA via aux/realemu, rio (boots once;
#            GUI_SCREENSHOT=file.png keeps the screen dump)
#   disk     install image with hjfs (scripts/guest/mkdisk.sh, minutes), booted
#            on its own (DISKIMG=image skips building it)
#   iso      install/live CD (scripts/guest/mkiso.sh): booted as CD and as disk
#            (hybrid), each with BIOS and UEFI, up to rio (ISO=image skips
#            building it)
#   rio      mouse and keyboard in rio on the CD: select, type, New + sweep
#            (ISO=image, else the last CD made by scripts/guest/mkiso.sh)
#   usb      USB stick on xHCI, EHCI, UHCI: root hub, /dev/sdU*,
#            sector 0 (same CD; USB_CTLRS="xhci ehci" for fewer)
#   gcc      the native gcc on the CD (/$objtype/gnu, ../9front-gcc): a POSIX
#            and a native Plan 9 program compiled, linked and run (same CD)
#   go       Go on the CD (/$objtype/go, ../9front-go, GO_OUT for mkiso.sh): go
#            version, GOROOT, go run, go build (same CD; skipped without Go)
#   hw       the hardware test stick (tests/hw) in QEMU: boots from USB,
#            runs the configurations with resets, HANG after a power-off
#            (makes the stick image, minutes)
#
# usage: tests/run.sh [category...]     (default: all, in the order above)
cd "$(dirname "$0")"
cats=${*:-static asm syscall build guest boot gui disk iso rio usb gcc go hw}
failed=()
for c in $cats; do
	[ -x "$c/run.sh" ] || { echo "unknown category $c"; exit 2; }
	echo "== $c"
	./"$c"/run.sh || failed+=("$c")
done
echo
if [ ${#failed[@]} -eq 0 ]; then
	echo "all categories passed: $cats"
else
	echo "failed categories: ${failed[*]}"
	exit 1
fi
