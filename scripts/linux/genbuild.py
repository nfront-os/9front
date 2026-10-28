#!/usr/bin/env python3
"""genbuild.py: the build.exe manifests of the LinuxKPI and its drivers.

  genbuild.py [-n]
	writes sys/src/linux/build.json (amd64): libkpi9.a (the Plan 9
	side, kpi/plan9), libkpi.a (the Linux side, kpi/linux) and
	liblinux.a (what BUILD names, from vendor/ or our replace/), one
	build object per directory (its own include path and KBUILD_MODNAME,
	as Kbuild has them); and sys/src/cmd/aux/gpu/i915/build.json.
	-n: only compare, exit 1 if a manifest differs (tests/static).

The files and flags are census.py's (kconfig, BUILD, OMIT), compiled
with -O2 as Linux is (BUILD_BUG_ON needs the optimiser)."""
import glob, json, os, sys

HERE = os.path.dirname(os.path.abspath(__file__))
src = open(os.path.join(HERE, 'census.py')).read().replace('main()\n', '')
g = {'__name__': 'x', '__file__': os.path.join(HERE, 'census.py')}
exec(src, g)
D = g['D']
TREE = os.path.normpath(os.path.join(D, '../../..'))

# as Linux compiles (Makefile: KBUILD_CFLAGS), without what is the kernel's own
LINUXFLAGS = ['-std=gnu11', '-O2', '-g', '-fno-strict-aliasing', '-fno-strict-overflow',
	'-fno-delete-null-pointer-checks', '-fno-common', '-fno-pie', '-fno-stack-protector',
	'-fno-omit-frame-pointer', '-mcmodel=small', '-fshort-wchar', '-funsigned-char',
	'-Wno-unused', '-Wno-pointer-sign', '-Wno-address-of-packed-member', '-Wno-format-truncation']

# Kbuild's module names, by directory
MODNAME = [('drivers/gpu/drm/i915', 'i915'), ('drivers/gpu/drm/display', 'drm_display_helper'),
	('drivers/gpu/drm/ttm', 'ttm'), ('drivers/gpu/drm', 'drm'), ('drivers/dma-buf', 'dma_buf'),
	('drivers/video', 'hdmi'), ('lib', 'scatterlist')]

def rel(p, base):
	return os.path.relpath(p, base)

def linuxflags(c, base, modname):
	"""census.py's flags for file c, for a manifest in base"""
	out = []
	fl = g['flags'](c)
	i = 0
	while i < len(fl):
		f = fl[i]
		if f in ('-fsyntax-only',) or f.startswith('-fmax-errors') or f.startswith('-std='):
			pass
		elif f.startswith('-DKBUILD_MODNAME='):
			out.append('-DKBUILD_MODNAME="%s"' % modname)
		elif f == '-I' and not os.path.isdir(fl[i + 1]):
			i += 1
		elif f in ('-I', '-include'):
			out.append(f + rel(fl[i + 1], base) if f == '-I' else f)
			if f == '-include':
				out.append(rel(fl[i + 1], base))
			i += 1
		else:
			out.append(f)
		i += 1
	return LINUXFLAGS + [x for x in out if x not in LINUXFLAGS]

def modname(d):
	for p, m in MODNAME:
		if d == p or d.startswith(p + '/'):
			return m
	return os.path.basename(d)

def linuxfiles():
	cfg = g['kbuild'].config()
	files = []
	for kind, path in g['build']():
		files += g['kbuild'].objects(g['V'], path, cfg) if kind == 'dir' else [path]
	om = g['omitted']()
	return [f for f in files if f not in om]

def libs():
	out = []
	out.append({
		'Name': 'linux-kpi9',
		'Include': ['/$ARCH/cflags.json'],
		'Install': '/$ARCH/lib.gcc/',
		'Library': 'libkpi9.a',
		'SourceFiles': sorted(rel(f, D) for f in glob.glob(os.path.join(D, 'kpi/plan9/*.c'))),
	})
	kpi = sorted(rel(f, D) for f in glob.glob(os.path.join(D, 'kpi/linux/*.c')))
	out.append({
		'Name': 'linux-kpi',
		'Noinc': True,
		'Cflags': linuxflags('kpi/linux/x.c', D, 'kpi'),
		'Install': '/$ARCH/lib.gcc/',
		'Library': 'libkpi.a',
		'SourceFiles': kpi,
	})
	bydir = {}
	for c in linuxfiles():
		bydir.setdefault(os.path.dirname(c), []).append(c)
	first = True
	for d in sorted(bydir):
		o = {
			'Name': 'linux-' + d.replace('/', '-'),
			'Noinc': True,
			'Cflags': linuxflags(bydir[d][0], D, modname(d)),
			'Install': '/$ARCH/lib.gcc/',
			'Library': 'liblinux.a',
			'SourceFiles': [rel(g['source'](c), D) for c in sorted(bydir[d])],
		}
		if first:
			o['Shared'] = True
		else:
			o['Append'] = True
		first = False
		out.append(o)
	return out

P9LIBS = ['lib9p.a', 'libthread.a', 'libc.a']

def i915():
	base = os.path.join(TREE, 'sys/src/cmd/aux/gpu/i915')
	return [{
		'Name': 'aux-i915-linux',
		'Noinc': True,
		'Cflags': linuxflags('drivers/gpu/drm/i915/x.c', base, 'i915'),
		'Install': '/$ARCH/lib.gcc/',
		'Library': 'libi915.a',
		'SourceFiles': sorted('linux/' + os.path.basename(f) for f in glob.glob(os.path.join(base, 'linux', '*.c'))),
	}, {
		'Name': 'aux-i915',
		'Include': ['/$ARCH/cflags.json'],
		'Cflags': ['-I', rel(os.path.join(D, 'kpi/include'), base)],
		'Install': '/$ARCH/bin/aux/',
		'Oflags': ['--entry=_main'],
		'Libs': ['--start-group'] + ['/$ARCH/lib.gcc/' + l for l in
			['libi915.a', 'liblinux.a', 'libkpi.a', 'libkpi9.a'] + P9LIBS + ['libgcc.a']] + ['--end-group'],
		'Program': 'i915',
		'SourceFiles': sorted(os.path.basename(f) for f in glob.glob(os.path.join(base, '*.c'))),
	}]

def dump(o):
	return json.dumps(o, indent='\t') + '\n'

def main():
	check = sys.argv[1:] == ['-n']
	bad = 0
	for path, o in ((os.path.join(D, 'build.json'), libs()),
			(os.path.join(TREE, 'sys/src/cmd/aux/gpu/i915/build.json'), i915())):
		s = dump(o)
		old = open(path).read() if os.path.exists(path) else None
		if check:
			if old != s:
				print('%s: differs from scripts/linux/genbuild.py' % rel(path, TREE))
				bad = 1
		elif old != s:
			open(path, 'w').write(s)
			print('wrote', rel(path, TREE))
	sys.exit(bad)

main()
