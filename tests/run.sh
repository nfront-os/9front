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
#
# usage: tests/run.sh [category...]     (default: all, in the order above)
cd "$(dirname "$0")"
cats=${*:-static asm syscall build guest boot gui disk iso}
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
