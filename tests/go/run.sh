#!/bin/bash
# go: Go on the install CD (/$objtype/go, from ../9front-go via
# scripts/guest/boot/goproto, GO_OUT=../9front-go/out/$objtype for mkiso.sh):
# go version, GOROOT, go run and go build, Go's ELF test data not stripped
# (tests/go/go.py). Skips a CD without Go.
# Uses ISO, else the last CD scripts/guest/mkiso.sh made.
. "$(dirname "$0")/../lib.sh"
IMG=${ISO:-$CLI_DIR/9front.$objtype.iso}
[ -f "$IMG" ] || { skip "go" "no CD $IMG, run GO_OUT=... objtype=$objtype scripts/guest/mkiso.sh or set ISO"; done_tests; exit; }
# the test data's checksum in 9front-go's tree, if there (the CD's must be the same)
F=${GO_OUT:-$ROOT/../9front-go/out/$objtype}/$objtype/go/src/debug/elf/testdata/go-relocation-test-gcc441-x86-64.obj
sha=$( [ -f "$F" ] && sha1sum "$F" | cut -d' ' -f1 )

python3 "$ROOT/tests/go/go.py" "$IMG" "$TMPD/vm" $objtype $sha > "$TMPD/results" 2>&1
while read -r r name; do
	case $r in
	ok)	pass "$name" ;;
	fail)	fail "$name" ;;
	skip)	skip "go" "$name" ;;
	*)	fail "go.py: $r $name" ;;
	esac
done < "$TMPD/results"
grep -q . "$TMPD/results" || fail "go.py printed nothing"
done_tests
