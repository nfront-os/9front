#!/bin/bash
# iso: the install/live CD (scripts/mkiso.sh: 9front's %.amd64.iso, %.386.iso
# recipe run in the guest) boots the way the release's does -- as a CD
# (9bootiso, El Torito; bootx64.efi from efiboot.fat) and as a disk or
# USB stick (hybrid MBR: mbr, pbs, 9boothyb; the ESP's efi/boot), each
# with BIOS and UEFI (OVMF) -- up to rio, taking the default at each of
# its prompts (tests/iso/drive.py). Builds the CD unless ISO names one.
. "$(dirname "$0")/../lib.sh"
# objtype=386: the 386 CD, booted on an x86-64 PC as well (BIOS and
# 64-bit UEFI both start 9pc; there is no 32-bit OVMF here)
for f in $objtype/9$KERNEL 386/9bootiso 386/efiboot.fat; do
	[ -f "$ROOT/$f" ] || { skip "iso" "no $f, run $BUILDHOW and scripts/build-boot.sh"; done_tests; exit; }
done
OVMF_CODE=${OVMF_CODE:-/usr/share/edk2/x64/OVMF_CODE.4m.fd}

IMG=${ISO:-$TMPD/9front.$objtype.iso}
if [ -z "$ISO" ]; then
	check "scripts/mkiso.sh builds the CD" bash "$ROOT/scripts/mkiso.sh" "$IMG" || { done_tests; exit; }
fi
check "ISO 9660 with El Torito boot record" bash -c "file '$IMG' | grep -q 'ISO 9660.*bootable'"
check "the CD holds everything mk install installs (scripts/check-install.py)" \
	bash -c "python3 '$ROOT/scripts/check-install.py' -i '$IMG' > '$TMPD/check-install' || { grep -v '^ok' '$TMPD/check-install' | head; false; }"
check "hybrid MBR: active ESP partition" python3 -c "
import sys
m = open('$IMG', 'rb').read(512)
e = m[446:462]
sys.exit(not (m[510:512] == b'\x55\xaa' and e[0] == 0x80 and e[4] == 0xef))"

for media in cd disk; do
	for fw in bios uefi; do
		if [ $fw = uefi ] && [ ! -f "$OVMF_CODE" ]; then
			skip "$media, $fw" "no OVMF ($OVMF_CODE)"
			continue
		fi
		dev=/dev/sdD0/data
		[ $media = disk ] && dev=/dev/sdC0/data
		check "boot as $media with $fw up to rio, root $dev" bash -c \
			"OVMF_CODE='$OVMF_CODE' python3 '$ROOT/tests/iso/drive.py' '$IMG' $media $fw '$TMPD/$media-$fw' | grep -q '^$dev\$'"
	done
done
done_tests
