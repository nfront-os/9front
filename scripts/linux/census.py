#!/usr/bin/env python3
"""census.py [-v] [file...]: how far the Linux code compiles against our layer.

Compiles (syntax only) each file kbuild.py chooses from sys/src/linux/
vendor with the layer's headers (replace/include, kpi/include) and
counts: files that compile, and for the others the first missing
header, or the first error.  The measure of progress of the LinuxKPI
for 9front: when it says 0 failing, everything compiles.  -v prints
each failing file's first error."""
import os, re, subprocess, sys
from concurrent.futures import ThreadPoolExecutor

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import kbuild

D = os.path.normpath(os.path.join(HERE, '../../sys/src/linux'))
V = os.path.join(D, 'vendor')

def build():
	"""what sys/src/linux/BUILD names: (dir|file, path)"""
	out = []
	for l in open(os.path.join(D, 'BUILD')):
		if l.strip() and not l.startswith('#'):
			k, p = l.split()
			out.append((k, p))
	return out

def flags(c):
	d = os.path.dirname(c)
	inc = ['replace/include', 'kpi/include', 'kpi/arch/x86/include', 'vendor/include', 'vendor/include/uapi', 'vendor']
	f = ['-std=gnu11', '-nostdinc', '-fsyntax-only', '-D__KERNEL__', '-DKBUILD_MODNAME="%s"' % os.path.basename(c)[:-2],
		'-include', os.path.join(D, 'kpi/include/linux/kconfig.h'),
		'-include', os.path.join(D, 'kpi/include/linux/compiler_types.h'),
		'-Wno-unused', '-Wno-pointer-sign', '-fmax-errors=20', '-fno-stack-protector']
	for i in inc:
		f += ['-I', os.path.join(D, i)]
	# subdir-ccflags: i915's own directory, as Kbuild has it
	if '/i915' in c:
		f += ['-I', os.path.join(V, 'drivers/gpu/drm/i915'), '-I', os.path.join(D, 'replace/drivers/gpu/drm/i915')]
		# our headers of i915's selftests, for their .c files #included from elsewhere
		f += ['-I', os.path.join(D, 'replace/drivers/gpu/drm/i915/selftests')]
	f += ['-I', os.path.join(V, d)]
	# quoted includes next to the file, replaced: replace/ mirrors vendor/
	f += ['-I', os.path.join(D, 'replace', d)]
	return f

def omitted():
	"""the files OMIT leaves out of the build"""
	out = set()
	for l in open(os.path.join(D, 'OMIT')):
		if l.strip() and not l.startswith('#'):
			out.add(l.split()[0])
	return out

def source(c):
	"""our replacement of a file (replace/), else Linux's (vendor/)"""
	r = os.path.join(D, 'replace', c)
	return r if os.path.exists(r) else os.path.join(V, c)

def compile1(c):
	if not os.path.exists(source(c)):
		return c, 'to write: replace/' + c
	r = subprocess.run(['gcc'] + flags(c) + [source(c)], capture_output=True, text=True)
	if r.returncode == 0:
		return c, None
	m = re.search(r'fatal error: ([^:]+): (No such file|Datei oder Verzeichnis)', r.stderr)
	if m:
		return c, 'missing ' + m.group(1)
	m = re.search(r'(error|Fehler): (.*)', r.stderr)
	return c, 'error ' + (m.group(2) if m else r.stderr.strip().split('\n')[-1])

def main():
	verbose = '-v' in sys.argv
	cfg = kbuild.config()
	files = [a for a in sys.argv[1:] if a != '-v']
	if not files:
		files = []
		for kind, path in build():
			files += kbuild.objects(V, path, cfg) if kind == 'dir' else [path]
		files = [f for f in files if f not in omitted()]
	with ThreadPoolExecutor(os.cpu_count()) as ex:
		res = list(ex.map(compile1, files))
	bad = [(c, e) for c, e in res if e]
	count = {}
	for c, e in bad:
		count[e] = count.get(e, 0) + 1
		if verbose:
			print('%s: %s' % (c, e))
	for e, n in sorted(count.items(), key=lambda x: -x[1])[:40]:
		print('%5d  %s' % (n, e))
	print('census: %d files, %d compile, %d fail' % (len(res), len(res) - len(bad), len(bad)))

main()
