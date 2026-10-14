#!/usr/bin/env python3
"""
Candidates for the LP64 problem in userspace: data whose memory image
leaves the program (file, disk, network, device) and that contains a
`long'. kencc's long is 32 bit on every arch, gcc's is 64 bit on amd64,
so such a struct or value changes size and layout there (nusb/disk's
Cbw/Csw, cwfs's Dentry, scat's catalogue records, snoopy's pcap header).
A long that stays in memory is harmless.

Reported, each as a line to review by hand (regexes, not a C parser --
expect false positives, e.g. two programs' structs of the same name):
  IO    read/write/pread/pwrite/readn/Bread/Bwrite/... of a variable whose
        type is a struct with a long member
  MEM   memmove/memcpy/memcmp between such a struct and something else
  CAST  a cast to a pointer to such a struct (from a byte buffer?)
  VAR   read/write/memmove of a plain long variable
  SIZE  sizeof(long) / (long*) -- a 4-byte word on disk or wire?
"long" includes typedefs of it (Timet, DAngle, Off in cwfs/32bit.h, ...),
found in the tree. Not searched: the kernel (sys/src/9), APE (LP64 on
purpose), gs, and the compilers/assemblers/linkers (scripts/check/cmp-lp64.sh
covers 6c/6a/6l). Struct layouts against kencc: scripts/port/cmp-struct-layout.sh.

usage: scripts/check/find-long-io.py [--structs] [dir...]   (default sys/src)
    --structs  only list the structs with long members
"""
import os, re, sys

SKIP = re.compile(r'sys/src/(9|ape|cmd/gs|cmd/(cc|[0-9kqtv][cal]))(/|$)')
ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
os.chdir(ROOT)

def sources(dirs, exts):
	for top in dirs:
		for d, _, fs in os.walk(top):
			if SKIP.match(d + '/'):
				continue
			for f in fs:
				if f.endswith(exts):
					p = os.path.join(d, f)
					yield p, open(p, errors='replace').read()

def nocomments(t):
	return re.sub(r'/\*.*?\*/', lambda m: '\n' * m.group(0).count('\n'), t, flags=re.S)

def program(p):
	"""what a name is visible in: sys/include everywhere, else the
	program (sys/src/cmd/x, sys/src/cmd/{aux,ip,...}/x, sys/src/libx)"""
	parts = p.split('/')
	if parts[1] == 'include':
		return ''
	n = 4
	if parts[2] == 'cmd' and len(parts) > 5 and parts[3] in ('aux', 'ip', 'disk', 'upas', 'nusb', 'ndb', 'auth', 'audio', 'vnc'):
		n = 5
	return '/'.join(parts[:min(n, len(parts) - 1)])

def visible(prog, byprog):
	"""merge what sys/include and prog (or a program it is part of) define"""
	out = dict(byprog.get('', {}))
	for k, v in byprog.items():
		if k and (prog == k or prog.startswith(k + '/') or k.startswith(prog + '/')):
			out.update(v)
	return out

# typedefs of long (per program; not ulong, which u.h makes 32 bit,
# nor the vlrt.c copies of it), then the long pattern including them
aliases = {}	# program -> {name: path}
for p, t in sources(['sys/src', 'sys/include'], ('.c', '.h', '.y')):
	for m in re.finditer(r'^\s*typedef\s+(?:unsigned\s+|signed\s+)?long\s+(\w+)\s*;', t, re.M):
		if m.group(1) != 'ulong':
			aliases.setdefault(program(p), {})[m.group(1)] = p
longrx = {}
def LONG(prog):
	if prog not in longrx:
		longrx[prog] = re.compile(r'(?<![\w])(?:(?:(?:unsigned|signed)\s+)?long(?![ \t]+long)(?![ \t]+double)%s)\b'
			% ''.join('|' + re.escape(a) for a in sorted(visible(prog, aliases))))
	return longrx[prog]

# structs with a long member
structs = {}	# program -> {name: (path, members)}
RX_STRUCT = re.compile(r'^(?:typedef\s+)?(?:struct|union)\s+(\w+)\s*\n?\{(.*?)\n\}', re.S | re.M)
for p, t in sources(['sys/src', 'sys/include'], ('.c', '.h')):
	rx = LONG(program(p))
	for m in RX_STRUCT.finditer(nocomments(t)):
		members = [l.strip() for l in m.group(2).split('\n')
			if rx.search(l) and not re.match(r'^\s*(?:unsigned\s+)?long\s*\(\s*\*', l)]
		if members:
			structs.setdefault(program(p), {}).setdefault(m.group(1), (p, members))

args = [a for a in sys.argv[1:] if not a.startswith('--')]
if '--structs' in sys.argv:
	for prog in sorted(structs):
		for n, (p, members) in sorted(structs[prog].items()):
			print('%s\t%s\t%s' % (p, n, ' | '.join(members)))
	sys.exit(0)

IOFN = r'(?:read|write|pread|pwrite|readn|Bread|Bwrite|send|recv|sendto|recvfrom|fread|fwrite|devread|devwrite|wrenread|wrenwrite)'
MEMFN = r'(?:memmove|memcpy|memcmp)'
ALLOC = re.compile(r'^\s*(?:e?m?alloc\w*|mallocz|calloc|realloc|erealloc|emalloc\w*|nil\b|0\b|\w*aux\b|\w+->aux\b|v\b|a\b|b\b|arg\b)')

def callargs(s, i):
	d, cur, out = 0, '', []
	for c in s[i:]:
		if c == '(':
			d += 1
		elif c == ')':
			if d == 0:
				break
			d -= 1
		if c == ',' and d == 0:
			out.append(cur); cur = ''
		else:
			cur += c
	out.append(cur)
	return out

def var(a):
	a = re.sub(r'^\s*\(\s*(?:void|char|uchar|u8int)\s*\*\s*\)\s*', '', a.strip())
	m = re.match(r'^&?\s*\(?\s*\*?\s*(\w+)\s*\)?$', a)
	return m.group(1) if m else None

for p, t in sources(args or ['sys/src'], ('.c', '.y')):
	prog = program(p)
	vis = visible(prog, structs)
	t = nocomments(t)
	decl, longs = {}, set()
	for m in re.finditer(r'\b(?:struct\s+)?([A-Za-z_]\w*)\s+((?:\*?\s*\w+(?:\[[^\]]*\])?\s*,\s*)*\*?\s*\w+(?:\[[^\]]*\])?)\s*[;,)=]', t):
		if m.group(1) in vis:
			for v in re.findall(r'\*?\s*(\w+)', m.group(2)):
				decl.setdefault(v, set()).add(m.group(1))
	for m in re.finditer(r'^\s*(?:static\s+)?(?:unsigned\s+|signed\s+)?long\s+(?!long|double)([^;(]*);', t, re.M):
		for v in m.group(1).split(','):
			mm = re.match(r'^\s*\*?\s*(\w+)', v.split('=')[0])
			if mm:
				longs.add(mm.group(1))
	for i, l in enumerate(t.split('\n')):
		where = '%s:%d: %s' % (p, i + 1, l.strip()[:150])
		for m in re.finditer(r'\b(%s|%s)\s*\(' % (IOFN, MEMFN), l):
			a = callargs(l, m.end())
			if re.match(MEMFN, m.group(1)):
				vs = [var(x) for x in a[:2]]
				ts = [decl.get(v, set()) if v else set() for v in vs]
				hit = [v for v, ty in zip(vs, ts) if ty]
				if hit and not (ts[0] & ts[1]):
					print('MEM  %s   [%s]' % (where, ', '.join('%s: %s' % (v, '/'.join(sorted(decl[v]))) for v in hit)))
				hit = [v for v in vs if v in longs]
			else:
				bi = 0 if m.group(1) in ('fread', 'fwrite') else 2 if m.group(1).startswith(('dev', 'wren')) else 1
				v = var(a[bi]) if len(a) > bi else None
				if v in decl:
					print('IO   %s   [%s: %s]' % (where, v, '/'.join(sorted(decl[v]))))
				hit = [v] if v in longs else []
			if hit:
				print('VAR  %s   [long %s]' % (where, ', '.join(hit)))
		for m in re.finditer(r'(\bsizeof\s*)?\(\s*(?:struct\s+)?(\w+)\s*\*\s*\)\s*([^;]*)', l):
			# a cast has an operand: not a prototype (Device*); or sizeof(T*)
			if m.group(2) in vis and not m.group(1) and re.match(r'[\w(&*]', m.group(3)) and not ALLOC.match(m.group(3)):
				print('CAST %s' % where)
		if re.search(r'sizeof\s*\(\s*(?:unsigned\s+)?long\s*\)|\(\s*(?:unsigned\s+)?long\s*\*\s*\)', l):
			print('SIZE %s' % where)
