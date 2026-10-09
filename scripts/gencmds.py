#!/usr/bin/env python3
"""
Generate gcc build-tool manifests (cmds.json) for every program
directory that sys/src/mkfile's install walks besides the libraries
($SUBSYS $CMDS: ape, /acme, cmd, games), from 9front's mkfiles.

mkfile kinds, following /sys/src/cmd/mk{one,many,lib,syslib}:
  mkone      one target TARG from OFILES (+ local LIB archives)
  mkmany     each TARG from TARG.c + OFILES; `$O.t: objects' rules
             (ndb's dns) override the object list
  mklib,     a program-local archive (LIB=libfoo.a$O), built first
  mksyslib
  sys/src/cmd/mkfile itself: every cmd/*.c is a one-file command
APE (sys/src/ape, and programs whose mkfile sets CC=pcc or reads
$APE/config) compiles like pcc (manifest key Ape: only the ape include
directories) and links APE's libraries; libap is one archive from
several directories (Append, see APEDIRS).
YFILES run through scripts/yacc.sh (Plan 9 yacc with 9front's yaccpar);
generators needed by a few directories are listed in PRE.

Install rules are followed as mk would (mkinstalls): a program that a
local $BIN/x rule copies elsewhere goes there (ip's telnet: /$objtype/bin),
programs only the install rule names are built too (vmx: vmxgdb), and
the files it installs by recipe -- rc scripts (git: %.rcinstall),
wrappers (replica), sed-made scripts (postscript) -- get a last manifest
entry running scripts/mkinstall.py. scripts/check-install.py checks the
result against the tree, the staged distribution and the CD.

Only targets that build are written to the manifests. `--probe' builds
every target on its own with the build tool and records the ones that
fail in scripts/cmds-broken.txt (target, reason); without it, the
broken list from the last probe is used. Probing needs the libraries
(scripts/build-cli.sh) and $CLI_DIR/build.exe.

usage: scripts/gencmds.py [--probe] [dir...]   (dirs relative to sys/src)
       scripts/gencmds.py --list               manifests in build order
"""
import json
import os
import re
import subprocess
import sys

import genclean

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
SRC = os.path.join(ROOT, 'sys', 'src')
BROKEN = os.path.join(ROOT, 'scripts', 'cmds-broken.txt')
CLI_DIR = os.environ.get('CLI_DIR', os.path.join(
	os.environ.get('XDG_CACHE_HOME', os.path.expanduser('~/.cache')), '9front-cli'))

# linked as one group (--start-group ... --end-group): 8l rescans the
# libraries until nothing is undefined, GNU ld would scan each archive
# once and miss mutual references (libc crypt.c -> libsec key_setup,
# libmemdraw -> libdraw chantodepth); unused archives cost nothing
LIBS = ['libdisk', 'libcontrol', 'libhtml', 'libframe', 'libdraw', 'libmemlayer',
	'libmemdraw', 'libplumb', 'libcomplete', 'lib9p', 'libventi', 'libsunrpc',
	'libthread', 'libauth', 'libauthsrv', 'libndb', 'libip', 'libhttpd', 'libsec',
	'libmp', 'libflate', 'libregexp', 'libString', 'libmach', 'libbin',
	'libavl', 'libjson', 'libgeometry', 'libpcm', 'libttf', 'libsat', 'libscribble',
	'liblex', 'libstdio', 'libdtracy', 'libaml', 'libfis', 'libbio', 'libc']

# per-architecture directories of other machines
ARCHS = ['386', '68000', '68020', 'arm', 'arm64', 'mips', 'mips2', 'power', 'power64',
	'sparc', 'sparc64', 'spim', 'alpha', '3210', '29000']

# APE's libraries, linked as one group like LIBS (pcc: -lX is
# /$objtype/lib/ape/libX.a, libap always; the rest via #pragma lib)
APELIBS = ['libdraw', 'libplumb', 'libauth', 'libsec', 'libmp', 'libnet', 'libbsd',
	'libl', 'libv', 'libz', 'libbz2', 'libbio', 'lib9', 'libfmt', 'libutf', 'libap']

# libap: ape/lib/ap/mkfile builds `$DIRS $objtype' into one archive; the
# first directory starts it, the others add to it (later ones replace
# members of the same name: stdio/strtod over gen/strtod, as with ar)
APEDIRS = ['ape/lib/ap/gen', 'ape/lib/ap/math', 'ape/lib/ap/plan9', 'ape/lib/ap/posix',
	'ape/lib/ap/stdio', 'ape/lib/ap/syscall', 'ape/lib/ap/amd64']
# sources outside the mkfile's OFILES: system call stubs (genall),
# the note trampolines of the gcc port
APEEXTRA = {'ape/lib/ap/amd64': ['notify.S']}

# directories whose mkfile runs a generator before compiling
PRE = {
	'cmd/awk':		['gen-awk-proctab.sh'],
	'cmd/aux/kbdfs':	['gen-latin1-h.sh'],
	'cmd/jpg':		['gen-jpg-tables.sh'],
	'cmd/upas/smtp':	['gen-upas-yacc.sh'],
	'cmd/upas/send':	['gen-upas-yacc.sh'],
	'cmd/upas/ml':		['gen-upas-yacc.sh'],
}
# directories whose mkfile rules gen-src.sh replicates
for d in ('cmd/ip/snoopy', 'cmd/syscall', 'cmd/tcs', 'cmd/nusb/disk', 'games/blabs', 'cmd/gs'):
	PRE[d] = ['gen-src.sh ' + d]
# the compilers' and loaders' enam.c (cc/mkfile.port, ?l/mkfile)
for d in sorted(os.listdir(os.path.join(SRC, 'cmd'))):
	if re.match(r'^[0-9a-z][cl]$', d) and d != 'rc':
		PRE['cmd/' + d] = ['gen-src.sh cmd/' + d]
# not target programs: compilers and assemblers, test data, the build tool
SKIP = re.compile(r'^(cmd/build|.*/test|ape/lib/(mp|sec)/amd64)(/|$)')	# the build tool, test data, unported asm

def rel(path):
	return os.path.relpath(path, SRC)

STDMK = re.compile(r'^/(\$objtype|amd64)/mkfile$|^/sys/src/cmd/mk(one|many|lib|syslib|dirs)$')

def uncomment(l):
	# # outside rc quotes (gs: `{sed 's#^./obj/...#'})
	q = False
	for i, c in enumerate(l):
		if c == "'":
			q = not q
		elif c == '#' and not q:
			return l[:i] + ('\\' if l.endswith('\\') else '')
	return l

def readmk(d, name='mkfile'):
	"""the mkfile with its local includes (<../mkupas) inlined"""
	path = os.path.join(SRC, d, name)
	# a comment ends at its line; mk still joins a trailing \ (htmlroff's #	t9.$O\);
	# recipe lines go to rc as they are (replica: echo '#!/bin/rc')
	lines, cont = [], False
	for l in open(path).read().split('\n'):
		if not (l.startswith('\t') and not cont):
			l = uncomment(l)
		cont = l.endswith('\\')
		lines.append(l)
	text = '\n'.join(lines)
	text = text.replace('\\\n', ' ')
	def inc(m):
		f = m.group(1).strip()
		if STDMK.match(f) or '$' in f.replace('$objtype', ''):
			return m.group(0)
		full = os.path.join(ROOT, f.lstrip('/')) if f.startswith('/') else os.path.join(os.path.dirname(path), f)
		if not os.path.isfile(full):
			return m.group(0)
		return readmk(os.path.relpath(os.path.dirname(full), SRC), os.path.basename(full))
	return re.sub(r'^<\s*(\S+)\s*$', inc, text, flags=re.M)

def rules(text):
	"""[(targets, prerequisites, recipe text or '')] of the mkfile's rules"""
	out = []
	lines = text.split('\n')
	for i, l in enumerate(lines):
		# ${X:%=$BIN/%} (replica's target) holds : and =; hidden while matching
		subst = re.findall(r'\$\{[^}]*\}', l)
		m = re.match(r'^([^\t=:<][^:=]*?):(?![=])\s*(.*)$', re.sub(r'\$\{[^}]*\}', '\x00', l))
		if not m or l.startswith('\t'):
			continue
		def unhide(t):
			return re.sub('\x00', lambda _: subst.pop(0), t)
		targets, prereq = unhide(m.group(1)), unhide(m.group(2))
		recipe = []
		for r in lines[i+1:]:
			if not r.startswith('\t'):
				break
			recipe.append(r.strip())
		out.append((targets.split(), prereq, '\n'.join(recipe)))
	return out

def rcquote(w):
	return "'" + w.replace("'", "''") + "'" if re.search(r"[\s']", w) or w == '' else w

def rcwords(s):
	"""split like rc: '...' quotes (with '' a quote), blanks separate"""
	out, w, q, i, word = [], '', False, 0, False
	while i < len(s):
		c = s[i]
		if q:
			if c == "'":
				if s[i+1:i+2] == "'":
					w += "'"
					i += 1
				else:
					q = False
			else:
				w += c
		elif c == "'":
			q = word = True
		elif c in ' \t\n':
			if word:
				out.append(w)
			w, word = '', False
		else:
			w += c
			word = True
		i += 1
	if word:
		out.append(w)
	return out

def expand(s, v):
	def sub(m):
		name, pat = m.group(1) or m.group(3), m.group(2)
		words = v.get(name, [])
		if pat and '=' in pat:
			a, b = pat.split('=', 1)
			pa, pb = a.split('%', 1) if '%' in a else (a, '')
			ba, bb = b.split('%', 1) if '%' in b else (b, '')
			out = []
			for w in words:
				if w.startswith(pa) and w.endswith(pb) and len(w) >= len(pa) + len(pb):
					out.append(ba + w[len(pa):len(w) - len(pb)] + bb)
				else:
					out.append(w)
			words = out
		return ' '.join(rcquote(w) for w in words)
	for _ in range(5):
		s = re.sub(r'\$\{(\w+)(?::([^}]*))?\}|\$(\w+)', sub, s)
	return s

def isape(text):
	# <$APE/config (CC=pcc, APEBIN), or CC=pcc of its own (grap, pic)
	# or native cc with APE's headers first (antiword: -I/sys/include/ape)
	return bool(re.search(r'^<\s*/?\$APE/config|^CC\s*=\s*pcc\b|^CFLAGS\s*=.*-I/sys/include/ape', text, re.M))

def variables(text):
	v = {'objtype': ['amd64'], 'O': ['6'], 'cputype': ['amd64'], 'CPUS': ['amd64']}
	if isape(text):	# sys/src/ape/config
		v.update({'APE': ['/sys/src/ape'], 'APEBIN': ['/amd64/bin/ape'], 'FAMILY': ['plan9'],
			'APELIB': ['/rc/bin/ape'], 'CC': ['pcc'], 'CFLAGS': ['-Fw']})
	for m in re.finditer(r'^(\w+)\s*=(.*)$', text, re.M):
		# rc quoting: FS='64', FS='', -D'PROGRAMNAME="troff, Plan 9 edition"'
		# `{...} (doom's `{~ $objtype mips && echo -D__BIG_ENDIAN__}) is not run
		val = re.sub(r'`\{[^}]*\}', '', m.group(2))
		v[m.group(1)] = [w for w in rcwords(expand(val, v)) if w != '']
	return v

# variables whose `{...} the port runs as mk does (read-only commands;
# the others are not run, see variables())
EVAL = {
	'cmd/gs':	['OFILES'],	# `{sed 's#^./obj/(.*)\.o .*#obj/\1.$O#' src/ld.tr | sort}
}
P9P = os.environ.get('PLAN9PORT', os.path.join(ROOT, '..', 'plan9port'))

def evalvars(text, v, d):
	out = {}
	for name in EVAL.get(d, []):
		m = re.search(r'^%s\s*=(.*)$' % name, text, re.M)
		def run(b):
			r = subprocess.run([os.path.join(P9P, 'bin', 'rc'), '-c', b.group(1)], cwd=os.path.join(SRC, d),
				env=dict(os.environ, PLAN9=P9P, PATH=os.path.join(P9P, 'bin') + ':' + os.environ['PATH'],
				objtype='amd64', O='6', cputype='amd64'),
				capture_output=True, text=True, check=True)
			return ' '.join(r.stdout.split())
		out[name] = [w for w in rcwords(expand(re.sub(r'`\{([^}]*)\}', run, m.group(1)), v)) if w]
	return out

def kind(text):
	m = re.search(r'^<\s*/sys/src/cmd/(mkone|mkmany|mklib|mksyslib)\s*$', text, re.M)
	return m.group(1) if m else None

def gasport(f, d):
	# the GAS port (foo.S) instead of 9front's 6a source (foo.s) beside it
	if f.endswith('.s') and os.path.exists(os.path.join(SRC, d, f[:-2] + '.S')):
		return f[:-2] + '.S'
	return f

def objs2src(objs, d, text='', v={}):
	# explicit (eui.$O: ../eui.c) and meta (%.$O: ../%.c) object rules
	explicit, meta = {}, []
	for targets, prereq, recipe in rules(text):
		ts = expand(' '.join(targets), v).split()
		ps = [p for p in expand(prereq, v).split() if p.endswith(('.c', '.s', '.S'))]
		if not ps or not recipe:	# scat.$O: strings.c is a dependency only
			continue
		for t in ts:
			if t.endswith('%.6'):	# %.$O, gs: obj/%.$O: src/%.c
				meta.append((t[:-3], tree(ps[0], d)))
			elif t.endswith('.6') and '%' not in t:
				explicit[t[:-2]] = tree(ps[0], d)
	out = []
	for o in objs:
		if o.endswith(('.c', '.s', '.S')):
			out.append(gasport(o, d))
			continue
		base = o[:-2] if o.endswith('.6') else o
		if base == 'y.tab':
			out.append('y.tab.c')
			continue
		if base in explicit:
			out.append(gasport(explicit[base], d))
			continue
		# the GAS port (.S) before the 6a original (.s)
		for ext in ('.c', '.S', '.s'):
			if os.path.exists(os.path.join(SRC, d, base + ext)):
				out.append(base + ext)
				break
		else:
			for pre, m in meta:
				if not base.startswith(pre):
					continue
				f = m.replace('%', base[len(pre):])
				if os.path.exists(os.path.join(SRC, d, f)) or made(f, d, text, v):
					out.append(gasport(f, d))
					break
			else:
				out.append(base + '.c')	# generated (awk's proctab.c, ...)
	return out

def made(f, d, text, v):
	# a rule of the mkfile makes f from what exists (APE libsec:
	# ../../../../libsec/port/%.c: ../../../../libsec/port/%.mp)
	r = findrule(rules(text), v, f)
	return bool(r and r[1] and all(os.path.exists(os.path.join(SRC, d, p)) for p in r[1]))

def tree(path, d):
	# absolute 9front paths (/sys/src/cmd/map) relative to sys/src/<d>
	if path.startswith('/sys/'):
		return os.path.relpath(os.path.join(ROOT, path[1:]), os.path.join(SRC, d))
	return path

def cflags(v, d):
	return ['-I' + tree(f[2:], d) if f.startswith('-I') else f
		for f in v.get('CFLAGS', []) if f.startswith(('-I', '-D'))]

def locallibs(v, d):
	# LIB may also name plain objects (vac's error.$O ...): those are sources
	# and archives of other source dirs (bzfs: /sys/src/cmd/bzip2/lib/libbzip2.a$O)
	return [os.path.normpath(os.path.join('/sys/src', d, l)) for l in v.get('LIB', [])
		if (not l.startswith('/') or l.startswith('/sys/src/')) and not l.endswith('.6')]

def pragmalibs(cf, d):
	"""#pragma lib of local headers (map.h: /sys/src/cmd/map/libmap/libmap.a$O);
	gcc ignores the pragma, 8l links what it names"""
	out = []
	for inc in ['.'] + [f[2:] for f in cf if f.startswith('-I')]:
		p = os.path.join(SRC, d, inc)
		if not os.path.isdir(p):
			continue
		for h in sorted(os.listdir(p)):
			if h.endswith('.h'):
				for m in re.finditer(r'^#pragma\s+lib\s+"(/sys/src/[^"]+)"', open(os.path.join(p, h), errors='replace').read(), re.M):
					out.append(m.group(1).replace('$O', '6'))
	return out

def libobjs(v):
	return [l for l in v.get('LIB', []) if not l.startswith('/') and l.endswith('.6')]

def dirlibs(text, v, d, common):
	"""archives of LIB that a rule of this mkfile builds (httpd's libhttps.a$O)"""
	out = []
	for l in v.get('LIB', []):
		if '/' in l or l.endswith('.6'):
			continue
		for targets, prereq, recipe in rules(text):
			if recipe and l in expand(' '.join(targets), v).split():
				objs = [o for o in expand(prereq, v).split() if o.endswith('.6')]
				if not objs:	# cmd/auth: $LIB: cd lib; mk (a mklib directory)
					break
				out.append((os.path.join(d, l), dict(common, Libs=[], library=l, libdir=d,
					src=objs2src(objs, d, text, v))))
				break
	return out

def up(d):
	# from sys/src/<d> back to the tree root (/acme: ../../acme/...)
	return os.path.relpath(ROOT, os.path.join(SRC, d)) + '/'

def nine(d):
	"""directory d as a 9front path (/sys/src/cmd/git, /acme/bin/source)"""
	return '/' + os.path.relpath(os.path.normpath(os.path.join(SRC, d)), ROOT)

def yaccpre(text, v, d):
	y = v.get('YFILES', [])
	if not y:
		return []
	m = re.search(r'^[^\n:]*:\S*\s*\$YFILES\s*\n\t\$YACC (.*)$', text, re.M)
	args = expand(m.group(1), dict(v, YFLAGS=v.get('YFLAGS', ['-d']), prereq=y)) if m else ' '.join(v.get('YFLAGS', ['-d']) + y)
	out = ['bash %sscripts/yacc.sh %s' % (up(d), args)]
	# grap.c: y.tab.c	mv $prereq $target
	for targets, prereq, recipe in rules(text):
		if prereq.split() == ['y.tab.c'] and re.match(r'(mv|cp)\b', recipe):
			out += ['%s y.tab.c %s' % (recipe.split()[0], t) for t in targets if t.endswith('.c')]
	return out

def lexpre(text, v, d):
	# grapl.c: $LFILES	$LEX -t $prereq > $target
	out = []
	for targets, prereq, recipe in rules(text):
		if re.match(r'\$LEX\s+-t\b', recipe):
			for t in targets:
				out.append('bash %sscripts/lex.sh --out %s -t %s' % (up(d), t, expand(prereq, v)))
	return out

def install(v, d):
	b = (v.get('BIN') or ['/amd64/bin'])[0]
	b = os.path.normpath(os.path.join(nine(d), b))	# acme: BIN=../$objtype
	return b.replace('/amd64', '/$ARCH', 1).rstrip('/') + '/'

def attrless(prereq):
	# `install:V:	$BIN/status': rules() leaves the attributes in front
	return re.sub(r'^[A-Za-z<]*:\s*', '', prereq)

def findrule(rs, v, t):
	"""the rule with a recipe that makes t, explicit before meta:
	(recipe, prerequisites, stem) or None"""
	for targets, prereq, recipe in rs:
		if recipe and t in expand(' '.join(targets), v).split():
			return recipe, expand(attrless(prereq), v).split(), ''
	for targets, prereq, recipe in rs:
		for pat in expand(' '.join(targets), v).split():
			if not recipe or '%' not in pat:
				continue
			a, b = pat.split('%', 1)
			if t.startswith(a) and t.endswith(b) and len(t) > len(a) + len(b):
				stem = t[len(a):len(t) - len(b)]
				return recipe, [p.replace('%', stem) for p in expand(attrless(prereq), v).split()], stem
	return None

def cpdest(recipe, v, target, prereq, stem):
	"""where the recipe's cp puts its file (cp $prereq /$objtype/bin/telnet)"""
	for l in recipe.split('\n'):
		m = re.match(r'^cp\s+(?:-\w+\s+)*\S+\s+(\S+)\s*$', l.strip())
		if m:
			return expand(m.group(1), dict(v, target=[target], prereq=prereq, stem=[stem]))
	return None

def mkinstalls(text, v, d):
	"""what the mkfile's install rule installs besides mkone/mkmany's
	$BIN/target: [(target, rule)] for the files its prerequisites name
	(auth: $BIN/status), through the virtual rules of the mkfile (ape/cmd:
	install.rc), and for the `for (i in $RC) mk $i.rcinstall' loops of
	its recipes (git); files of /tmp (postscript's $MAN1DIR) are left out"""
	rs = rules(text)
	virtual = set()
	for targets, prereq, recipe in rs:
		if re.match(r'^[A-Za-z<]*V[A-Za-z<]*:', prereq):
			virtual.update(expand(' '.join(targets), v).split())
	progs = set(v.get('TARG', []))
	out, seen, queue = [], set(), [('install', False)]
	while queue:
		n, leaf = queue.pop(0)
		if n in seen:
			continue
		seen.add(n)
		if leaf:
			r = findrule(rs, v, n)
			if r:
				out.append((n, r))
			elif os.path.basename(n) not in progs:	# programs: mkmany's $BIN/%
				print('gencmds: %s: no rule makes %s' % (d, n), file=sys.stderr)
			continue
		for targets, prereq, recipe in rs:
			if n not in expand(' '.join(targets), v).split():
				continue
			for p in expand(attrless(prereq), v).split():
				if p.startswith('/') and not p.startswith('/tmp/'):
					queue.append((p, True))
				elif p in virtual:
					queue.append((p, False))
			for m in re.finditer(r'for\s*\(\s*i\s+in\s+\$(\w+)\s*\)\s*mk\s+(?:\$MKFLAGS\s+)?\$i\.(\w+)', recipe):
				if m.group(2) != 'install':	# programs: mkmany's %.install
					queue += [(w + '.' + m.group(2), True) for w in v.get(m.group(1), [])]
	return out

def installfiles(d):
	"""[(target, recipe, prerequisites, stem, dest)] of the files (not
	programs) the install rule of directory d puts in place: rc scripts,
	wrappers -- scripts/mkinstall.py runs their recipes"""
	if d == 'cmd' or not os.path.isfile(os.path.join(SRC, d, 'mkfile')):
		return []
	text = readmk(d)
	v = variables(text)
	out = []
	for t, (recipe, prereq, stem) in mkinstalls(text, v, d):
		if any(p.startswith('6.') for p in prereq):
			continue	# a program: entries()
		dest = t if t.startswith('/') else cpdest(recipe, v, t, prereq, stem)
		out.append((t, recipe, prereq, stem, dest))
	return out

def entries(d):
	"""[(target name, description)] for directory d, installed where the
	mkfile's install rule puts them"""
	es = mkentries(d)
	if d == 'cmd' or not os.path.isfile(os.path.join(SRC, d, 'mkfile')):
		return es
	text = readmk(d)
	v = variables(text)
	rs = rules(text)
	bindir = install(v, d).replace('/$ARCH', '/amd64', 1)
	progs = {e.get('program') or os.path.basename(n): e for n, e in es if 'library' not in e and 'script' not in e}
	# $BIN/telnet:V:	$O.telnet
	#	cp $prereq /$objtype/bin/telnet
	for t, e in progs.items():
		r = findrule(rs, v, bindir + t)
		if r and '6.' + t in r[1]:
			dest = cpdest(r[0], v, bindir + t, r[1], r[2])
			if dest and dest != bindir + t:
				e['Install'] = os.path.dirname(dest).replace('/amd64', '/$ARCH', 1) + '/'
	# programs only the install rule names (vmx: $BIN/vmxgdb: $O.vmxgdb)
	common = dict(es[-1][1]) if es else {}
	for t, (recipe, prereq, stem) in mkinstalls(text, v, d):
		ps = [p[2:] for p in prereq if p.startswith('6.')]
		if not ps or ps[0] in progs:
			continue
		link = findrule(rs, v, '6.' + ps[0])
		dest = cpdest(recipe, v, t, prereq, stem) or t
		if not link:
			print('gencmds: %s: no rule links 6.%s' % (d, ps[0]), file=sys.stderr)
			continue
		objs = [o for o in link[1] if o.endswith(('.6', '.c'))]
		e = {k: common[k] for k in ('Cflags', 'Libs', 'Pre', 'ape') if k in common}
		e.update(program=ps[0], Install=os.path.dirname(dest).replace('/amd64', '/$ARCH', 1) + '/',
			src=objs2src(objs, d, text, v))
		es.append((d + '/' + ps[0], e))
	if installfiles(d):
		es.append((d + '/install', {'install': d, 'Install': ''}))
	return es

def mkentries(d):
	"""[(target name, description)] for directory d"""
	if d == 'cmd':
		fs = sorted(os.listdir(os.path.join(SRC, d)))
		ys = [f[:-2] for f in fs if f.endswith('.y')]
		# %.c %.tab.h: %.y	yacc -d -o $stem.c -s $stem $stem.y
		# $BIN/init: $O.init	cp $O.init /$objtype/init
		return [('cmd/init', {'single': 'init.c', 'Install': '/$ARCH/'})] + \
			[('cmd/' + f[:-2], {'single': f, 'Install': '/$ARCH/bin/'})
			for f in fs if f.endswith('.c') and f[:-2] not in ys + ['init']] + \
			[('cmd/' + y, {'program': y, 'src': [y + '.c'], 'Install': '/$ARCH/bin/',
			'Pre': ['bash ../../../scripts/yacc.sh -d -o %s.c -s %s %s.y' % (y, y, y)]}) for y in ys]
	if d == 'ape/lib/ap/syscall':
		# genall: a stub per sys.h entry not starting with _
		sysh = open(os.path.join(SRC, 'libc/9syscall/sys.h')).read()
		stubs = ['_%s.S' % n for n in re.findall(r'^#define\s+([A-Z0-9][A-Z0-9_]*)\s', sysh, re.M)]
		return [(d + '/libap.a', {'Cflags': [], 'Libs': [], 'Install': '', 'ape': True,
			'Pre': ['bash %sscripts/gen-9syscall.sh --ape .' % up(d)], 'library': 'libap.a',
			'libinstall': '/$ARCH/lib.gcc/ape/', 'append': True, 'src': stubs})]
	text = readmk(d)
	k = kind(text)
	if k is None:
		return []
	v = variables(text)
	v.update(evalvars(text, v, d))
	common = {'Cflags': cflags(v, d), 'Libs': locallibs(v, d) + pragmalibs(cflags(v, d), d), 'Install': install(v, d),
		'Pre': yaccpre(text, v, d) + lexpre(text, v, d) + ['bash %sscripts/%s' % (up(d), s) for s in PRE.get(d, [])],
		'ape': isape(text)}
	if k in ('mklib', 'mksyslib'):
		if not v.get('LIB'):
			return []
		ofiles = v.get('OFILES', [])
		if re.search(r'^OFILES\s*=\s*`\{rc \./reduce', text, re.M):
			# ape/lib/ap/gen/reduce: ALLOFILES minus what ../$objtype
			# has as .c/.s (grep -v ^name, a prefix match)
			md = os.path.join(SRC, d, '..', 'amd64')
			drop = [f[:-2] for f in os.listdir(md) if f.endswith(('.c', '.s'))]
			ofiles = [o for o in v.get('ALLOFILES', []) if not any(o.startswith(x) for x in drop)]
		elif re.search(r'^OFILES\s*=\s*`\{.*/port/reduce', text, re.M):
			# APE libmp, libsec: reduce against the native lib's amd64
			# assembly, which the port does not use (as genlibs.py)
			ofiles = v.get('ALLOFILES', [])
		src = objs2src(ofiles, d, text, v) + APEEXTRA.get(d, [])
		if v['LIB'][0].startswith('/'):	# mksyslib: LIB=/$objtype/lib/ape/libap.a
			L = v['LIB'][0]
			return [(d + '/' + os.path.basename(L), dict(common, library=os.path.basename(L),
				libinstall=os.path.dirname(L).replace('/amd64/lib', '/$ARCH/lib.gcc', 1) + '/',	# gcc archives beside the native /$ARCH/lib
				append=d in APEDIRS[1:], src=src))]
		lib = os.path.normpath(os.path.join(d, v['LIB'][0].replace('$O', '6')))
		return [(lib, dict(common, library=os.path.basename(lib), libdir=os.path.dirname(lib),
			src=src))]
	if k == 'mkone':
		if not v.get('TARG'):
			return []
		t = v['TARG'][0]
		if '/' in t:	# disk/smart: TARG=disk/smart, BIN=/$objtype/bin
			common['Install'] += os.path.dirname(t) + '/'
			t = os.path.basename(t)
		return dirlibs(text, v, d, common) + [(d + '/' + t, dict(common, program=t,
			src=objs2src(v.get('OFILES', []) + libobjs(v), d, text, v)))]
	out = dirlibs(text, v, d, common)
	rs = rules(text)
	for t in v.get('TARG', []):
		# mk: a rule with a recipe replaces the meta-rule's prerequisites
		# (ndb's dns), one without adds to them ($O.4s $O.5s: xs.$O)
		objs, extra, script, pcc = None, [], None, False
		for targets, prereq, recipe in rs:
			if '6.' + t in expand(' '.join(targets), v).split():
				pcc = pcc or bool(re.match(r'pcc\b', recipe))
				pp = expand(prereq, v).split()
				p = [o for o in pp if o.endswith(('.6', '.c'))]
				rc = [o for o in pp if o.endswith('.rc')]
				if recipe and rc and not p:
					script = rc[0]
				if recipe:
					objs = p
				else:
					extra += p
		if script:	# $O.mk9660: mk9660.rc	cp mk9660.rc $target
			out.append((d + '/' + t, dict(common, script=script, program=t)))
			continue
		if objs == []:
			continue
		if objs is None:
			objs = [t + '.6'] + v.get('OFILES', []) + extra
		objs += libobjs(v)
		src = objs2src(list(dict.fromkeys(objs)), d, text, v)
		if pcc:	# lp: $O.lpdsend: lpdsend.$O	pcc -o $target ...
			fl = []
			for targets, prereq, recipe in rs:
				if any(o in expand(' '.join(targets), v).split() for o in objs) and recipe.startswith('pcc'):
					fl += [f.replace("'", '') for f in recipe.split() if f.startswith(("-D", "-I", "-'D"))]
			out.append((d + '/' + t, dict(common, ape=True, Cflags=list(dict.fromkeys(fl)),
				program=t, src=src)))
			continue
		if len(src) == 1 and not common['Pre'] and not common['Libs']:
			out.append((d + '/' + t, dict(common, single=src[0])))
		else:
			out.append((d + '/' + t, dict(common, program=t, src=src)))
	return out

def manifest(e, name, d):
	"""the build tool's manifest entry, with Clean: what its Pre steps
	(genclean.py) or the install rule's recipes generate in d"""
	m = buildentry(e, name)
	c = genclean.clean(m.get('Pre', []), os.path.join(SRC, d))
	if 'install' in e:
		c += [f for f in installtemps(d) if f not in c
			and not genclean.tracked(os.path.normpath(os.path.join(SRC, d, f)))]
	if c:
		m['Clean'] = c
	return m

def installtemps(d):
	"""local files the install rule's recipes make on the way
	(postscript: cropmarks from cropmarks.rc; mkinstall.py's make)"""
	text = readmk(d)
	v = variables(text)
	rs = rules(text)
	out = []
	def walk(f, depth):
		r = findrule(rs, v, f)
		if r is None or depth > 8 or f in out:
			return
		out.append(f)
		for p in r[1]:
			if not p.startswith('/'):
				walk(p, depth + 1)
	for t, recipe, prereq, stem, dest in installfiles(d):
		for p in prereq:
			if not p.startswith('/'):
				walk(p, 0)
	return out

def buildentry(e, name):
	name = re.sub(r'^(\.\./)+', '', name)	# /acme: ../../acme/...
	if 'install' in e:	# the files of the install rule, by its recipes
		return {'Name': name.replace('/', '-'), 'Install': '',
			'Post': ['python3 %sscripts/mkinstall.py %s' % (up(e['install']), e['install'])]}
	m = {'Name': name.replace('/', '-'), 'Include': ['/$ARCH/cflags.json'], 'Install': e['Install']}
	if e.get('ape'):	# kencc's USED/SET intrinsics (native: u.h)
		m['Include'].append('/$ARCH/ape.json')
	if e.get('Cflags'):
		m['Cflags'] = e['Cflags']
	if e.get('Pre'):
		m['Pre'] = e['Pre']
	if e.get('ape'):
		m['Ape'] = True
	if 'library' in e:
		m.update({'Install': e.get('libinstall') or '/sys/src/%s/' % e['libdir'],
			'Library': e['library'], 'SourceFiles': e['src']})
		if e.get('append'):
			m['Append'] = True
		return m
	if 'script' in e:
		# rc script installed as the program (venti's conf gets no header)
		m.pop('Include')
		m['Post'] = ['install -D -m 755 %s %s%s' % (e['script'], e['Install'], e['program'])]
		m['Install'] = ''
		return m
	m['Oflags'] = ['--entry=_main']
	syslibs = ['/$ARCH/lib.gcc/ape/%s.a' % l for l in APELIBS] if e.get('ape') else \
		['/$ARCH/lib.gcc/%s.a' % l for l in LIBS]
	m['Libs'] = ['--start-group'] + [l.replace('$O', '6') for l in e.get('Libs', [])] + \
		syslibs + ['--end-group']
	if 'single' in e:
		m['SourceFilesCmd'] = [e['single']]
	else:
		m['Program'] = e['program']
		m['SourceFiles'] = e['src']
	return m

def subdirs(d):
	"""the subdirectories mk descends into: DIRS of the mkfile (disk: sacfs
	commented out), else the list another loop walks (postscript:
	for (i in $TARGETS)) or the directories of `@{cd x; mk $target}'
	recipes (/acme); cmd computes DIRS as every */mkfile but $NOMK"""
	text = readmk(d)
	v = variables(text)
	m = re.search(r'^DIRS\s*=(.*)$', text, re.M)
	if m and '`' not in m.group(1):
		# plus library dirs that rules build (auth: $LIB:	cd lib; mk)
		libs = [s for s in sorted(os.listdir(os.path.join(SRC, d)))
			if s not in ARCHS and os.path.isfile(os.path.join(SRC, d, s, 'mkfile'))
			and kind(readmk(os.path.join(d, s))) in ('mklib', 'mksyslib')]
		ds = expand(m.group(1), v).split()
		if re.search(r'for\s*\(\s*i\s+in\s+\$DIRS\s+\$objtype\s*\)', text):
			ds.append('amd64')	# ape/lib/ap: for(i in $DIRS $objtype)
		return ds + [l for l in libs if l not in ds]
	ds = []
	for m in re.finditer(r'for\s*\(\s*i\s+in\s+\$(\w+)\s*\)\s*@\{[^}]*?\bcd\s+\$i\b', text):
		if m.group(1) != 'DIRS':
			ds += [s for s in v.get(m.group(1), []) if s not in ds]
	ds += [m.group(1) for m in re.finditer(r'@\{\s*cd\s+([^;\s$]+)\s*;\s*mk\s+\$target\s*\}', text)]
	if ds:
		return ds
	return [s for s in sorted(os.listdir(os.path.join(SRC, d))) if s != 'test']

def topdirs():
	"""what sys/src/mkfile's install walks besides the libraries:
	$SUBSYS $CMDS (ape, /acme, cmd, games), relative to sys/src"""
	v = variables(readmk('.'))
	return [os.path.relpath(os.path.join(ROOT, t[1:]), SRC) if t.startswith('/') else t
		for t in v['SUBSYS'] + v['CMDS']]

def dirs(args):
	if args:
		return args
	out = []
	def walk(d):
		if SKIP.match(d) or not os.path.isfile(os.path.join(SRC, d, 'mkfile')):
			return
		out.append(d)
		for s in subdirs(d):
			walk(os.path.normpath(os.path.join(d, s)))
	for t in topdirs():
		walk(t)
	return out

def loadbroken():
	b = {}
	if os.path.exists(BROKEN):
		for l in open(BROKEN):
			if l.strip() and not l.startswith('#'):
				t, _, why = l.rstrip('\n').partition('\t')
				b[t] = why
	return b

def probe(d, name, e):
	"""build one target on its own; '' or the first error"""
	m = manifest(e, name, d)
	if 'Library' not in m:
		m['Install'] = '/tests/.probe/'
	path = os.path.join(SRC, d, '.probe.json')
	open(path, 'w').write(json.dumps([m]))
	env = dict(os.environ, PLAN9=ROOT, ARCH='amd64', CPUS='4', CC='gcc', TOOLPREFIX='', TOOLPFX='')
	try:
		r = subprocess.run([os.path.join(CLI_DIR, 'build.exe'), '.probe.json'],
			cwd=os.path.join(SRC, d), env=env, capture_output=True, text=True, timeout=900)
		out, rc = r.stdout + r.stderr, r.returncode
	except subprocess.TimeoutExpired:
		out, rc = 'timeout', 1
	os.remove(path)
	if rc == 0:
		return ''
	for pat in (r'error: (.*)', r'(undefinierter Verweis auf [^\n]*)', r'(undefined reference[^\n]*)',
			r'(fatal error: [^\n]*)', r'(exit status \d+)'):
		mm = re.search(pat, out)
		if mm:
			return mm.group(1)[:160]
	return out.strip().split('\n')[-1][:160] or 'failed'

def buildorder():
	"""the cmds.json manifests, directories holding archives first"""
	libs, rest = [], []
	for d in dirs([]):
		f = os.path.join(SRC, d, 'cmds.json')
		if os.path.exists(f):
			(libs if any('Library' in m for m in json.load(open(f))) else rest).append(rel(f))
	return [os.path.relpath(os.path.join(SRC, f), ROOT) for f in libs + rest]

def main():
	args = sys.argv[1:]
	if args == ['--list']:
		print('\n'.join(buildorder()))
		return
	doprobe = '--probe' in args
	args = [a for a in args if a != '--probe']
	# a full probe starts afresh (targets renamed or no longer walked drop out)
	broken = {} if doprobe and not args else loadbroken()
	os.makedirs(os.path.join(ROOT, 'tests', '.probe'), exist_ok=True)
	total = ok = 0
	alldirs = [(d, entries(d)) for d in dirs(args)]
	alldirs = [(d, es) for d, es in alldirs if es]
	result = {}
	# program-local archives first (auth/lib builds ../lib.6.a for cmd/auth)
	for libpass in (True, False):
		for d, es in alldirs:
			for name, e in es:
				if ('library' in e) != libpass:
					continue
				total += 1
				if doprobe:
					why = probe(d, name, e)
					if why:
						broken[name] = why
					else:
						broken.pop(name, None)
					print('%-4s %s %s' % ('ok' if not why else 'FAIL', name, why), flush=True)
				if name not in broken:
					ok += 1
					result.setdefault(d, []).append((name, e))
	for d, es in alldirs:
		good = result.get(d, [])
		out = os.path.join(SRC, d, 'cmds.json')
		if not good:
			if os.path.exists(out):
				os.remove(out)
			continue
		ms = [manifest(e, n, d) for n, e in good if 'library' in e]
		# singles share a manifest per install directory (cmd: init goes to /$objtype)
		groups = {}
		for n, e in good:
			if 'single' in e:
				groups.setdefault(e['Install'], []).append(e)
		for i, (inst, es) in enumerate(sorted(groups.items(), key=lambda g: -len(g[1]))):
			m = manifest(es[0], d + '/cmds' + ('' if i == 0 else '-' + str(i)), d)
			m['SourceFilesCmd'] = [e['single'] for e in es]
			ms.append(m)
		ms += [manifest(e, n, d) for n, e in good if 'program' in e]
		ms += [manifest(e, n, d) for n, e in good if 'install' in e]	# after the programs (git/: $BIN)
		open(out, 'w').write(json.dumps(ms, indent='\t') + '\n')
	if not args:	# manifests of directories mk does not build (disk/sacfs)
		walked = set(d for d, es in alldirs)
		for top in topdirs():
			for dp, dn, fn in os.walk(os.path.join(SRC, top)):
				if 'cmds.json' in fn and rel(dp) not in walked:
					os.remove(os.path.join(dp, 'cmds.json'))
	if doprobe:
		with open(BROKEN, 'w') as f:
			f.write('# targets that do not build yet (scripts/gencmds.py --probe): target<TAB>first error\n')
			for t in sorted(broken):
				f.write('%s\t%s\n' % (t, broken[t]))
	print('%d of %d targets build, %d broken (%s)' % (ok, total, total - ok, os.path.relpath(BROKEN, ROOT)))

if __name__ == '__main__':
	main()
