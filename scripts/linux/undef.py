#!/usr/bin/env python3
"""undef.py [OUTDIR]: what the LinuxKPI still has to provide.

Compiles everything census.py compiles (plus kpi/linux/*.c), links it
into one relocatable object and lists the symbols nothing defines: the
functions and data kpi/ must implement before aux/i915 links."""
import os, subprocess, sys, glob
from concurrent.futures import ThreadPoolExecutor
HERE = os.path.dirname(os.path.abspath(__file__))
src = open(os.path.join(HERE, 'census.py')).read().replace('main()\n', '')
g = {'__name__': 'x', '__file__': os.path.join(HERE, 'census.py')}
exec(src, g)
D = g['D']
O = sys.argv[1] if len(sys.argv) > 1 else '/tmp/kpi-undef'
os.makedirs(O, exist_ok=True)
cfg = g['kbuild'].config()
files = []
for kind, path in g['build']():
	files += g['kbuild'].objects(g['V'], path, cfg) if kind == 'dir' else [path]
files = [f for f in files if f not in g['omitted']()]
def cc(c):
	srcf = g['source'](c)
	fl = [x for x in g['flags'](c) if x != '-fsyntax-only'] + ['-c', '-O1', '-fno-pie', '-o', os.path.join(O, c.replace('/', '_') + '.o')]
	r = subprocess.run(['gcc'] + fl + [srcf], capture_output=True, text=True)
	return c, r.returncode, r.stderr
kpi = sorted(os.path.relpath(f, D) for f in glob.glob(os.path.join(D, 'kpi/linux/*.c')))
def cck(c):
	fl = [x for x in g['flags']('kpi/linux/x.c') if x != '-fsyntax-only'] + ['-c', '-O1', '-fno-pie', '-o', os.path.join(O, c.replace('/', '_') + '.o')]
	r = subprocess.run(['gcc'] + fl + [os.path.join(D, c)], capture_output=True, text=True)
	return c, r.returncode, r.stderr
p9 = sorted(os.path.relpath(f, D) for f in glob.glob(os.path.join(D, 'kpi/plan9/*.c')))
TREE = os.path.normpath(os.path.join(D, '../../..'))
def ccp(c):
	fl = ['-std=gnu17', '-ffreestanding', '-nostdinc', '-fplan9-extensions', '-D__plan9__', '-fno-stack-protector',
		'-Wno-unknown-pragmas', '-I', os.path.join(TREE, 'amd64/include'), '-I', os.path.join(TREE, 'sys/include'),
		'-c', '-O1', '-fno-pie', '-o', os.path.join(O, c.replace('/', '_') + '.o')]
	r = subprocess.run(['gcc'] + fl + [os.path.join(D, c)], capture_output=True, text=True)
	return c, r.returncode, r.stderr
with ThreadPoolExecutor(os.cpu_count()) as ex:
	res = list(ex.map(cc, files)) + list(ex.map(cck, kpi)) + list(ex.map(ccp, p9))
bad = [(c, e) for c, r, e in res if r]
for c, e in bad:
	print('FAIL', c, [l for l in e.split('\n') if 'error' in l or 'Fehler' in l][:2])
objs = [os.path.join(O, c.replace('/', '_') + '.o') for c, r, e in res if not r]
subprocess.run(['ld', '-r', '-o', os.path.join(O, 'all.o')] + objs, check=True)
nm = subprocess.run(['nm', '-u', os.path.join(O, 'all.o')], capture_output=True, text=True).stdout
und = sorted(set(l.split()[-1] for l in nm.split('\n') if l.strip()))
for u in und:
	print(u)
print('undef: %d symbols (%d files, %d kpi, %d plan9)' % (len(und), len(files), len(kpi), len(p9)), file=sys.stderr)
