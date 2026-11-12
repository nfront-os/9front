#!/usr/bin/env python3
"""provenance.py [-v] [--check] [--linux TREE]... [PATH...]: lines of our
LinuxKPI that are Linux's GPL text.

Our own files (default: every .c and .h under sys/src/linux/kpi and
sys/src/linux/replace, or the PATHs given) must not carry text of a Linux
file that is GPL-only or has no notice.  For each of our files this counts
the lines of code and of comments (white space folded, 25 characters or
more, notices left out; a line that only states a fact of the interface,
a number's name, an enumerator, a member, a prototype, is counted apart)
that occur in such a Linux file and in no permissive source: Linux files
under MIT/BSD (alone or as the other choice of a dual licence), vendor/,
and the templates openbsd/ and freebsd/.  Lines of an interface (a call,
a field) found in a permissive file do not count; text that only the GPL
files have does.  The Linux trees compared: every --linux TREE, by default
~/Devel/linux-7.3.1 (vendor/'s version) and ~/Devel/linux-6.6 (the first
LinuxKPI's) where present; all of each, not only the file of the same name.

Prints the files with any such line, most first, with the Linux file most
of them come from; -v also the lines.  --check: exit 1 if a file has more
than 2 such lines, or more than 8 of the interface lines (a copy member
by member is a copy too): more only by an entry in sys/src/linux/PROVENANCE
(path, tab, number, tab, why), and only if they do not follow the Linux
file's order (the selection and arrangement our own)."""
import os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
D = os.path.normpath(os.path.join(HERE, '../../sys/src/linux'))
MIN = 25
LIMIT = 2
FACTLIMIT = 8	# a near copy of an interface, member by member, counts too
DIRS = ['include', 'arch/x86/include', 'drivers/gpu', 'drivers/dma-buf',
	'drivers/video', 'drivers/i2c/algos', 'lib', 'kernel', 'mm']

NOTICE = re.compile(r'SPDX|Copyright|licen[sc]e|Permission|WARRANT|LIABLE|Redistribution|'
	r'Software Foundation|MERCHANTABILITY|ALL RIGHTS|DAMAGES|THE SOFTWARE|EXPRESS OR', re.I)

def norm(text):
	"""the lines of code and of comments (comment marks dropped), white
	space folded; notices left out"""
	code, comment = [], []
	pos, n = 0, len(text)
	for m in re.finditer(r'/\*.*?(\*/|$)|//[^\n]*|"(\\.|[^"\\\n])*"', text, re.S):
		if m.group(0).startswith('"'):
			continue
		code.append(text[pos:m.start()])
		body = m.group(0)
		body = body[2:-2] if body.startswith('/*') and body.endswith('*/') else body[2:]
		comment.append(body)
		code.append('\n' * body.count('\n'))
		pos = m.end()
	code.append(text[pos:])
	out = {}	# in the file's order
	for part, strip in ((''.join(code), False), ('\n'.join(comment), True)):
		for l in part.split('\n'):
			if strip:
				l = re.sub(r'^\s*\*+', '', l)
			l = re.sub(r'\s+', ' ', l).strip()
			if len(l) >= MIN and not NOTICE.search(l):
				out.setdefault(l, len(out))
	return list(out)

# a line that only states a fact of the interface: a number's name, an
# enumerator, a member, a prototype; written by anyone, it reads the same
FACT = [re.compile(x) for x in (
	r'^#\s*define \w+ \(?-?(0x[0-9a-fA-F]+|\d+)[uUlL]*\)?$',
	r'^#\s*define \w+ \(?(BIT|GENMASK|BIT_ULL|GENMASK_ULL)\([\w, ]+\)\)?$',
	r'^#\s*define \w+ \(\d+ ?<< ?\d+\)$',
	r'^#\s*ifn?def \w+$',
	r'^#\s*define \w+$',
	r'^#\s*define \w+(\([\w, .]*\))? __attribute__\(\(\w+(\([\w, #.]*\))?\)\)$',
	r'^\w+ = (0x[0-9a-fA-F]+|\d+|\w+|BIT\(\d+\)|\(?\d+ ?<< ?\d+\)?),?$',
	r'^(const |unsigned |signed |struct |enum |union |volatile )*\w+[ *]+\w+(\[\w*\])?(:\d+)?;$',
	r'^(extern |static |inline |const |unsigned |struct |enum |__\w+ )*\w+[ *]+\**\w+\([^;{]*\);$',
)]

def fact(l):
	return any(f.match(l) for f in FACT)

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

ORDER = 0.75	# above: the interface lines follow the Linux file's order

def inorder(lines, gpl, top, linux):
	"""of our interface lines from top, the share of neighbours that
	stand in top in the same order (about 0.5 if ours are arranged
	otherwise)"""
	pos = {l: i for i, l in enumerate(norm(read(linux[top])))}
	seq = [pos[l] for l in lines if gpl[l] == top and l in pos]
	if len(seq) < 4:
		return None
	return sum(1 for a, b in zip(seq, seq[1:]) if a < b) / (len(seq) - 1)

def main():
	args = sys.argv[1:]
	verbose = check = False
	trees = []
	args_paths = []
	while args:
		a = args.pop(0)
		if a == '-v': verbose = True
		elif a == '--check': check = True
		elif a == '--linux': trees.append(os.path.expanduser(args.pop(0)))
		else: args_paths.append(a)
	if not trees:
		trees = [t for t in (os.path.expanduser('~/Devel/linux-7.3.1'),
			os.path.expanduser('~/Devel/linux-6.6')) if os.path.isdir(t)]
	if not trees:
		print('provenance.py: no Linux tree (--linux)', file=sys.stderr)
		sys.exit(2)

	gpl = {}	# line -> a GPL-only Linux file holding it
	free = set()
	paths = {}
	for t in trees:
		for d in DIRS:
			for p in sources(os.path.join(t, d)):
				text = read(p)
				lines = norm(text)
				if gplonly(text):
					rel = os.path.basename(t) + '/' + os.path.relpath(p, t)
					paths[rel] = p
					for l in lines:
						gpl.setdefault(l, rel)
				else:
					free.update(lines)
	for d in ('vendor', 'openbsd', 'freebsd'):
		for p in sources(os.path.join(D, d)):
			free.update(norm(read(p)))

	linux = paths
	paths = args_paths
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
		mine = norm(read(p))
		hits = [l for l in mine if l in gpl and l not in free and not fact(l)]
		factl = [l for l in mine if l in gpl and l not in free and fact(l)]
		facts = len(factl)
		if hits or facts:
			rows.append((p, hits, facts, factl))
	rows.sort(key=lambda r: (-len(r[1]) - r[2], r[0]))
	bad = 0
	for p, hits, facts, factl in rows:
		rel = os.path.relpath(p, D)
		srcs = {}
		for l in hits + factl:
			srcs[gpl[l]] = srcs.get(gpl[l], 0) + 1
		top = max(srcs, key=srcs.get)
		order = inorder(factl, gpl, top, linux)
		samefile = order is not None and order > ORDER
		over = len(hits) > LIMIT or facts > allow.get(rel, FACTLIMIT) or (facts > FACTLIMIT and samefile)
		bad += over
		print('%s%4d+%-3d %s  (%d from %s%s)' % ('!' if over else ' ', len(hits), facts, rel, srcs[top], top,
			', in its order' if samefile else ''))
		if verbose:
			for l in hits:
				print('\t\t' + l)
			for l in factl:
				print('\t\tfact: ' + l)
	print('%d files with GPL lines, %d over the limit' % (len(rows), bad))
	if check and bad:
		sys.exit(1)

main()
