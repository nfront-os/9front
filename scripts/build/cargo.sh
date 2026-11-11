#!/bin/bash
# Build a Rust crate of the tree for 9front and install its programs in /$objtype/bin
# (build.exe's step rust, scripts/gen/genbuild.py from scripts/gen/rustcrates.py):
# cargo with ../9front-rust's stage-1 rustc (std for x86_64-, i686-, aarch64-unknown-plan9), linked by
# ../9front-gcc's cross gcc with this tree as its sysroot (libp9, APE: the view
# scripts/mesa/common.sh makes). Crates come from the crate's vendor/ if it has
# dependencies (no network). Without that rustc, or cargo, it says so and builds
# nothing (check-install.py reports the programs as norust). A program that comes out
# the same keeps its time (build.exe -D).
# usage: [objtype=amd64] scripts/build/cargo.sh [-n] dir prog...
#	-n	without the crate's default features (wsys' and ui's gpu: Vulkan,
#		Mesa's libvulkan.a, which only the arches of scripts/gen/apeprogs.py
#		have; genbuild.py gives it for the others)
# env: RUST_DIR (../9front-rust), RUSTC (its stage-1 rustc)
# The rc twin for the build on 9front: scripts/build/rc/cargo.rc.
set -e
feat=()
[ "$1" = -n ] && { feat=(--no-default-features); shift; }
[ $# -ge 2 ] || { echo "usage: scripts/build/cargo.sh [-n] dir prog..." >&2; exit 1; }
ROOT=$(git -C "$(dirname "${BASH_SOURCE[0]}")" rev-parse --show-toplevel)
case ${objtype:=amd64} in
amd64)	T=x86_64-unknown-plan9 ;;
386)	T=i686-unknown-plan9 ;;
arm64)	T=aarch64-unknown-plan9 ;;
*)	echo "cargo.sh: objtype=$objtype: no Rust target here; $* not built" >&2; exit 0 ;;
esac
RUST_DIR=${RUST_DIR:-$ROOT/../9front-rust}
RUSTC=${RUSTC:-$RUST_DIR/rust/build/host/stage1/bin/rustc}
if [ ! -x "$RUSTC" ] || ! command -v cargo >/dev/null; then
	echo "cargo.sh: no Rust toolchain (stage-1 rustc $RUSTC, cargo): $* not built" >&2
	exit 0
fi
. "$ROOT/scripts/mesa/common.sh"
dir=$(cd "$ROOT/$1" 2>/dev/null || cd "$1" && pwd); shift
name=${dir#$ROOT/}
V=$(echo "$T" | tr a-z- A-Z_)
# a member of a workspace (workspace = "..." in its Cargo.toml: sys/src/rust's)
# is built there, as its package: the workspace has the vendor/ and the patches
ws=$(sed -n 's/^workspace *= *"\(.*\)"/\1/p' "$dir/Cargo.toml")
pkg=()
if [ -n "$ws" ]; then
	pkg=(-p "$(sed -n 's/^name *= *"\(.*\)"/\1/p' "$dir/Cargo.toml" | head -1)")
	dir=$(cd "$dir/$ws" && pwd)
fi
# nor does it see a std built again by the same rustc (../9front-rust: a fix
# in std): what it built with another goes
. "$ROOT/scripts/build/cargostd.sh"
cargostd "$BUILD/cargo/${name//\//_}" $T
# cargo does not know the C libraries it links (libvulkan.a, APE's, lib9's):
# a program older than one of them goes (release/p and the deps/p-hash it is
# a link of: cargo looks at that one), so that cargo links it again
for p in "$@"; do
	rel=$BUILD/cargo/${name//\//_}/$T/release
	if [ -e "$rel/$p" ] && [ -n "$(find "$ROOT/$objtype/lib.gcc" -name '*.a' -newer "$rel/$p" -print -quit 2>/dev/null)" ]; then
		rm -f "$rel/$p" "$rel/deps/$p"-*
	fi
done
(cd "$dir" &&
 env RUSTC="$RUSTC" CARGO_TARGET_DIR="$BUILD/cargo/${name//\//_}" \
	"CARGO_TARGET_${V}_LINKER=$CROSS/$TRIPLE-gcc" \
	cargo build --offline --release --target $T --quiet "${pkg[@]}" "${feat[@]}")
mkdir -p "$ROOT/$objtype/bin"
for p in "$@"; do
	new=$BUILD/cargo/${name//\//_}/$T/release/$p
	cmp -s "$new" "$ROOT/$objtype/bin/$p" && continue
	cp "$new" "$ROOT/$objtype/bin/$p.new"
	mv "$ROOT/$objtype/bin/$p.new" "$ROOT/$objtype/bin/$p"
done
