#!/usr/bin/env python3
"""provenance.py [-v] [--check] [--linux TREE]... [PATH...]: lines of our
LinuxKPI that are Linux's GPL text.

Our own files (default: every .c and .h under sys/src/linux/kpi and
sys/src/linux/replace, or the PATHs given) must not carry text of a Linux
file that is GPL-only or has no notice.  For each of our files this counts
the lines (comments dropped, white space folded, 25 characters or more)
that occur in such a Linux file and in no permissive source: Linux files
under MIT/BSD (alone or as the other choice of a dual licence), vendor/,
and the templates openbsd/ and freebsd/.  Lines of an interface (a call,
a field) found in a permissive file do not count; text that only the GPL
files have does.  The Linux trees compared: every --linux TREE, by default
~/Devel/linux-7.3.1 (vendor/'s version) and ~/Devel/linux-6.6 (the first
LinuxKPI's) where present; all of each, not only the file of the same name.

Prints the files with any such line, most first, with the Linux file most
of them come from; -v also the lines.  --check: exit 1 if a file has more
than 2 such lines, or more than its entry in sys/src/linux/PROVENANCE
allows (path, tab, number, tab, why)."""
import os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
D = os.path.normpath(os.path.join(HERE, '../../sys/src/linux'))
MIN = 25
LIMIT = 2
DIRS = ['include', 'arch/x86/include', 'drivers/gpu', 'drivers/dma-buf',
	'drivers/video', 'drivers/i2c/algos', 'lib', 'kernel', 'mm']

def norm(text):
	text = re.sub(r'/\*.*?\*/', ' ', text, flags=re.S)
	text = re.sub(r'//[^\n]*', '', text)
	out = set()
	for l in text.split('\n'):
		l = re.sub(r'\s+', ' ', l).strip()
		if len(l) >= MIN:
			out.add(l)
	return out

def read(p):
	try:
		with open(p, errors='replace') as f:
			return f.read()
	except OSError:
		return ''

PERMISSIVE = re.compile(r'\b(MIT|BSD|ISC|X11|Zlib)\b', re.I)

def gplonly(text):
	"""True if a Linux file is GPL-only or has no notice."""
	head = text[:6000]
	m = re.search(r'SPDX-License-Identifier:\s*([^\n]*)', head)
	if m:
		return not PERMISSIVE.search(m.group(1))
	if 'Permission is hereby granted' in head or 'Permission to use, copy, modify' in head \
	   or 'Redistribution and use in source and binary forms' in head:
		return False
	return True

def sources(root):
	for dp, dn, fn in os.walk(root):
		dn.sort()
		for n in sorted(fn):
			if n.endswith(('.c', '.h')):
				yield os.path.join(dp, n)

def main():
	args = sys.argv[1:]
	verbose = check = False
	trees = []
	paths = []
	while args:
		a = args.pop(0)
		if a == '-v': verbose = True
		elif a == '--check': check = True
		elif a == '--linux': trees.append(os.path.expanduser(args.pop(0)))
		else: paths.append(a)
	if not trees:
		trees = [t for t in (os.path.expanduser('~/Devel/linux-7.3.1'),
			os.path.expanduser('~/Devel/linux-6.6')) if os.path.isdir(t)]
	if not trees:
		print('provenance.py: no Linux tree (--linux)', file=sys.stderr)
		sys.exit(2)

	gpl = {}	# line -> a GPL-only Linux file holding it
	free = set()
	for t in trees:
		for d in DIRS:
			for p in sources(os.path.join(t, d)):
				text = read(p)
				lines = norm(text)
				if gplonly(text):
					rel = os.path.basename(t) + '/' + os.path.relpath(p, t)
					for l in lines:
						gpl.setdefault(l, rel)
				else:
					free |= lines
	for d in ('vendor', 'openbsd', 'freebsd'):
		for p in sources(os.path.join(D, d)):
			free |= norm(read(p))

	if not paths:
		paths = [os.path.join(D, 'kpi'), os.path.join(D, 'replace')]
	ours = []
	for p in paths:
		ours.extend(sources(p) if os.path.isdir(p) else [p])

	allow = {}
	for l in read(os.path.join(D, 'PROVENANCE')).split('\n'):
		f = l.split('\t')
		if len(f) >= 2 and not l.startswith('#'):
			allow[f[0]] = int(f[1])

	rows = []
	for p in ours:
		hits = sorted(l for l in norm(read(p)) if l in gpl and l not in free)
		if hits:
			rows.append((p, hits))
	rows.sort(key=lambda r: (-len(r[1]), r[0]))
	bad = 0
	for p, hits in rows:
		rel = os.path.relpath(p, D)
		srcs = {}
		for l in hits:
			srcs[gpl[l]] = srcs.get(gpl[l], 0) + 1
		top = max(srcs, key=srcs.get)
		limit = allow.get(rel, LIMIT)
		over = len(hits) > limit
		bad += over
		print('%s%4d  %s  (%d from %s)' % ('!' if over else ' ', len(hits), rel, srcs[top], top))
		if verbose:
			for l in hits:
				print('\t\t' + l)
	print('%d files with GPL lines, %d over the limit' % (len(rows), bad))
	if check and bad:
		sys.exit(1)

main()
