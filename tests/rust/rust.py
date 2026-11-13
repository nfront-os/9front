#!/usr/bin/env python3
"""
Rust on the install CD (/$objtype/rust, from toolchain/rust via
scripts/guest/boot/rustproto; dist/bin/mkcd with -dev rust, or RUST_OUT for
mkiso.sh): boot to rio, then over the serial rc: rustc --version, rustc
builds a program with threads, a channel and files and it runs (rustc links
with the CD's gcc, /$objtype/gnu), cargo new / build / run / test offline
(CARGO_HOME in /tmp: the live CD's $home is read-only), rustfmt, rustdoc and
cargo clippy.

usage: tests/rust/rust.py iso workdir objtype
   prints one line per check: ok|fail|skip <name>
"""
import os, sys, time
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
from vm import VM

iso, work, objtype = sys.argv[1:4]
vm = VM(iso, work, objtype)
def result(ok, name):
	print('%s %s' % ('ok' if ok else 'fail', name), flush=True)
	return ok

def run(line, marker, secs):
	"""line on the serial rc, then echo marker; output until it shows"""
	out = vm.serial(line + "; echo %s $status" % marker, 1)
	end = time.time() + secs
	while marker not in out and time.time() < end:
		out += vm.read(2)
	return out

def lines(path, text):
	"""text into path on the guest, a line at a time (rc's quoting: ' doubled)"""
	op = '>'
	for l in text.split('\n'):
		vm.serial("echo '%s' %s %s" % (l.replace("'", "''"), op, path), 1)
		op = '>>'

HELLO = '''use std::sync::mpsc;
use std::thread;
fn main() {
    let (tx, rx) = mpsc::channel();
    let hs: Vec<_> = (1..=4).map(|i| { let tx = tx.clone(); thread::spawn(move || tx.send(i * 10).unwrap()) }).collect();
    for h in hs { h.join().unwrap(); }
    drop(tx);
    let sum: i32 = rx.iter().sum();
    std::fs::write("/tmp/rs.txt", format!("{}", sum)).unwrap();
    let back = std::fs::read_to_string("/tmp/rs.txt").unwrap();
    println!("hello from rust {} {}", back, std::env::consts::ARCH);
}'''

try:
	if not result(vm.boot(), 'boot to rio, serial rc'):
		sys.exit(0)
	out = run('test -x /%s/rust/bin/rustc && echo has-rust' % objtype, 'has-done', 10)
	if 'has-rust\n' not in out:
		print('skip no /%s/rust on the CD (dist/bin/mkcd -dev rust, or RUST_OUT for mkiso.sh)' % objtype, flush=True)
		sys.exit(0)
	vm.serial('bind -b /%s/gnu/bin /bin; bind -b /%s/rust/bin /bin; cd /tmp; CARGO_HOME=/tmp/cargo' % (objtype, objtype), 2)
	arch = {'amd64': 'x86_64', '386': 'x86', 'arm64': 'aarch64'}[objtype]
	out = run('rustc --version --verbose', 'version-done', 60)
	result('host: %s-unknown-plan9' % ('i686' if objtype == '386' else arch) in out, 'rustc --version')

	lines('hello.rs', HELLO)
	out = run('rustc --edition 2024 -O hello.rs && ./hello', 'rustc-done', 600)
	result(('hello from rust 100 %s' % arch) in out, 'rustc: threads, channel, files; the program runs')

	out = run('cargo new --vcs none --bin hi && cd hi && cargo build --offline', 'cbuild-done', 600)
	result('cbuild-done \n' in out.replace('\r', '') or 'cbuild-done\n' in out.replace('\r', ''), 'cargo new, cargo build (offline)')
	out = run('cargo run --offline', 'crun-done', 300)
	result('Hello, world!' in out, 'cargo run')
	lines('src/lib.rs', 'pub fn add(a: i32, b: i32) -> i32 { a + b }\n#[cfg(test)]\nmod t { #[test] fn adds() { assert_eq!(super::add(40, 2), 42); } }')
	out = run('cargo test --offline --lib', 'ctest-done', 600)
	result('test t::adds ... ok' in out and 'test result: ok' in out, 'cargo test')

	vm.serial('cd /tmp', 1)
	lines('fmt.rs', 'fn main(){let x=1;println!("{}",x);}')
	out = run('rustfmt --edition 2024 fmt.rs && cat fmt.rs', 'fmt-done', 120)
	result('    let x = 1;' in out, 'rustfmt formats a file')
	lines('doc.rs', '//! A crate.\n/// Adds two numbers.\npub fn add(a: i32, b: i32) -> i32 { a + b }')
	out = run('rustdoc --edition 2024 --crate-type lib doc.rs && test -f doc/doc/fn.add.html && echo has-doc', 'doc-done', 300)
	result('has-doc\n' in out.replace('\r', ''), 'rustdoc writes the docs')
	vm.serial('cd /tmp/hi', 1)
	lines('src/main.rs', 'fn main() { let v = vec![1, 2, 3]; if v.len() == 0 { println!("empty"); } }')
	out = run('cargo clippy --offline', 'clippy-done', 600)
	result('len_zero' in out or 'is_empty' in out, 'cargo clippy finds a lint')
finally:
	vm.close()
