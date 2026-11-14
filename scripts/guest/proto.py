#!/usr/bin/env python3
"""
Copy a file tree described by 9front proto files (mkfs(8), proto(2))
into a directory on the build host, like

	disk/mkfs -s srcroot -d dest proto...

which 9front's mkfiles run (the kernel's bootfs.paq, the install image),
but which only runs on Plan 9. Follows sys/src/libdisk/proto.c:

  - indentation (blank = 1, tab = 8) nests a line below the previous one
  - name [mode [uid [gid [source]]]], mode d755, a775, l600, 0444, -
  - $name is the environment variable name ($objtype)
  - + copies the directory's tree, * only its files
  - name=value lines set skip (regexp), uid, gid, mode for the
    lines below
  - # starts a comment

Sources are srcroot/path unless the line names one (relative to the
current directory, like mkfs). uid/gid cannot be set on the host and
are ignored; the permission bits of mode are applied. Git keeps no empty
directories: a missing source with a directory mode (d775) is created
empty, and so is -e path (tmp, lib/firmware in the boot protos, which
give no mode).

-l list writes what mkfs would give each entry on Plan 9, one line per
path: path, mode (octal, with DMDIR 0x80000000, DMAPPEND, DMEXCL), uid,
gid -- the proto's fields, else its uid=, gid=, mode= options over the
source's permissions, as libdisk's copyfile does; an owner neither names
is sys (scripts/replica/publish.py: the replica's db).

usage: scripts/guest/proto.py [-s srcroot] -d dest [-l list] [-e emptydir]... proto...
"""
import os
import re
import shutil
import stat
import sys

DMDIR, DMAPPEND, DMEXCL, DMTMP = 0x80000000, 0x40000000, 0x20000000, 0x04000000


def warn(msg):
	print('proto: ' + msg, file=sys.stderr)


class Proto:
	def __init__(self, src, dest, empty):
		self.src = src
		self.dest = dest
		self.empty = set(os.path.normpath(e).lstrip('/') for e in empty)
		self.meta = {}	# path: (mode, uid, gid), for -l

	@staticmethod
	def parsemode(spec, mask, mode):
		# libdisk's parsemode: a mode= option over the options above it
		s = spec
		op, setall = '', False
		m = 0
		k = DMAPPEND | DMEXCL | DMTMP
		i = 0
		if s[:1] in '01234567' and s:
			k, m, op, s = 0o666, int(s, 8), '=', '!'
		while i < len(s) and not op:
			c = s[i]
			i += 1
			if c == 'a':
				k |= 0o666
			elif c == 'u':
				k |= 0o600
			elif c == 'g':
				k |= 0o60
			elif c == 'o':
				k |= 0o6
			elif c in '-+=':
				op = c
			else:
				return None
		if s == spec and i == 0:
			k |= 0o666
		for c in s[i:]:
			if c == 'r':
				m |= 0o444
			elif c == 'w':
				m |= 0o222
			elif c == 'x':
				m |= 0o111
			elif c == 'a':
				m |= DMAPPEND
			elif c == 'l':
				m |= DMEXCL
			elif c == 't':
				m |= DMTMP
			elif c == '!':
				setall = True
			else:
				return None
		if op in '+-':
			k &= m
		if op == '-':
			m = ~m
		if setall:
			mask = 0
		mask |= k
		return mask, (mode & ~k) | (m & k)

	@staticmethod
	def fullmode(spec):
		# a field's mode: [d][a][l]ooo
		m = re.match(r'^(d?)(a?)(l?)([0-7]{3})$', spec or '')
		if not m:
			return None
		return (DMDIR if m.group(1) else 0) | (DMAPPEND if m.group(2) else 0) | \
			(DMEXCL if m.group(3) else 0) | int(m.group(4), 8)

	def record(self, new, old, fmode, uid, gid, o):
		# what libdisk's copyfile gives the entry
		if fmode is None:
			try:
				st = os.stat(old)
				mode = stat.S_IMODE(st.st_mode) & 0o777
				if stat.S_ISDIR(st.st_mode):
					mode |= DMDIR
			except OSError:
				mode = DMDIR | 0o775
			if 'mask' in o:
				mode = (mode & ~o['mask']) | (o['mmode'] & o['mask'])
		else:
			mode = fmode
		uid = uid if uid not in (None, '-', '') else o.get('uid') or 'sys'
		gid = gid if gid not in (None, '-', '') else o.get('gid') or 'sys'
		self.meta[new] = (mode, uid, gid)

	def lines(self, path):
		for no, raw in enumerate(open(path), 1):
			indent = 0
			i = 0
			for c in raw:
				if c == ' ':
					indent += 1
				elif c == '\t':
					indent += 8
				else:
					break
				i += 1
			text = raw[i:].rstrip('\n')
			if text == '' or text.startswith('#'):
				continue
			yield indent, text, '%s:%d' % (path, no)

	@staticmethod
	def word(w):
		if w.startswith('$'):
			v = os.environ.get(w[1:])
			if v is None:
				raise KeyError(w[1:])
			return v
		return w

	def mode(self, spec):
		if spec in ('', '-'):
			return None
		m = re.match(r'^(d?)(a?)(l?)([0-7]{3})$', spec)
		if not m:
			warn('bad mode specification ' + spec)
			return None
		return int(m.group(4), 8)

	def copyfile(self, old, new, mode):
		dst = os.path.join(self.dest, new)
		os.makedirs(os.path.dirname(dst), exist_ok=True)
		if os.path.isdir(old):
			os.makedirs(dst, exist_ok=True)
		else:
			if os.path.exists(dst):
				os.chmod(dst, stat.S_IWUSR | stat.S_IRUSR)
			shutil.copyfile(old, dst)
			mode = mode if mode is not None else stat.S_IMODE(os.stat(old).st_mode)
			os.chmod(dst, mode | stat.S_IRUSR | stat.S_IWUSR)
			return
		if mode is not None:
			os.chmod(dst, mode | stat.S_IRWXU)

	def tree(self, old, new, rec, skip, o=None, uid=None, gid=None, fmode=None):
		o = o if o is not None else {}
		for name in sorted(os.listdir(old)):
			if skip is not None and skip.search(name):
				continue
			src = os.path.join(old, name)
			n = os.path.join(new, name)
			# permonly: the + line's mode only sets the permission bits
			m = None
			if fmode is not None:
				try:
					st = os.stat(src)
					m = (stat.S_IMODE(st.st_mode) & 0o777 & ~0o666) | (fmode & 0o666)
					if stat.S_ISDIR(st.st_mode):
						m |= DMDIR
				except OSError:
					pass
			if os.path.isdir(src):
				if rec:
					os.makedirs(os.path.join(self.dest, n), exist_ok=True)
					self.record(n, src, m, uid, gid, o)
					self.tree(src, n, rec, skip, o, uid, gid, fmode)
			else:
				self.copyfile(src, n, None)
				self.record(n, src, m, uid, gid, o)

	def run(self, path):
		# stack of (indent, new path, old path); options per level
		stack = [(-1, '', self.src)]
		opts = [(-1, {})]
		for indent, text, where in self.lines(path):
			while opts[-1][0] > indent:
				opts.pop()
			if '=' in text.split()[0]:
				k, v = text.split('=', 1)
				if opts[-1][0] < indent:
					opts.append((indent, dict(opts[-1][1])))
				elif opts[-1][0] > indent:
					continue
				if k == 'skip':
					opts[-1][1]['skip'] = re.compile(v)
				elif k in ('uid', 'gid'):
					opts[-1][1][k] = v
				elif k == 'mode':
					r = self.parsemode(v, opts[-1][1].get('mask', 0), opts[-1][1].get('mmode', 0))
					if r is None:
						warn('%s: bad mode specification %s' % (where, v))
					else:
						opts[-1][1]['mask'], opts[-1][1]['mmode'] = r
				else:
					warn('%s: bad option %s=%s' % (where, k, v))
				continue
			while stack[-1][0] >= indent:
				stack.pop()
			parent_new, parent_old = stack[-1][1], stack[-1][2]
			f = text.split()
			try:
				elem = self.word(f[0])
				fields = [self.word(x) for x in f[1:4]]
			except KeyError as e:
				warn("%s: can't read environment variable %s" % (where, e.args[0]))
				stack.append((indent, None, None))
				continue
			if parent_new is None:	# below a skipped line
				stack.append((indent, None, None))
				continue
			mode = self.mode(fields[0]) if fields else None
			skip = opts[-1][1].get('skip')
			if elem in ('+', '*'):
				if os.path.isdir(parent_old):
					self.tree(parent_old, parent_new, elem == '+', skip, opts[-1][1],
						fields[1] if len(fields) > 1 else None, fields[2] if len(fields) > 2 else None,
						self.fullmode(fields[0]) if fields else None)
				continue
			new = os.path.normpath(os.path.join(parent_new, elem)).lstrip('/')
			if len(f) > 4 and f[4] != '-':
				# an absolute source is in Plan 9's namespace: srcroot here
				old = os.path.join(self.src, f[4].lstrip('/')) if f[4].startswith('/') else f[4]
			else:
				old = os.path.join(parent_old, elem)
			stack.append((indent, new, old))
			if not os.path.exists(old):
				# a directory by its mode (d775): git keeps no empty ones
				if new in self.empty or (len(f) > 1 and self.word(f[1]).startswith('d')):
					os.makedirs(os.path.join(self.dest, new), exist_ok=True)
					self.record(new, old, self.fullmode(fields[0]) if fields else None,
						fields[1] if len(fields) > 1 else None, fields[2] if len(fields) > 2 else None, opts[-1][1])
				else:
					warn("%s: can't stat file %s" % (where, old))
					stack[-1] = (indent, None, None)
				continue
			self.copyfile(old, new, mode)
			self.record(new, old, self.fullmode(fields[0]) if fields else None,
				fields[1] if len(fields) > 1 else None, fields[2] if len(fields) > 2 else None, opts[-1][1])


def main():
	args = sys.argv[1:]
	src, dest, empty, protos, listing = '/', None, [], [], None
	while args:
		a = args.pop(0)
		if a == '-s':
			src = args.pop(0)
		elif a == '-d':
			dest = args.pop(0)
		elif a == '-l':
			listing = args.pop(0)
		elif a == '-e':
			empty.append(args.pop(0))
		else:
			protos.append(a)
	if dest is None or not protos:
		sys.exit('usage: proto.py [-s srcroot] -d dest [-e emptydir]... proto...')
	p = Proto(src, dest, empty)
	for path in protos:
		p.run(path)
	if listing:
		with open(listing, 'w') as f:
			for n in sorted(p.meta):
				if n in ('', '.'):
					continue
				m, u, g = p.meta[n]
				f.write('%s\t%o\t%s\t%s\n' % (n, m, u, g))


if __name__ == '__main__':
	main()
