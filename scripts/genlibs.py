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

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
SRC = os.path.join(ROOT, 'sys', 'src')

# library -> origin of its source list
LIBS = {
	'libbio':	'mkfile',
	'libauth':	'mkfile',
	'libthread':	'mkfile',
	'lib9p':	'mkfile',
	'libregexp':	'mkfile',
	'libString':	'mkfile',
	'libndb':	'mkfile',
	'libip':	'mkfile',
	'libflate':	'mkfile',
	'libauthsrv':	'mkfile',
	'libdisk':	'mkfile',
	'libsec':	'reduce',
	'libmp':	'reduce',
}

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
	files = []
	for o in m.group(1).split():
		o = o.replace('$objtype', 'amd64').replace('${objtype}', 'amd64')
		if not o.endswith('.$O'):
			sys.exit('%s: unexpected OFILES entry %r' % (lib, o))
		c = o[:-len('.$O')] + '.c'
		if not os.path.exists(os.path.join(SRC, lib, c)):
			sys.exit('%s: %s not found' % (lib, c))
		files.append(c)
	return files

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
			if not os.path.exists(os.path.join(SRC, lib, 'port', n + '.c')):
				sys.exit('%s: port/%s.c not found' % (lib, n))
			files.append('port/' + n + '.c')
	return files + sorted(ported.values())

def main():
	if sys.argv[1:] == ['--list']:	# build order for scripts/build-cli.sh
		print(' '.join(LIBS))
		return
	libs = sys.argv[1:] or sorted(LIBS)
	for lib in libs:
		if lib not in LIBS:
			sys.exit('unknown library %s' % lib)
		files = mkfile_sources(lib) if LIBS[lib] == 'mkfile' else reduce_sources(lib)
		manifest = [{
			'Name': lib[3:].capitalize() if lib[3:].islower() else lib[3:],
			'Include': ['../lib.json'],
			'Install': '/$ARCH/lib/',
			'Library': lib + '.a',
			'SourceFiles': files,
		}]
		if lib in PRE:
			manifest[0]['Pre'] = ['bash ../../../scripts/' + PRE[lib]]
		out = os.path.join(SRC, lib, lib + '.json')
		open(out, 'w').write(json.dumps(manifest, indent='\t') + '\n')
		print('%s: %d files' % (os.path.relpath(out, ROOT), len(files)))

main()
