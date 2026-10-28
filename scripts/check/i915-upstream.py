#!/usr/bin/env python3
"""i915-upstream.py: follow Linux's i915 in aux/intelgpu.

  i915-upstream.py OLD NEW [ourfile...]
	the changes from Linux tree OLD to NEW in the files UPSTREAM
	lists, grouped by our file: the hunks that touch a name it lists
	(for an excerpt, a name the excerpt defines)
  i915-upstream.py -c [TREE]
	check that every Linux file and name UPSTREAM lists is in TREE
  i915-upstream.py -n [TREE]
	write NOTICE's part for TREE: the notice of each Linux file
	UPSTREAM lists, verbatim; fails on a file that is not MIT-like

TREE defaults to ~/Devel/linux-6.6.  UPSTREAM is in
sys/src/cmd/aux/gpu/intel; see there for the procedure."""
import os, re, subprocess, sys

D = os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../sys/src/cmd/aux/gpu/intel')
DEF = os.path.expanduser('~/Devel/linux-6.6')
MARK = '==== Linux files the code follows now, with their notices'

def upstream():
	"""(our file, Linux file, [Linux names], kind) per line"""
	out = []
	for l in open(os.path.join(D, 'UPSTREAM')):
		f = l.rstrip('\n').split('\t')
		if l.startswith('#') or len(f) != 3:
			continue
		ours, lx, names = f
		if names in ('*', '+'):
			out.append((ours, lx, excerptnames(ours), names))
			continue
		ns = []
		for n in names.split():
			if n.startswith('-'):
				continue
			ns.append(n.split('=')[-1])
		out.append((ours, lx, ns, 'code'))
	return out

def excerptnames(ours):
	t = open(os.path.join(D, ours)).read()
	ns = re.findall(r'^#\s*define\s+(\w+)', t, re.M)
	ns += re.findall(r'^\s+([A-Z_][A-Z0-9_]*)\s*(?:=|,|$)', t, re.M)
	ns += re.findall(r'^(?:struct|union)\s+(\w+)\s*\{', t, re.M)
	return [n for n in ns if not n.startswith('_INTELGPU_')]

def notice(text):
	"""a file's notice: its leading comments up to the one with the
	copyright (and the licence)"""
	out = []
	for m in re.finditer(r'\s*(//[^\n]*|/\*.*?\*/)', text, re.S):
		if m.start() != (out[-1][1] if out else 0):
			break
		out.append((m.group(1), m.end()))
		if re.search(r'Copyright', m.group(1)):
			break
	return '\n'.join(c for c, e in out)

def hunks(diff):
	cur = None
	for l in diff.split('\n'):
		if l.startswith('@@'):
			if cur:
				yield cur
			cur = [l]
		elif cur is not None:
			cur.append(l)
	if cur:
		yield cur

def follow(old, new, only):
	byours = {}
	for ours, lx, names, kind in upstream():
		if only and ours not in only:
			continue
		byours.setdefault(ours, []).append((lx, names))
	for ours, ls in byours.items():
		out = []
		for lx, names in ls:
			a, b = os.path.join(old, lx), os.path.join(new, lx)
			if not os.path.exists(b):
				out.append('Linux %s: gone in %s (moved or renamed?)' % (lx, new))
				continue
			r = subprocess.run(['diff', '-u', '-p', a, b], capture_output=True, text=True)
			pat = re.compile(r'\b(' + '|'.join(map(re.escape, names)) + r')\b') if names else None
			for h in hunks(r.stdout):
				t = '\n'.join(h)
				if pat is None or pat.search(t):
					out.append('---- %s\n%s' % (lx, t))
		if out:
			print('==== %s\n%s\n' % (ours, '\n'.join(out)))

def check(tree):
	bad = 0
	for ours, lx, names, kind in upstream():
		p = os.path.join(tree, lx)
		if not os.path.exists(p):
			print('%s: %s missing' % (ours, lx))
			bad += 1
			continue
		t = open(p).read()
		for n in names:
			if not re.search(r'\b' + re.escape(n) + r'\b', t):
				print('%s: %s not in %s' % (ours, n, lx))
				bad += 1
	print('%d problems' % bad)
	return bad

def notices(tree):
	ver = open(os.path.join(tree, 'Makefile')).read()
	ver = '.'.join(re.search(r'^%s = (\d+)' % k, ver, re.M).group(1) for k in ('VERSION', 'PATCHLEVEL'))
	seen, parts = set(), []
	for ours, lx, names, kind in upstream():
		if lx in seen:
			continue
		seen.add(lx)
		n = notice(open(os.path.join(tree, lx)).read())
		if 'GPL' in n or not re.search(r'MIT|Permission is hereby granted|Permission to use, copy', n):
			sys.exit('%s: not MIT-like:\n%s' % (lx, n))
		parts.append('---- %s (Linux %s)\n%s\n' % (lx, ver, n))
	p = os.path.join(D, 'NOTICE')
	s = open(p).read()
	s = s[:s.index(MARK)] if MARK in s else s.rstrip() + '\n\n'
	open(p, 'w').write(s + MARK + '\n\n' + '\n'.join(parts))
	print('NOTICE: %d files of Linux %s' % (len(parts), ver))

a = sys.argv[1:]
if a and a[0] == '-c':
	sys.exit(check(os.path.expanduser(a[1]) if len(a) > 1 else DEF) != 0)
elif a and a[0] == '-n':
	notices(os.path.expanduser(a[1]) if len(a) > 1 else DEF)
elif len(a) >= 2:
	follow(os.path.expanduser(a[0]), os.path.expanduser(a[1]), set(a[2:]))
else:
	sys.exit(__doc__)
