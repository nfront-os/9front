#!/usr/bin/env python3
"""
List lines where the port dropped a level of a member path relative to
9front (branch front): front has a.x.y, the port a.y (or a.x / &p->x,
the port a / &p) on an otherwise equal line. An earlier mechanical rewrite ("explicit Lock/QLock member
access") turned clunkq.q.head (the QLock's waiters) into clunkq.head (the
chan queue) and so changed the kernel's closeproc logic: closeprocs
never exited and filled the process table. Comments are ignored.

usage: scripts/check/find-shortened-members.py [path...]  (default: sys/src)
"""
import os
import re
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))

def code(s):
	return re.sub(r'\s+', '', re.sub(r'/\*.*?\*/|//.*', '', s))

def main():
	paths = sys.argv[1:] or ['sys/src']
	diff = subprocess.run(['git', 'diff', '-U0', 'front', '--'] + paths,
		cwd=ROOT, capture_output=True, text=True).stdout
	f, minus, plus = None, [], []
	bad = 0
	def flush():
		nonlocal bad
		for m, p in zip(minus, plus):
			if m.lstrip().startswith(('*', '/*')):
				continue
			# drop one member selector (.x or ->x), in the middle of a path
			# or at its end: front &mc->Mouse, port &mc (readmouse received
			# into the pointer variable instead of the embedded Mouse)
			for mm in re.finditer(r'(?:\.|->)\w+', m):
				short = m[:mm.start()] + m[mm.end():]
				if code(short) == code(p) and code(short) != code(m):
					print('%s: front: %s | port: %s' % (f, m.strip(), p.strip()))
					bad += 1
					break
	for line in diff.split('\n'):
		if line.startswith('+++ b/'):
			flush(); minus, plus = [], []
			f = line[6:]
		elif line.startswith('@@'):
			flush(); minus, plus = [], []
		elif line.startswith('-') and not line.startswith('---'):
			minus.append(line[1:])
		elif line.startswith('+') and not line.startswith('+++'):
			plus.append(line[1:])
	flush()
	sys.exit(1 if bad else 0)

main()
