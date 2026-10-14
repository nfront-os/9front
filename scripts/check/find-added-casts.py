#!/usr/bin/env python3
"""
List casts the gcc port added relative to 9front (branch front).

Earlier mechanical port scripts inserted explicit casts such as
`rh = (Routehint*)c` where 9front relies on 8c's implicit conversion to
an unnamed member (`rh = c`). gcc's -fplan9-extensions performs that
implicit conversion correctly, but an explicit cast only reinterprets
the address -- wrong whenever the member is not the first one.

For every diff hunk line pair (front line removed, port line added)
where deleting the added casts from the port line gives back the front
line, this prints file, line, the cast types and both lines.

usage: scripts/check/find-added-casts.py [path...]   (default: sys/src)
"""
import os
import re
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
CAST = re.compile(r'\(\s*(?:struct\s+)?([A-Za-z_]\w*)\s*\*+\s*\)\s*')

def norm(s):
	return re.sub(r'\s+', '', s)

def strip_comment(s):
	return re.sub(r'/\*.*?\*/', '', s)

def main():
	paths = sys.argv[1:] or ['sys/src']
	diff = subprocess.run(['git', 'diff', '-U0', 'front', '--'] + paths,
		cwd=ROOT, capture_output=True, text=True).stdout
	f = None
	minus, plus = [], []
	def flush():
		for (ln, a), b in zip(plus, minus):
			pass
		# pair removed and added lines of a hunk in order
		for (pl, p), m in zip(plus, minus):
			pc = strip_comment(p)
			casts = CAST.findall(pc)
			if not casts:
				continue
			if norm(CAST.sub('', pc)) == norm(CAST.sub('', strip_comment(m)))\
			   and norm(pc) != norm(strip_comment(m)):
				# keep only casts not already in the front line
				added = [c for c in casts if '(%s*)' % c not in norm(m)]
				if added:
					print('%s:%d: (%s) | front: %s | port: %s' % (f, pl,
						','.join(sorted(set(added))), m.strip(), p.strip()))
	for line in diff.split('\n'):
		if line.startswith('+++ b/'):
			flush(); minus, plus = [], []
			f = line[6:]
		elif line.startswith('@@'):
			flush(); minus, plus = [], []
			m = re.match(r'@@ -\d+(?:,\d+)? \+(\d+)', line)
			ln = int(m.group(1))
		elif line.startswith('-') and not line.startswith('---'):
			minus.append(line[1:])
		elif line.startswith('+') and not line.startswith('+++'):
			plus.append((ln, line[1:]))
			ln += 1
	flush()

main()
