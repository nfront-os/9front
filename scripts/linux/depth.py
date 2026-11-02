#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""depth.py [TREE]: how much LinuxKPI work a Linux import still needs.

census.py stops at each file's first error; depth.py compiles (syntax
only) the same files, puts an empty stub for each missing header (in
$TMPDIR/depth-stub, first in the include path) round after round until
none is missing, then counts over all errors of all files: the distinct
missing functions/macros, types and other errors, each with the number
of files that hit it.  Register-like names (reg..., X__Y) come from
headers import.py rejected (no notice): files to OMIT, not KPI work.
Writes the counts as JSON next to the stubs."""
import os, re, shutil, subprocess, sys, collections
from concurrent.futures import ThreadPoolExecutor

TREE = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.path.dirname(os.path.abspath(__file__)), '../..'))
sys.path.insert(0, os.path.join(TREE, 'scripts/linux'))
src = open(os.path.join(TREE, 'scripts/linux/census.py')).read().replace('\nmain()\n', '\n')
g = {'__file__': os.path.join(TREE, 'scripts/linux/census.py'), '__name__': 'census'}
exec(compile(src, 'census.py', 'exec'), g)
STUB = os.path.join(os.environ.get('TMPDIR', '/tmp'), 'depth-stub')
shutil.rmtree(STUB, ignore_errors=True)
os.makedirs(STUB)
env = dict(os.environ, LC_ALL='C')

def files():
	cfg = g['kbuild'].config()
	out = []
	for kind, path in g['build']():
		out += g['kbuild'].objects(g['V'], path, cfg) if kind == 'dir' else [path]
	om = g['omitted']()
	return [f for f in out if f not in om]

def cc(c, all_errors):
	s = g['source'](c)
	if not os.path.exists(s):
		return c, None, 'to write'
	fl = ['-I', STUB] + g['flags'](c)
	if all_errors:
		fl = [x if not x.startswith('-fmax-errors') else '-fmax-errors=0' for x in fl]
	r = subprocess.run(['gcc'] + fl + [s], capture_output=True, text=True, env=env)
	return c, r.returncode, r.stderr

g['amdincludes']()
fs = files()
print('files', len(fs))
for rnd in range(12):
	with ThreadPoolExecutor(os.cpu_count()) as ex:
		res = list(ex.map(lambda c: cc(c, False), fs))
	miss = set()
	for c, rc, err in res:
		if rc and err != 'to write':
			for m in re.finditer(r'fatal error: ([^:\s]+): No such file', err):
				miss.add(m.group(1))
	ok = sum(1 for c, rc, e in res if rc == 0)
	print('round %d: %d compile, %d headers missing' % (rnd, ok, len(miss)))
	if not miss:
		break
	for h in miss:
		p = os.path.join(STUB, h)
		os.makedirs(os.path.dirname(p), exist_ok=True)
		open(p, 'w').close()
stubs = sorted(os.path.relpath(os.path.join(r, f), STUB) for r, ds, fl in os.walk(STUB) for f in fl)
with ThreadPoolExecutor(os.cpu_count()) as ex:
	res = list(ex.map(lambda c: cc(c, True), fs))
func, typ, mem, other = collections.Counter(), collections.Counter(), collections.Counter(), collections.Counter()
for c, rc, err in res:
	if not rc or err == 'to write':
		continue
	seen = set()
	for l in err.split('\n'):
		m = re.search(r'implicit declaration of function \'(\w+)\'', l)
		if m: k = ('f', m.group(1))
		elif re.search(r'unknown type name \'(\w+)\'', l): k = ('t', re.search(r'unknown type name \'(\w+)\'', l).group(1))
		elif re.search(r'\'(\w+)\' undeclared', l): k = ('f', re.search(r'\'(\w+)\' undeclared', l).group(1))
		elif re.search(r'has no member named \'(\w+)\'', l): k = ('m', re.search(r'(struct \w+|\'\w+\')[^\']*has no member named \'(\w+)\'', l) and re.search(r'has no member named \'(\w+)\'', l).group(1))
		elif re.search(r'(invalid use of undefined type|storage size of) \'?(struct \w+)', l): k = ('t', re.search(r'(struct \w+)', l).group(1))
		elif ' error: ' in l: k = ('o', re.sub(r'.* error: ', '', l)[:90])
		else: continue
		if k in seen: continue
		seen.add(k)
		{'f': func, 't': typ, 'm': mem, 'o': other}[k[0]][k[1]] += 1
ok = sum(1 for c, rc, e in res if rc == 0)
tw = sum(1 for c, rc, e in res if e == 'to write')
print('\nwith %d stub headers: %d of %d files compile, %d to write (GPL: replace/)' % (len(stubs), ok, len(fs), tw))
reg = lambda k: re.match(r'^(reg|mm|ix|smn)[A-Z0-9_]', k) or re.match(r'^[A-Z0-9_]+__[A-Z0-9_]+', k)
print('distinct missing functions/macros %d (%d register-like), types %d, struct members %d, other errors %d' % (len(func), sum(1 for k in func if reg(k)), len(typ), len(mem), len(other)))
func = collections.Counter({k: v for k, v in func.items() if not reg(k)})
for name, c in (('functions/macros', func), ('types', typ), ('members', mem), ('other', other)):
	print('\n-- %s (files using it), top 25' % name)
	for k, n in c.most_common(25):
		print('%5d  %s' % (n, k))
import json
json.dump({'func': func, 'typ': typ, 'other': other, 'stubs': stubs}, open(STUB + '.json', 'w'))
print('\n-- stub headers')
print(' '.join(stubs))
