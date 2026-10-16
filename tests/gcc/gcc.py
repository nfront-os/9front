#!/usr/bin/env python3
"""
The native gcc on the install CD (/$objtype/gnu, ../9front-gcc): boot to
rio, then over the serial rc: gcc runs, compiles and links a POSIX program
over APE, and a native Plan 9 program (/sys/src/cmd/echo.c with libc.a
from /$objtype/lib.gcc); both run; gccmk builds hoc from its mkfile.

usage: tests/gcc/gcc.py iso workdir objtype
   prints one line per check: ok|fail <name>
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

try:
	if not result(vm.boot(), 'boot to rio, serial rc'):
		sys.exit(0)
	vm.serial('bind -b /%s/gnu/bin /bin; cd /tmp' % objtype, 2)
	out = run('gcc --version', 'gcc-version-done', 60)
	result('16.2' in out, 'gcc --version')

	vm.serial("echo '#include <stdio.h>' > hello.c", 1)
	vm.serial("echo 'int main(void){ printf(\"hello from gcc\\n\"); return 0; }' >> hello.c", 1)
	out = run('gcc -O2 -o hello hello.c && ./hello', 'ape-done', 300)
	result('hello from gcc' in out, 'APE program: compile, link, run')

	p9 = ('gcc -O2 -nostdinc -I/%s/include -I/sys/include -fplan9-extensions -fno-builtin'
		' -c -o echo.o /sys/src/cmd/echo.c && gcc -nostdlib -o echo9 echo.o'
		' -L/%s/lib.gcc -lc -lgcc && ./echo9 native plan9 echo') % (objtype, objtype)
	out = run(p9, 'p9-done', 300)
	result('native plan9 echo' in out.replace(p9, ''), 'native Plan 9 program: compile, link, run')

	# 9front's mkfile with gcc (the CD's /sys/src is read-only: a copy)
	O = {'amd64': '6', '386': '8'}[objtype]
	mk = ("mkdir /tmp/hoc && dircp /sys/src/cmd/hoc /tmp/hoc && cd /tmp/hoc && gccmk >/tmp/hoc.log"
		" && echo '6*7' | ./%s.out | sed 's/^/hoc says /'") % O
	out = run(mk, 'mk-done', 600)
	result('hoc says 42' in out, 'gccmk: hoc from its mkfile, runs')
finally:
	vm.close()
