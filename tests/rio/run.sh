#!/bin/bash
# rio: mouse and keyboard in rio on the install CD (tests/rio/rio.py):
# select in a window, type, New + sweep, type into the new window.
# Uses ISO, else the last CD scripts/guest/mkiso.sh made.
. "$(dirname "$0")/../lib.sh"
IMG=${ISO:-$CLI_DIR/9front.$objtype.iso}
[ -f "$IMG" ] || { skip "rio" "no CD $IMG, run objtype=$objtype scripts/guest/mkiso.sh or set ISO"; done_tests; exit; }

python3 "$ROOT/tests/rio/rio.py" "$IMG" "$TMPD/vm" $objtype > "$TMPD/results" 2>&1
while read -r r name; do
	case $r in
	ok)	pass "$name" ;;
	fail)	fail "$name" ;;
	*)	fail "rio.py: $r $name" ;;
	esac
done < "$TMPD/results"
grep -q . "$TMPD/results" || fail "rio.py printed nothing"
done_tests
