#!/usr/bin/env python3
"""
The Rust crates of the tree: each directory of sys/src/cmd with a rust.json
({"Programs": [...]}) is a crate whose programs scripts/build/cargo.sh builds
and installs in /$objtype/bin. scripts/gen/genbuild.py makes the build step from
them, scripts/build/check-install.py expects their programs.

usage: scripts/gen/rustcrates.py	lists them: dir program...
"""
import json, os, sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.insert(0, os.path.join(ROOT, 'scripts', 'lib'))
import paths	# GCC_DIR, RUST_DIR, ... (the distribution's paths.env)

# the architectures cargo.sh builds for (its sysroot view: scripts/mesa/common.sh)
OBJTYPES = ['amd64', '386', 'arm64']

def crates():
	"""[(dir from the root, [program, ...]), ...]"""
	out = []
	top = os.path.join(ROOT, 'sys/src/cmd')
	for dp, dn, fn in os.walk(top):
		dn[:] = sorted(d for d in dn if d not in ('target', 'vendor', '.git'))
		if 'rust.json' in fn and 'Cargo.toml' in fn:
			progs = json.load(open(os.path.join(dp, 'rust.json')))['Programs']
			out.append((os.path.relpath(dp, ROOT), progs))
	return out

def rustc():
	"""is there a Rust toolchain for cargo.sh (the stage-1 rustc of 9front-rust, RUST_DIR)?"""
	r = os.environ.get('RUSTC') or os.path.join(paths.get('RUST_DIR'), 'rust/build/host/stage1/bin/rustc')
	return os.access(r, os.X_OK)

if __name__ == '__main__':
	for d, p in crates():
		print(d, ' '.join(p))
