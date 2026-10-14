#!/bin/bash
# usb: a USB stick on xHCI, EHCI and UHCI (tests/usb/usb.py): the
# controller's root hub, the stick as /dev/sdU*, sector 0 read back.
# One boot of the install CD per controller (USB_CTLRS="xhci ehci" for
# fewer; USB_CTLRS=ohci fails in QEMU with 9front's own release too).
# Uses ISO, else the last CD scripts/guest/mkiso.sh made.
. "$(dirname "$0")/../lib.sh"
IMG=${ISO:-$CLI_DIR/9front.$objtype.iso}
[ -f "$IMG" ] || { skip "usb" "no CD $IMG, run objtype=$objtype scripts/guest/mkiso.sh or set ISO"; done_tests; exit; }

python3 "$ROOT/tests/usb/usb.py" "$IMG" "$TMPD/vm" $objtype $USB_CTLRS > "$TMPD/results" 2>&1
while read -r r name; do
	case $r in
	ok)	pass "$name" ;;
	fail)	fail "$name" ;;
	*)	fail "usb.py: $r $name" ;;
	esac
done < "$TMPD/results"
grep -q . "$TMPD/results" || fail "usb.py printed nothing"
done_tests
