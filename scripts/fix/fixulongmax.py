#!/usr/bin/env python3
"""
GCC port: rewrite `x != ~0UL` / `x == ~0UL` comparisons to `(ulong)~0UL`.

Plan 9's ulong is 32 bits on amd64 (amd64/include/u.h), and 6c's UL
suffix matches it. Under gcc (LP64) a UL literal is 64 bits, so a
32-bit ulong compared against ~0UL is promoted and never equal to
0xffffffffffffffff: `!= ~0UL` is always true, `== ~0UL` always false.
Assignments (`x = ~0UL`) and argument passing truncate correctly and
are left alone; ~0ULL (vlong/uvlong operands) is not touched.

usage: scripts/fix/fixulongmax.py file.c...
"""
import re
import sys

PAT = re.compile(r'([!=]=)(\s*)~0UL\b(?!L)')
NOTE = '\t/* GCC port: ~0UL is 64-bit, ulong is 32-bit */'

for path in sys.argv[1:]:
	src = open(path).read().split('\n')
	n = 0
	for i, line in enumerate(src):
		new = PAT.sub(r'\1\2(ulong)~0UL', line)
		if new != line:
			if 'GCC port: ~0UL' not in new:
				new += NOTE
			src[i] = new
			n += 1
	if n:
		open(path, 'w').write('\n'.join(src))
		print('%s: %d' % (path, n))
