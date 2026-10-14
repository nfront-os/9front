#!/usr/bin/env python3
"""
Find explicit casts that rely on 8c's unnamed-member conversion.

Plan 9 C lets a pointer to a struct be converted to a pointer to the
type of an unnamed (embedded) member, and 8c/6c apply that conversion
even for an explicit cast: (Lock*)&sl yields &sl's Lock member. gcc's
-fplan9-extensions only converts implicitly (assignment, argument
passing); an explicit cast just reinterprets the address. That is
harmless while the member is the first one (offset 0), but when it is
not, the result points at the wrong field (libc syslog.c: lock() spun
forever on sl.fd == -1).

This lists every struct with an unnamed member that is not its first
member, then every explicit cast to one of those member types. Each hit
needs a look: replace the cast with &x->Type when x is such a struct.

usage: scripts/check/find-embedded-casts.py [dir...]   (default: sys/src)
"""
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
SKIP = re.compile(r'/cmd/(8c|6c|5c|7c|kc|qc|vc|cc|8a|6a|5a|7a|ka|qa|va)/')

def sources(dirs):
	for d in dirs:
		for dp, _, fs in os.walk(d):
			if SKIP.search(dp + '/'):
				continue
			for f in fs:
				if f.endswith(('.c', '.h')):
					yield os.path.join(dp, f)

STRUCT = re.compile(r'^(?:typedef\s+)?struct\s+(\w+)\s*\n?\{(.*?)^\}', re.M | re.S)
ANON = re.compile(r'^\s*([A-Z]\w*)\s*;', re.M)

def main():
	dirs = sys.argv[1:] or [os.path.join(ROOT, 'sys', 'src')]
	files = list(sources(dirs))
	embedded = {}		# member type -> [(struct, file)] where not first
	for f in files:
		text = open(f, errors='replace').read()
		for m in STRUCT.finditer(text):
			body = re.sub(r'/\*.*?\*/', '', m.group(2), flags=re.S)
			members = [l for l in body.split('\n') if l.strip() and not l.strip().startswith('#')]
			for i, l in enumerate(members):
				a = ANON.match(l)
				if a and i > 0:
					embedded.setdefault(a.group(1), []).append((m.group(1), os.path.relpath(f, ROOT)))
	cast = re.compile(r'\(\s*(%s)\s*\*\s*\)(?=\s*[&\w(*])' % '|'.join(map(re.escape, embedded)))
	for f in files:
		for n, l in enumerate(open(f, errors='replace'), 1):
			for c in cast.finditer(l):
				structs = ', '.join(sorted({s for s, _ in embedded[c.group(1)]}))
				print('%s:%d: (%s*) -- %s not first in: %s' % (
					os.path.relpath(f, ROOT), n, c.group(1), c.group(1), structs))
				print('\t' + l.strip())

main()
