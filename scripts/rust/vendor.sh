#!/bin/bash
# The crates.io crates of a Rust crate of the tree into its vendor/ (cargo
# vendor), for a build without network (scripts/build/cargo.sh: --offline).
# Only those 9front's target builds keep their sources: cargo vendor takes
# every platform's (Windows, Wayland, Android, ...: hundreds of MB); the
# others become stubs, their manifest and an empty lib.rs (cargo reads every
# package of Cargo.lock, builds only those of the target). Crates the
# manifest patches to a path (sys/src/rust/crates) are not vendored.
# usage: scripts/rust/vendor.sh dir   (dir: a crate with Cargo.toml)
# env: RUSTC (../9front-rust's stage-1 rustc: it knows the plan9 target)
set -e
[ $# -eq 1 ] || { echo "usage: scripts/rust/vendor.sh dir" >&2; exit 1; }
ROOT=$(git -C "$(dirname "${BASH_SOURCE[0]}")" rev-parse --show-toplevel)
RUSTC=${RUSTC:-$ROOT/../9front-rust/rust/build/host/stage1/bin/rustc}
T=${T:-x86_64-unknown-plan9}
cd "$1"
rm -rf vendor
mkdir -p .cargo
cargo vendor --versioned-dirs --quiet vendor >/dev/null
cat > .cargo/config.toml <<'CONF'
# the crates come from vendor/ (scripts/rust/vendor.sh), the build needs no network
[source.crates-io]
replace-with = "vendored-sources"

[source.vendored-sources]
directory = "vendor"
CONF
# the packages the target builds: name-version
keep=$(RUSTC=$RUSTC cargo tree --offline --target "$T" -e normal,build --prefix none --format '{p}' |
	sed -n 's/^\([^ ]*\) v\([^ ]*\).*/\1-\2/p' | sort -u)
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
echo "vendor.sh: $n crates for $T in $1/vendor, $s others as stubs ($(du -sh vendor | cut -f1))"
