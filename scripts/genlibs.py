#!/usr/bin/env python3
"""
Generate userspace library manifests (sys/src/lib<name>/lib<name>.json)
for the gcc build tool (sys/src/cmd/build).

Two source-list origins, mirroring how 9front builds the library:
  - mkfile: the library's own mkfile OFILES list (x.$O -> x.c; an
    "$objtype.$O" entry -> amd64.c).
  - reduce: libraries built from port/ plus a per-arch dir (libsec,
    libmp): port/mkfile's CFILES or FILES list, minus files the arch
    dir replaces (9front's port/reduce). Only arch files ported to gcc
    (.c, .S) replace anything; an unported 6a .s keeps the port C
    version. (The kernel manifests <lib>/amd64.json drop those C
    fallbacks, e.g. libmp's mpvecsub, so they are not reused here.)

The userspace flags come from ../lib.json (amd64/cflags.json).

usage: scripts/genlibs.py [lib...]     (default: all libraries below)
       scripts/genlibs.py --list      print them in build order
"""
import json
import os
import re
import sys

import genclean

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
SRC = os.path.join(ROOT, 'sys', 'src')

# build order: libraries others depend on first; the rest of
# sys/src/mkfile's LIBS (libc excluded, see genlibc.py) follow
FIRST = ['libbio', 'libauth', 'libthread', 'lib9p', 'libregexp', 'libString',
	'libndb', 'libip', 'libflate', 'libauthsrv', 'libdisk', 'libsec', 'libmp']

def mklibs():
	text = open(os.path.join(SRC, 'mkfile')).read().replace('\\\n', ' ')
	m = re.search(r'^LIBS\s*=(.*)$', text, re.M)
	libs = [l for l in m.group(1).split() if l != 'libc']
	return FIRST + [l for l in libs if l not in FIRST]

LIBS = mklibs()

def kind(lib):
	# libraries built from port/ plus an arch dir use 9front's reduce
	return 'reduce' if os.path.exists(os.path.join(SRC, lib, 'port', 'mkfile')) else 'mkfile'

# generated sources (9front mkfile rules) -> generator run as Pre step
PRE = {
	'libsec':	'gen-mpc.sh',		# port/%.c from %.mp
	'libauthsrv':	'gen-mpc.sh',		# %.mpc from %.mp
}

def mkfile_sources(lib):
	text = open(os.path.join(SRC, lib, 'mkfile')).read()
	text = text.replace('\\\n', ' ')
	m = re.search(r'^OFILES\s*=(.*)$', text, re.M)
	if m is None:
		sys.exit('%s: no OFILES in mkfile' % lib)
	# expand mkfile variables used in OFILES ($PROTO in libsunrpc)
	defs = dict((d.group(1), d.group(2).split()) for d in re.finditer(r'^(\w+)\s*=(.*)$', text, re.M))
	words = []
	for o in m.group(1).split():
		v = re.fullmatch(r'\$\{?(\w+)\}?', o)
		words += defs.get(v.group(1), [o]) if v and v.group(1) != 'objtype' else [o]
	files = []
	for o in words:
		o = o.replace('$objtype', 'amd64').replace('${objtype}', 'amd64')
		if not o.endswith('.$O'):
			sys.exit('%s: unexpected OFILES entry %r' % (lib, o))
		c = o[:-len('.$O')] + '.c'
		if not os.path.exists(os.path.join(SRC, lib, c)):
			sys.exit('%s: %s not found' % (lib, c))
		files.append(c)
	return files

def mkincludes(lib):
	# CFLAGS=$CFLAGS -I/sys/src/cmd -> -I../cmd (manifests run in the lib dir)
	text = open(os.path.join(SRC, lib, 'mkfile')).read()
	inc = []
	for m in re.finditer(r'^CFLAGS\s*=.*$', text, re.M):
		for d in re.findall(r'-I(\S+)', m.group(0)):
			if d.startswith('/'):
				d = os.path.relpath(os.path.join(ROOT, d.lstrip('/')), os.path.join(SRC, lib))
			inc.append('-I' + d)
	return inc

def reduce_sources(lib):
	text = open(os.path.join(SRC, lib, 'port', 'mkfile')).read()
	text = text.replace('\\\n', ' ')
	m = re.search(r'^CFILES\s*=(.*)$', text, re.M)
	if m is not None:
		names = [f[:-2] for f in m.group(1).split()]
	else:
		m = re.search(r'^FILES\s*=(.*)$', text, re.M)
		if m is None:
			sys.exit('%s: no CFILES/FILES in port/mkfile' % lib)
		names = m.group(1).split()
	archdir = os.path.join(SRC, lib, 'amd64')
	ported = {}
	for f in os.listdir(archdir):
		base, ext = os.path.splitext(f)
		if ext in ('.c', '.S'):
			ported[base] = 'amd64/' + f
	files = []
	for n in names:
		if n in ported:
			files.append(ported.pop(n))
		else:
			# port/%.c: %.mp (gen-mpc.sh, a Pre step) need not exist yet
			p = os.path.join(SRC, lib, 'port', n)
			if not os.path.exists(p + '.c') and not os.path.exists(p + '.mp'):
				sys.exit('%s: port/%s.c not found' % (lib, n))
			files.append('port/' + n + '.c')
	return files + sorted(ported.values())

def main():
	if sys.argv[1:] == ['--list']:	# build order for scripts/build-cli.sh
		print(' '.join(LIBS))
		return
	libs = sys.argv[1:] or LIBS
	bad = 0
	for lib in libs:
		if lib not in LIBS:
			sys.exit('unknown library %s' % lib)
		try:
			files = mkfile_sources(lib) if kind(lib) == 'mkfile' else reduce_sources(lib)
		except SystemExit as e:
			print('%s: %s' % (lib, e), file=sys.stderr)
			bad = 1
			continue
		manifest = [{
			'Name': lib[3:].capitalize() if lib[3:].islower() else lib[3:],
			'Include': ['../lib.json'],
			'Install': '/$ARCH/lib.gcc/',	# gcc archives; /$ARCH/lib is the native (6c) one
			'Library': lib + '.a',
			'SourceFiles': files,
		}]
		inc = mkincludes(lib)
		if inc:
			manifest[0]['Cflags'] = inc
		if lib in PRE:
			manifest[0]['Pre'] = ['bash ../../../scripts/' + PRE[lib]]
			manifest[0]['Clean'] = genclean.clean(manifest[0]['Pre'], os.path.join(SRC, lib))
		out = os.path.join(SRC, lib, lib + '.json')
		open(out, 'w').write(json.dumps(manifest, indent='\t') + '\n')
		print('%s: %d files' % (os.path.relpath(out, ROOT), len(files)))

	sys.exit(bad)

main()
