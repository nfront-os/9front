#!/bin/bash
# The Rust crates 9front changed, into the tree (sys/src/rust), so that the
# tree builds them offline, also on 9front itself: a copy of ../9front-rust's
# crates/<crate> (crates.io's release with its patches/crates/<crate>.patch,
# scripts/crates-patch.sh apply there) for each crate sys/src/rust/CRATES
# names, and of its plan9/p9win (ours).  Same layout as there:
# sys/src/rust/crates/<crate>, sys/src/rust/plan9/p9win.  The master is
# ../9front-rust; change the crates there, then run this.
# tests/static compares the copy with ../9front-rust.
# usage: scripts/rust/crates.sh [-n]   (-n: only say what differs)
# env: RUST_DIR (../9front-rust)
set -e
ROOT=$(git -C "$(dirname "${BASH_SOURCE[0]}")" rev-parse --show-toplevel)
. "$ROOT/scripts/lib/paths.sh"	# GCC_DIR, RUST_DIR, GO_DIR, MCP_DIR, DISTFILES (the distribution's paths.env)
D=$ROOT/sys/src/rust
check=
[ "$1" = -n ] && check=1
ex=(--exclude=target --exclude=.cargo-ok --exclude=Cargo.lock)
fail=0
copy() { # from to
	[ -d "$1" ] || { echo "crates.sh: no $1 (../9front-rust: scripts/crates-patch.sh apply)" >&2; exit 1; }
	if [ -n "$check" ]; then
		diff -rq "${ex[@]}" "$1" "$2" >/dev/null 2>&1 || { echo "differs: ${2#$ROOT/}"; fail=1; }
		return
	fi
	mkdir -p "$2"
	rsync -a --delete "${ex[@]}" "$1/" "$2/"
}
for c in $(grep -v '^#' "$D/CRATES"); do
	copy "$RUST_DIR/crates/$c" "$D/crates/$c"
done
copy "$RUST_DIR/plan9/p9win" "$D/plan9/p9win"
# crates no longer named
for d in "$D"/crates/*/; do
	[ -d "$d" ] || continue
	c=$(basename "$d")
	grep -qx "$c" "$D/CRATES" && continue
	if [ -n "$check" ]; then echo "not in CRATES: sys/src/rust/crates/$c"; fail=1; else rm -rf "${d:?}"; fi
done
[ -n "$check" ] && exit $fail
echo "crates.sh: $(grep -vc '^#' "$D/CRATES") crates and p9win from $RUST_DIR in sys/src/rust"
