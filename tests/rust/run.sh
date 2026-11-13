#!/bin/bash
# rust: Rust on the install CD (/$objtype/rust, from toolchain/rust via
# scripts/guest/boot/rustproto; dist/bin/mkcd -dev rust, or RUST_OUT for mkiso.sh):
# rustc --version, a program with threads and files built by rustc, cargo new,
# build, run and test offline, rustfmt, rustdoc, cargo clippy (tests/rust/rust.py).
# Skips a CD without Rust. Uses ISO, else the last CD scripts/guest/mkiso.sh made.
. "$(dirname "$0")/../lib.sh"
IMG=${ISO:-$CLI_DIR/9front.$objtype.iso}
[ -f "$IMG" ] || { skip "rust" "no CD $IMG, run dist/bin/mkcd -dev rust ... or set ISO"; done_tests; exit; }

python3 "$ROOT/tests/rust/rust.py" "$IMG" "$TMPD/vm" $objtype > "$TMPD/results" 2>&1
while read -r r name; do
	case $r in
	ok)	pass "$name" ;;
	fail)	fail "$name" ;;
	skip)	skip "rust" "$name" ;;
	*)	fail "rust.py: $r $name" ;;
	esac
done < "$TMPD/results"
grep -q . "$TMPD/results" || fail "rust.py printed nothing"
done_tests
