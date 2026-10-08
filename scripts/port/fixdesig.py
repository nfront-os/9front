#!/usr/bin/env python3
"""
Add the `=' that kencc lets designated initializers omit:

	Srv fs = {
		.attach fsattach,	->	.attach = fsattach,
	.w 640, .h 400		->	.w = 640, .h = 400

gcc only knows the `=' form (and GNU's obsolete `[i] v' for arrays,
which it still accepts). The first changed line of each initializer
gets a /* GCC port: */ note.

usage: scripts/port/fixdesig.py [-n] file...	(-n: only list)
"""
import re
import sys

# a member designator not followed by `=': at line start, after `{' or `,'
DESIG = re.compile(r'(^\s*|[{,]\s*)\.([A-Za-z_]\w*)\s+(?![=\s])(?=\S)')
NOTE = '\t/* GCC port: kencc initializers may omit the = of .member */'

def fix(path, dry):
	text = open(path, encoding='utf-8', errors='surrogateescape').read()
	lines = text.split('\n')
	# comments and strings blanked, newlines kept: multi-line comments too
	blank = re.sub(r'/\*.*?\*/|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'',
		lambda m: re.sub(r'[^\n]', ' ', m.group(0)), text, flags=re.S).split('\n')
	changed = 0
	last = -2
	for i, l in enumerate(lines):
		code = blank[i]
		if not DESIG.search(code):
			continue
		# rewrite positions found in the comment/string-blanked copy
		out, pos = [], 0
		for m in DESIG.finditer(code):
			out.append(l[pos:m.end(2)] + ' =')
			pos = m.end(2)
		new = ''.join(out) + l[pos:]
		if i != last + 1 and '/*' not in new:
			new += NOTE
		if dry:
			print('%s:%d: %s' % (path, i + 1, new.strip()))
		lines[i] = new
		last = i
		changed += 1
	if changed and not dry:
		open(path, 'w', encoding='utf-8', errors='surrogateescape').write('\n'.join(lines))
	return changed

dry = '-n' in sys.argv
for f in [a for a in sys.argv[1:] if a != '-n']:
	n = fix(f, dry)
	if n:
		print('%s: %d lines' % (f, n), file=sys.stderr)
