#!/usr/bin/env python3
"""
Rewrite Plan 9 absolute includes (#include "/sys/src/...") to paths
relative to the including file. In Plan 9 "/" is the root of this tree;
on the build host gcc would open the host's /. The original path stays
in a comment.

usage: scripts/port/fixabsincludes.py file...
"""
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))

for p in sys.argv[1:]:
	d = os.path.dirname(os.path.abspath(p))
	s = open(p).read()
	def sub(m):
		target = os.path.join(ROOT, m.group(1).lstrip('/'))
		return '#include "%s"\t/* GCC port: was "%s", a Plan 9 absolute path */' % (
			os.path.relpath(target, d), m.group(1))
	new, n = re.subn(r'#include\s*["<](/[^">]+)[">]', sub, s)
	if n:
		open(p, 'w').write(new)
		print('%s: %d' % (p, n))
