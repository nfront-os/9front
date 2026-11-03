#!/usr/bin/env python3
"""
Go on the install CD (/$objtype/go, from ../9front-go via
scripts/guest/boot/goproto, GO_OUT for mkiso.sh): boot to rio, then over the
serial rc: go version, go finds its tree (GOROOT) with no $GOROOT, go run and
go build of a program that uses goroutines (GOCACHE in /tmp: the live CD's
$home is read-only), and an ELF file of Go's test data is unchanged (stage-dist.sh strips ELF files, not in /$objtype/go).

usage: tests/go/go.py iso workdir objtype [sha1-of-testdata]
   prints one line per check: ok|fail|skip <name>
"""
import os, sys, time
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
from vm import VM

iso, work, objtype = sys.argv[1:4]
want = sys.argv[4] if len(sys.argv) > 4 else ''
TESTDATA = '/%s/go/src/debug/elf/testdata/go-relocation-test-gcc441-x86-64.obj' % objtype
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

try:
	if not result(vm.boot(), 'boot to rio, serial rc'):
		sys.exit(0)
	out = run('test -x /%s/go/bin/go && echo has-go' % objtype, 'has-done', 10)
	if 'has-go\n' not in out:
		print('skip no /%s/go on the CD (GO_OUT for mkiso.sh)' % objtype, flush=True)
		sys.exit(0)
	# the live CD's $home is read-only: the build cache in /tmp (a ramfs)
	vm.serial('bind -b /%s/go/bin /bin; cd /tmp; GOCACHE=/tmp/go-build' % objtype, 2)
	out = run('go version', 'version-done', 60)
	result(('plan9/%s' % objtype) in out, 'go version')
	out = run('go env GOROOT', 'goroot-done', 60)
	result(('/%s/go\ngoroot-done' % objtype) in out, 'GOROOT without $GOROOT')

	vm.serial("echo 'package main' > hello.go", 1)
	vm.serial("echo 'import (\"fmt\"; \"runtime\")' >> hello.go", 1)
	vm.serial("echo 'func main(){ c := make(chan int); go func(){ c <- 42 }(); fmt.Println(\"hello from go\", <-c, runtime.GOARCH) }' >> hello.go", 1)
	out = run('go run hello.go', 'run-done', 900)	# std compiled first
	result(('hello from go 42 %s' % objtype) in out, 'go run (std compiled on the CD)')
	out = run('go build -o hello hello.go && ./hello', 'build-done', 300)
	result(('hello from go 42 %s' % objtype) in out.replace('go build', ''), 'go build, the program runs')

	if want:
		out = run('sha1sum %s' % TESTDATA, 'sha-done', 30)
		result(want in out, "Go's ELF test data unchanged (not stripped)")
finally:
	vm.close()
