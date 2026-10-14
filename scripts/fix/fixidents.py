#!/usr/bin/env python3
"""
Rename identifiers that use runes gcc does not accept in identifiers.

kencc takes every rune >= 0x80 as an identifier character (galaxy's
fx÷❨m₁m₂❩, upas' ∂); gcc in -std=gnu17 only takes the ranges of C11
Annex D. Each rejected rune of an identifier becomes _<name>_ with the
rune's hex code (÷ -> _f7_), so the rename is mechanical and the same in
every file. Strings, character constants and comments are left alone.
Each changed file gets one /* GCC port: */ note at the first changed line.

usage: scripts/fix/fixidents.py [-n] file...	(-n: only list)
"""
import re
import sys

# C11 Annex D.1: ranges of characters allowed in identifiers
D1 = [(0xA8,0xA8),(0xAA,0xAA),(0xAD,0xAD),(0xAF,0xAF),(0xB2,0xB5),(0xB7,0xBA),
	(0xBC,0xBE),(0xC0,0xD6),(0xD8,0xF6),(0xF8,0xFF),(0x100,0x167F),(0x1681,0x180D),
	(0x180F,0x1FFF),(0x200B,0x200D),(0x202A,0x202E),(0x203F,0x2040),(0x2054,0x2054),
	(0x2060,0x206F),(0x2070,0x218F),(0x2460,0x24FF),(0x2776,0x2793),(0x2C00,0x2DFF),
	(0x2E80,0x2FFF),(0x3004,0x3007),(0x3021,0x302F),(0x3031,0x303F),(0x3040,0xD7FF),
	(0xF900,0xFD3D),(0xFD40,0xFDCF),(0xFDF0,0xFE44),(0xFE47,0xFFFD),
	(0x10000,0x1FFFD),(0x20000,0x2FFFD),(0x30000,0x3FFFD),(0x40000,0x4FFFD),
	(0x50000,0x5FFFD),(0x60000,0x6FFFD),(0x70000,0x7FFFD),(0x80000,0x8FFFD),
	(0x90000,0x9FFFD),(0xA0000,0xAFFFD),(0xB0000,0xBFFFD),(0xC0000,0xCFFFD),
	(0xD0000,0xDFFFD),(0xE0000,0xEFFFD)]

def ok(c):
	o = ord(c)
	return o < 0x80 or any(a <= o <= b for a, b in D1)

IDENT = re.compile(r'[A-Za-z_\u0080-\U0010FFFF][A-Za-z0-9_\u0080-\U0010FFFF]*')
SKIP = re.compile(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'', re.S)
NOTE = '\t/* GCC port: runes gcc rejects in identifiers spelled _<hex>_ */'

def rename(m):
	w = m.group(0)
	if all(ok(c) for c in w):
		return w
	return ''.join(c if ok(c) else '_%x_' % ord(c) for c in w)

def fix(path, dry):
	text = open(path, encoding='utf-8').read()
	out, pos, names = [], 0, set()
	for m in SKIP.finditer(text):
		code = text[pos:m.start()]
		new = IDENT.sub(rename, code)
		if new != code:
			names.update(w for w in IDENT.findall(code) if not all(ok(c) for c in w))
		out.append(new + m.group(0))
		pos = m.end()
	code = text[pos:]
	new = IDENT.sub(rename, code)
	names.update(w for w in IDENT.findall(code) if not all(ok(c) for c in w))
	out.append(new)
	res = ''.join(out)
	if res == text:
		return
	print('%s: %s' % (path, ' '.join(sorted(names))))
	if dry:
		return
	a, b = text.split('\n'), res.split('\n')
	for i in range(len(a)):
		if a[i] != b[i]:
			b[i] += NOTE
			break
	open(path, 'w', encoding='utf-8').write('\n'.join(b))

dry = '-n' in sys.argv
for f in [a for a in sys.argv[1:] if a != '-n']:
	try:
		fix(f, dry)
	except UnicodeDecodeError:
		pass
