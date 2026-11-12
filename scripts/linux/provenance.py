#!/usr/bin/env python3
"""provenance.py [-v] [--check] [--strict] [--linux TREE]... [PATH...]: lines of our
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

--strict: a line of the BSD templates (openbsd/, freebsd/) is no
permissive source where a GPL-only Linux file has it too (a template
that took Linux's text would carry it through).

Prints the files with any such line, most first, as text+code+facts:
text (any other line, and lines of code that stand in a row as in the
Linux file), lines of code that match alone (a call, a condition, an
assignment: at most 4, no exceptions), facts of the interface; with the
Linux file most of them come from; -v also the lines.  --check: exit 1 if a file has more
than 2 such lines, or more than 8 of the interface lines (a copy member
by member is a copy too): more only by an entry in sys/src/linux/PROVENANCE
(path, tab, number, tab, why), and only if they do not follow the Linux
file's order (the selection and arrangement our own)."""
import hashlib, os, pickle, re, sys

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
	r'^(const |unsigned |signed |struct |enum |union |volatile |__\w+ )*\w+( __\w+)?[ *]+\w+(\[\w*\])?(:\d+)?;$',
	r'^\w+,$',
	r'^((const |struct |unsigned |enum |static |inline )*\w+ \**)?\w+\((void|((const |struct |unsigned |enum )*\w+ \**\w+(, )?)*)\)$',	# a function's head
	r'^\w+\(\w+, \w+\)$',	# an entry of an X-macro table (name, function)
	r'^\w+\(\w+\),$',	# an entry of a table of tests (SUBTEST(f),)
	r'^(const )?(struct |enum |unsigned )?\w+ \**\(\*\w+\)\([^()]*\);$',
	r'^\**\w+\(((const |struct |unsigned |enum )*\w+ \**\w+, )*(const |struct |unsigned |enum )*\w+ \**\w+,$',
	r'^((const |struct |unsigned |enum )*\w+ \**\w+, )*(const |struct |unsigned |enum )*\w+ \**\w+\)( \{)?$',
	r'^(static )?(const )?struct \w+ \**\w+(\[\])? = (\{|\w+;)$',
	r'^typedef (signed |unsigned |long |short |const |struct |union |enum )*\w+ \**\w+;$',
	r'^#\s*define \w+ \(?\w+\)?$',
	r'^#\s*define \w+ \(?\w+( \| \w+)+\)?$',
	r'^\w+ = \w+( \| \w+)+,?$',
	r'^#\s*define \w+ \(\(?-\w+ - 1\)?\)$',
	r'^#\s*define \w+ \(\(\w+\)\(?-\w+ - 1\)?\)$',
	r'^#\s*define \w+ \(?\(\w+\)~?\(?\w+\)?\)?$',
	r'^#\s*define \w+ \(\d+U?L*L? << \d+\)$',
	r'^#\s*include [<"][\w/.-]+[>"]$',
	r'^(typedef )?(struct|enum|union) \w+ \{$',
	r'^(static |inline |const |unsigned |struct |enum )*\w+ ?\**$',
	r'^(?!return\b)(extern |static |inline |const |unsigned |struct |enum |__\w+ )*\w+[ *]+\**\w+\([^;{]*\);$',
)]

# a plain line of code: one call, a condition on one call, an assignment.
# Alone it is no copy (anyone writes it so), two in a row that stand in a
# row in the Linux file too are (a function taken line by line)
CODE = [re.compile(x) for x in (
	r'^[\w>.*-]+ = \w+\((?:[\w &>.*()-]+(?:, ?[\w &>.*()-]+)*)?\);$',
	r'^for \(\w+ = \w+; \w+ < [\w>.-]+; \w+\+\+\)( \{)?$',
	r'^\.\w+ = &?\w+,$',
	r'^#\s*define \w+\([\w, ]*\) !?\(?\w+\([\w, !()]*\)\)?$',
	r'^(return )?!?\w+\((?:[\w &>.*-]+(?:, ?[\w &>.*-]+)*)?\)\)?;$',
	r'^(if|while) \(!?\w+\((?:[\w &>.*-]+(?:, ?[\w &>.*-]+)*)?\)\)( \{)?$',
	r'^[\w>.-]+ [|&]?= [\w|&]+( [|&] )?(BIT\(\w+\)|[\w>.-]+)?;$',
	r'^\w+\([\w>.-]+, [\w>.-]+\);$',
	r'^BIT\(\w+\)\);$',
)]
CODELIMIT = 4	# lines of code that match alone, at most

def fact(l):
	return any(f.match(l) for f in FACT)

def code(l):
	return any(f.match(l) for f in CODE)

def inrun(mine, matched, top, linux):
	"""the matched lines of ours that stand next to another matched one
	(at most one line between) that is next to it in top too (at most
	two lines between, in the same order)"""
	pos = {l: i for i, l in enumerate(norm(read(linux[top])))} if top in linux else {}
	idx = [(i, l) for i, l in enumerate(mine) if l in matched]
	run = set()
	for (i, a), (j, b) in zip(idx, idx[1:]):
		if j - i <= 2 and a in pos and b in pos and 0 < pos[b] - pos[a] <= 3:
			run.add(a)
			run.add(b)
	return run

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

ORDER = 0.6	# above: the interface lines follow the Linux file's order

def inorder(lines, gpl, top, linux):
	"""of our interface lines from top, the share of neighbours that are
	neighbours in top too (in its order, among the lines we share): 1 for
	a copy, low for an arrangement of our own (related members still
	stand together)"""
	pos = {l: i for i, l in enumerate(norm(read(linux[top])))}
	# numbers by name: listed by value, as anyone lists them (not counted)
	ours = [l for l in lines if gpl[l] == top and l in pos and not any(f.match(l) for f in FACT[:3])]
	if len(ours) < 4:
		return None
	rank = {l: r for r, l in enumerate(sorted(ours, key=pos.get))}
	seq = [rank[l] for l in ours]
	return sum(1 for a, b in zip(seq, seq[1:]) if b == a + 1) / (len(seq) - 1)

def index(trees):
	"""the lines of the GPL-only Linux files (line -> file), those of the
	permissive sources, the GPL files' paths; cached in ~/.cache by the
	trees, the permissive files' times and this script"""
	h = hashlib.sha256(open(os.path.abspath(__file__), 'rb').read())
	for t in trees:
		h.update(os.path.abspath(t).encode())
	for d in ('vendor', 'openbsd', 'freebsd'):
		for p in sources(os.path.join(D, d)):
			st = os.stat(p)
			h.update(('%s %d %d' % (p, st.st_mtime_ns, st.st_size)).encode())
	cache = os.path.expanduser('~/.cache/9front-provenance/%s.pickle' % h.hexdigest()[:16])
	try:
		with open(cache, 'rb') as f:
			return pickle.load(f)
	except (OSError, EOFError, pickle.UnpicklingError):
		pass
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
	for p in sources(os.path.join(D, 'vendor')):
		free.update(norm(read(p)))
	bsd = set()	# the BSD templates' lines (--strict credits them only where Linux's GPL files lack them)
	for d in ('openbsd', 'freebsd'):
		for p in sources(os.path.join(D, d)):
			bsd.update(norm(read(p)))

	os.makedirs(os.path.dirname(cache), exist_ok=True)
	with open(cache + '.tmp', 'wb') as f:
		pickle.dump((gpl, free, bsd, paths), f, protocol=pickle.HIGHEST_PROTOCOL)
	os.replace(cache + '.tmp', cache)
	return gpl, free, bsd, paths

def main():
	args = sys.argv[1:]
	verbose = check = strict = False
	trees = []
	args_paths = []
	while args:
		a = args.pop(0)
		if a == '-v': verbose = True
		elif a == '--check': check = True
		elif a == '--strict': strict = True
		elif a == '--linux': trees.append(os.path.expanduser(args.pop(0)))
		else: args_paths.append(a)
	if not trees:
		trees = [t for t in (os.path.expanduser('~/Devel/linux-7.3.1'),
			os.path.expanduser('~/Devel/linux-6.6')) if os.path.isdir(t)]
	if not trees:
		print('provenance.py: no Linux tree (--linux)', file=sys.stderr)
		sys.exit(2)

	gpl, free, bsd, paths = index(trees)
	if strict:
		free |= bsd - set(gpl)
	else:
		free |= bsd

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
		matched = [l for l in mine if l in gpl and l not in free]
		if not matched:
			continue
		srcs = {}
		for l in matched:
			srcs[gpl[l]] = srcs.get(gpl[l], 0) + 1
		top = max(srcs, key=srcs.get)
		run = inrun(mine, set(matched), top, linux)
		hits = [l for l in matched if not fact(l) and (not code(l) or l in run)]
		codel = [l for l in matched if code(l) and l not in run]
		factl = [l for l in matched if fact(l)]
		rows.append((p, hits, codel, factl, srcs[top], top))
	rows.sort(key=lambda r: (-len(r[1]) - len(r[2]) - len(r[3]), r[0]))
	bad = 0
	for p, hits, codel, factl, ntop, top in rows:
		rel = os.path.relpath(p, D)
		facts = len(factl)
		order = inorder(factl, gpl, top, linux)
		samefile = order is not None and order > ORDER
		over = len(hits) > LIMIT or len(codel) > CODELIMIT or \
			facts > allow.get(rel, FACTLIMIT) or (facts > FACTLIMIT and samefile)
		bad += over
		print('%s%4d+%-2d+%-3d %s  (%d from %s%s)' % ('!' if over else ' ', len(hits), len(codel), facts,
			rel, ntop, top, ', in its order' if samefile else ''))
		if verbose:
			for l in hits:
				print('\t\t' + l)
			for l in codel:
				print('\t\tcode: ' + l)
			for l in factl:
				print('\t\tfact: ' + l)
	print('%d files with GPL lines, %d over the limit' % (len(rows), bad))
	if check and bad:
		sys.exit(1)

main()
