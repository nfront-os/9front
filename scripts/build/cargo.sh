#!/bin/bash
# Build a Rust crate of the tree for 9front and install its programs in /$objtype/bin:
# cargo with ../9front-rust's stage-1 rustc (std for x86_64-/i686-unknown-plan9), linked
# by ../9front-gcc's cross gcc with this tree as its sysroot (libp9, APE: the view
# scripts/mesa/common.sh makes). Crates come from the crate's vendor/ if it has
# dependencies (no network).
# usage: [objtype=amd64|386] scripts/build/cargo.sh dir prog...
# env: RUST_DIR (../9front-rust), RUSTC (its stage-1 rustc)
# TODO: arm64 (no sysroot view for it yet), the rc twin for the build on 9front.
set -e
[ $# -ge 2 ] || { echo "usage: scripts/build/cargo.sh dir prog..." >&2; exit 1; }
. "$(dirname "$0")/../mesa/common.sh"
case $objtype in
amd64)	T=x86_64-unknown-plan9 ;;
386)	T=i686-unknown-plan9 ;;
esac
RUST_DIR=${RUST_DIR:-$ROOT/../9front-rust}
RUSTC=${RUSTC:-$RUST_DIR/rust/build/host/stage1/bin/rustc}
[ -x "$RUSTC" ] || { echo "cargo.sh: no stage-1 rustc $RUSTC (../9front-rust: ./x build --stage 1 library)" >&2; exit 1; }
dir=$(cd "$1" && pwd); shift
name=${dir#$ROOT/}
V=$(echo "$T" | tr a-z- A-Z_)
(cd "$dir" &&
 env RUSTC="$RUSTC" CARGO_TARGET_DIR="$BUILD/cargo/${name//\//_}" \
	"CARGO_TARGET_${V}_LINKER=$CROSS/$TRIPLE-gcc" \
	cargo build --offline --release --target $T)
mkdir -p "$ROOT/$objtype/bin"
for p in "$@"; do
	cp "$BUILD/cargo/${name//\//_}/$T/release/$p" "$ROOT/$objtype/bin/$p.new"
	mv "$ROOT/$objtype/bin/$p.new" "$ROOT/$objtype/bin/$p"
done
