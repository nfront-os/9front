#!/usr/bin/env python3
"""import.py [LINUXTREE]: Linux's DRM code into sys/src/linux/vendor.

Copies what sys/src/linux/IMPORT names from a Linux tree (default
~/Devel/linux-6.6), unchanged, under the same paths; leaves out every
file whose notice is not MIT-like (GPL only, or no notice: the kernel's
GPL then) and lists those in REJECTED; writes VERSION and NOTICE (each
distinct notice once, with the files it covers).  vendor/ is replaced
as a whole: the files there are Linux's, ours are elsewhere."""
import glob, os, re, shutil, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
D = os.path.normpath(os.path.join(HERE, '../../sys/src/linux'))
LX = os.path.expanduser(sys.argv[1] if len(sys.argv) > 1 else '~/Devel/linux-6.6')

OK = re.compile(r'SPDX-License-Identifier:[^\n]*\b(MIT|BSD|X11)\b|Permission is hereby granted|Permission to use, copy, modify')

def notice(text):
	"""the leading comments, up to the first code; past an include guard
	(#ifndef X / #define X) too, where some headers have theirs"""
	out, pos = [], 0
	for m in re.finditer(r'\s*(//[^\n]*|/\*.*?\*/|#ifndef\s+\w+\s*\n\s*#define\s+\w+[^\n]*)', text, re.S):
		if m.start() != pos:
			break
		if not m.group(1).startswith('#'):
			out.append(m.group(1).strip())
		pos = m.end()
	return '\n'.join(out)

def wanted():
	inc, exc = [], []
	for l in open(os.path.join(D, 'IMPORT')):
		l = l.strip()
		if not l or l.startswith('#'):
			continue
		(exc if l.startswith('!') else inc).append(l.lstrip('!'))
	files = set()
	for g in inc:
		files |= {os.path.relpath(f, LX) for f in glob.glob(os.path.join(LX, g), recursive=True) if os.path.isfile(f)}
	for g in exc:
		files -= {os.path.relpath(f, LX) for f in glob.glob(os.path.join(LX, g), recursive=True)}
	return sorted(files)

def main():
	vendor = os.path.join(D, 'vendor')
	if os.path.exists(vendor):
		shutil.rmtree(vendor)
	taken, rejected, notices = [], [], {}
	for f in wanted():
		t = open(os.path.join(LX, f), errors='replace').read()
		if os.path.basename(f) == 'Makefile':
			ok, n = True, None
		else:
			n = notice(t)
			ok = bool(OK.search(n))
		if not ok:
			spdx = re.search(r'SPDX-License-Identifier:\s*([^\n*]+)', n)
			rejected.append('%s\t%s' % (f, spdx.group(1).strip() if spdx else 'no notice'))
			continue
		dst = os.path.join(vendor, f)
		os.makedirs(os.path.dirname(dst), exist_ok=True)
		shutil.copy2(os.path.join(LX, f), dst)
		taken.append(f)
		if n:
			notices.setdefault(n, []).append(f)
	try:
		commit = subprocess.run(['git', '-C', LX, 'rev-parse', 'HEAD'], capture_output=True, text=True).stdout.strip()
	except OSError:
		commit = ''
	mk = open(os.path.join(LX, 'Makefile')).read()
	ver = '.'.join(re.search(r'^%s = (\d+)' % k, mk, re.M).group(1) for k in ('VERSION', 'PATCHLEVEL', 'SUBLEVEL'))
	open(os.path.join(D, 'VERSION'), 'w').write('linux-%s %s\n' % (ver, commit))
	open(os.path.join(D, 'REJECTED'), 'w').write(
		'# files IMPORT names that import.py left out: their notice is not MIT-like\n' + '\n'.join(rejected) + '\n')
	with open(os.path.join(D, 'NOTICE'), 'w') as o:
		o.write('sys/src/linux/vendor holds files of Linux %s, unchanged, each under its own\n'
			'notice; they follow, each once, with the files it covers.  Our own files\n'
			'(kpi/, replace/, the programs using them) are MIT.\n\n' % ver)
		for n, fs in sorted(notices.items(), key=lambda x: x[1][0]):
			o.write('==== ' + '\n==== '.join(fs) + '\n' + n + '\n\n')
	print('%d files of linux-%s, %d rejected, %d notices' % (len(taken), ver, len(rejected), len(notices)))

main()
