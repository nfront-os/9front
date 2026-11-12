#!/bin/bash
# The crates.io crates of a Rust crate of the tree into its vendor/ (cargo
# vendor), for a build without network (scripts/build/cargo.sh: --offline).
# Only those 9front's targets build keep their sources: cargo vendor takes
# every platform's (Windows, Wayland, Android, ...: hundreds of MB); the
# others become stubs, their manifest and an empty lib.rs (cargo reads every
# package of Cargo.lock, builds only those of the target). Crates the
# manifest patches to a path (sys/src/rust/crates) are not vendored.
# usage: scripts/rust/vendor.sh [dir]   (dir: a crate or workspace with Cargo.toml;
#        default sys/src/rust, the tree's workspace)
# env: RUSTC (../9front-rust's stage-1 rustc: it knows the plan9 target)
set -e
ROOT=$(git -C "$(dirname "${BASH_SOURCE[0]}")" rev-parse --show-toplevel)
. "$ROOT/scripts/lib/paths.sh"	# GCC_DIR, RUST_DIR, GO_DIR, MCP_DIR, DISTFILES (the distribution's paths.env)
[ $# -le 1 ] || { echo "usage: scripts/rust/vendor.sh [dir]" >&2; exit 1; }
set -- "${1:-$ROOT/sys/src/rust}"
RUSTC=${RUSTC:-$RUST_DIR/rust/build/host/stage1/bin/rustc}
# the targets scripts/build/cargo.sh builds for (amd64, 386, arm64)
TARGETS=${TARGETS:-x86_64-unknown-plan9 i686-unknown-plan9 aarch64-unknown-plan9}
cd "$1"
rm -rf vendor
mkdir -p .cargo
# cargo vendor says how to find them (crates.io's and git's): the config (on
# its standard output; not with --quiet, which leaves it out)
{ echo '# the crates come from vendor/ (scripts/rust/vendor.sh), the build needs no network'
  cargo vendor --versioned-dirs vendor 2>/dev/null; } > .cargo/config.toml.new
grep -q '^replace-with' .cargo/config.toml.new || { echo "vendor.sh: cargo vendor gave no config" >&2; exit 1; }
mv .cargo/config.toml.new .cargo/config.toml
# the packages the targets build: name-version
keep=$(for T in $TARGETS; do
	RUSTC=$RUSTC cargo tree --offline --target "$T" -e normal,build --prefix none --format '{p}'
done | sed -n 's/^\([^ ]*\) v\([^ ]*\).*/\1-\2/p' | sort -u)
n=0; s=0
for d in vendor/*/; do
	c=$(basename "$d")
	if echo "$keep" | grep -qx "$c"; then
		# its tests, benchmarks and examples are not built: out, with their
		# targets and checksums
		python3 - "$d" <<'PY'
import json, os, re, shutil, sys
d = sys.argv[1]
gone = [x for x in ('tests', 'benches', 'examples', 'test-macros') if os.path.isdir(os.path.join(d, x))]
for x in gone:
    shutil.rmtree(os.path.join(d, x))
p = os.path.join(d, 'Cargo.toml'); s = open(p).read()
s = re.sub(r'(?ms)^\[\[(bin|example|test|bench)\]\].*?(?=^\[|\Z)', lambda m: m.group(0) if m.group(1) == 'bin' else '', s)
s = re.sub(r'(?m)^(autotests|autobenches|autoexamples) = .*\n', '', s)
s = s.replace('[package]\n', '[package]\nautotests = false\nautobenches = false\nautoexamples = false\n', 1)
open(p, 'w').write(s)
c = os.path.join(d, '.cargo-checksum.json'); j = json.load(open(c))
j['files'] = {f: h for f, h in j['files'].items() if f.split('/')[0] not in gone and f != 'Cargo.toml'}
json.dump(j, open(c, 'w'))
PY
		n=$((n + 1))
		continue
	fi
	# a stub: the manifest, its checksum without files, an empty library
	pkg=$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["package"])' "$d/.cargo-checksum.json")
	find "$d" -mindepth 1 -maxdepth 1 ! -name Cargo.toml -exec rm -rf {} +
	python3 - "$d/Cargo.toml" <<'PY'
import re, sys
# no build script, no other targets: an empty library is all cargo reads
p = sys.argv[1]; s = open(p).read()
s = re.sub(r'(?m)^(build|links) = .*\n', '', s)
s = re.sub(r'(?ms)^\[\[(bin|example|test|bench)\]\].*?(?=^\[|\Z)', '', s)
s = re.sub(r'(?ms)^\[lib\].*?(?=^\[|\Z)', '', s)
s += '\n[lib]\npath = "lib.rs"\n'
open(p, 'w').write(s)
PY
	: > "$d/lib.rs"
	printf '{"files":{},"package":"%s"}\n' "$pkg" > "$d/.cargo-checksum.json"
	s=$((s + 1))
done
echo "vendor.sh: $n crates for $TARGETS in $1/vendor, $s others as stubs ($(du -sh vendor | cut -f1))"
