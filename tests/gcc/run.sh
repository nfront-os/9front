#!/bin/bash
# gcc: the native gcc and binutils on the install CD (/$objtype/gnu, from
# ../9front-gcc via scripts/guest/boot/gnuproto) compile, link and run a
# POSIX and a native Plan 9 program (tests/gcc/gcc.py).
# Uses ISO, else the last CD scripts/guest/mkiso.sh made.
. "$(dirname "$0")/../lib.sh"
IMG=${ISO:-$CLI_DIR/9front.$objtype.iso}
[ -f "$IMG" ] || { skip "gcc" "no CD $IMG, run objtype=$objtype scripts/guest/mkiso.sh or set ISO"; done_tests; exit; }

python3 "$ROOT/tests/gcc/gcc.py" "$IMG" "$TMPD/vm" $objtype > "$TMPD/results" 2>&1
while read -r r name; do
	case $r in
	ok)	pass "$name" ;;
	fail)	fail "$name" ;;
	*)	fail "gcc.py: $r $name" ;;
	esac
done < "$TMPD/results"
grep -q . "$TMPD/results" || fail "gcc.py printed nothing"
done_tests
