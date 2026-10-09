"""
The files a manifest's Pre steps generate, for its Clean key (the build
tool's -clean removes them, as the clean rules of 9front's mkfiles do:
rm -f y.tab.? y.debug y.output ...). Used by genlibc.py, genlibs.py and
gencmds.py.

clean(pre, d): globs relative to d (an absolute source directory) for
the Pre commands pre. cp and mv targets are not listed: the build tool
knows those itself. Files the repository tracks are never listed (9l's
cnam.c is checked in although its mkfile has a rule for it).
"""
import glob
import os
import re
import subprocess

import arch

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
SRC = os.path.join(ROOT, 'sys', 'src')

def rel(path, d):
	return os.path.relpath(path, d)

def yacc(args):
	# Plan 9 yacc: y.tab.c (-o file), y.tab.h with -d (-s stem: stem.tab.[ch]),
	# y.debug, y.output with -v
	out = ['y.tab.c', 'y.tab.h', 'y.debug', 'y.output']
	for i, a in enumerate(args):
		if a == '-o' and i + 1 < len(args):
			out.append(args[i + 1])
		elif a == '-s' and i + 1 < len(args):
			out += [args[i + 1] + '.tab.c', args[i + 1] + '.tab.h']
	return out

def gensrc(sub, d):
	"""what scripts/gen-src.sh <sub> writes (its cases)"""
	if sub == 'cmd/ip/snoopy':
		return ['protos.h', 'protos.c']
	if sub == 'cmd/syscall':
		return ['tab.h']
	if sub == 'cmd/tcs':
		return ['alias.h']
	if sub == 'cmd/nusb/disk':
		return ['scsierrs.c']
	if sub == 'games/blabs':
		return ['*.bits']
	if sub == 'cmd/gs':
		return [arch.OBJTYPE + '.h']	# $objtype.h
	if re.match(r'^cmd/[0-9a-z]c$', sub):
		return ['enam.c']
	if re.match(r'^cmd/[0-9a-z]l$', sub):
		# gen-src.sh: rc ./mkcname if the mkfile has a cnam.c rule
		mk = open(os.path.join(d, 'mkfile')).read()
		return ['cnam.c'] if re.search(r'^cnam\.c:', mk, re.M) else []
	return []

def generator(name, args, d):
	if name == 'yacc.sh':
		return yacc(args)
	if name == 'lex.sh':	# --out file
		return [args[args.index('--out') + 1]] if '--out' in args else ['lex.yy.c']
	if name == 'gen-src.sh':
		return gensrc(args[0], d)
	if name == 'gen-awk-proctab.sh':
		return ['proctab.c']
	if name == 'gen-latin1-h.sh':
		return [rel(os.path.join(SRC, 'cmd/aux/kbdfs/latin1.h'), d)]
	if name == 'gen-jpg-tables.sh':
		return ['rgbv.h', 'ycbcr.h']
	if name == 'gen-upas-yacc.sh':
		smtp = os.path.join(SRC, 'cmd/upas/smtp')
		return [rel(os.path.join(smtp, f), d) for f in
			('smtpd.tab.c', 'rfc822.tab.c', 'rfc822.tab.h', 'y.debug', 'y.output', 'xxx')]
	if name == 'gen-9syscall.sh':	# a stub per sys.h entry
		return ['_*.S'] if '--ape' in args else ['*.S']
	if name == 'gen-mpc.sh':	# libsec/port/%.c, libauthsrv/%.mpc from %.mp
		out = []
		for lib, ext in (('libsec/port', '.c'), ('libauthsrv', '.mpc')):
			for mp in sorted(glob.glob(os.path.join(SRC, lib, '*.mp'))):
				out.append(rel(mp[:-3] + ext, d))
		return out
	if name == 'gen-runetype.sh':
		return [rel(os.path.join(SRC, 'libc/ucd', f), d) for f in
			('runenormdata', 'runetotypedata', 'runeistypedata', 'runebreakdata')]
	return []

_tracked = None

def tracked(path):
	global _tracked
	if _tracked is None:
		r = subprocess.run(['git', '-C', ROOT, 'ls-files'], capture_output=True, text=True, check=True)
		_tracked = set(os.path.join(ROOT, f) for f in r.stdout.split('\n') if f)
	return path in _tracked

def clean(pre, d):
	out = []
	for c in pre:
		w = c.split()
		if len(w) >= 2 and w[0] in ('bash', 'python3') and '/scripts/' in '/' + w[1]:
			for f in generator(os.path.basename(w[1]), w[2:], d):
				p = os.path.normpath(os.path.join(d, f))
				if f in out or tracked(p):
					continue
				if any(c in f for c in '*?[') and any(tracked(t) for t in glob.glob(p)):
					continue	# a glob over tracked files: not ours to remove
				out.append(f)
	return out
