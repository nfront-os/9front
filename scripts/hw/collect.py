#!/usr/bin/env python3
"""Copy hwtest/ from the 9fat of a hardware test stick (or its image) and
print the summaries (tests/hw/README.md).

usage: scripts/hw/collect.py stick-or-image [destdir]
       scripts/hw/collect.py --offset stick-or-image
	(default destdir: tests/hw/results/<date>)
The 9fat lies in the Plan 9 partition (MBR type 0x39); its place is in
that partition's prep table (sector 1, text: "part 9fat start end").
Reads with mtools (mcopy) at that offset; writes nothing to the stick.
"""
import os, struct, subprocess, sys, time

SECT = 512


def fat_offset(path):
	with open(path, 'rb') as f:
		mbr = f.read(SECT)
		if mbr[510:512] != b'\x55\xaa':
			sys.exit('collect: %s: no MBR' % path)
		for i in range(4):
			e = mbr[446 + 16*i: 446 + 16*(i+1)]
			ptype = e[4]
			start = struct.unpack('<I', e[8:12])[0]
			if ptype == 0x39:
				f.seek((start + 1) * SECT)
				table = f.read(SECT).split(b'\0')[0].decode('latin1')
				for line in table.splitlines():
					w = line.split()
					if len(w) == 4 and w[0] == 'part' and w[1] == '9fat':
						return (start + int(w[2])) * SECT
				sys.exit('collect: no 9fat in the plan9 partition table')
	sys.exit('collect: %s: no plan9 partition' % path)


def main():
	if len(sys.argv) < 2:
		sys.exit(__doc__)
	if sys.argv[1] == '--offset':	# the byte offset of the 9fat, for mtools
		print(fat_offset(sys.argv[2]))
		return
	src = sys.argv[1]
	root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
	stamp = time.strftime('%Y-%m-%d-%H%M%S')
	if len(sys.argv) > 2:
		dest = sys.argv[2]
	elif os.path.isdir(os.path.join(root, 'tests/hw')):	# in the repo
		dest = os.path.join(root, 'tests/hw/results', stamp)
	else:	# stand-alone, next to the stick image
		dest = 'hwtest-' + stamp
	off = fat_offset(src)
	os.makedirs(dest, exist_ok=True)
	env = dict(os.environ, MTOOLS_SKIP_CHECK='1')
	r = subprocess.run(['mcopy', '-s', '-n', '-i', '%s@@%d' % (src, off), '::hwtest', dest], env=env)
	if r.returncode != 0:
		sys.exit('collect: mcopy failed')
	top = os.path.join(dest, 'hwtest')
	print('logs in', top)
	for d, _, files in sorted(os.walk(top)):
		if 'summary.txt' in files:
			print('==', os.path.relpath(d, top))
			with open(os.path.join(d, 'summary.txt'), errors='replace') as f:
				sys.stdout.write(f.read())


main()
