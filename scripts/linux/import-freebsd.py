#!/usr/bin/env python3
"""import-freebsd.py [FREEBSDTREE [DRMKMODTREE]]: FreeBSD's LinuxKPI into sys/src/linux/freebsd.

The second template of our LinuxKPI (kpi/), beside OpenBSD's (openbsd/):
FreeBSD's sys/compat/linuxkpi/common (include, src) from the git tree
FREEBSDTREE (default ~/Devel/freebsd-src), and from drm-kmod (DRMKMODTREE,
default ~/Devel/drm-kmod) only the files that are its own and BSD:
linuxkpi/bsd and its versions of drm files Linux has under GPL
(drm_sysfs.c, drm_lease.c, ...).  Never drm-kmod's linuxkpi/gplv2 nor the
Linux files it carries.  Both at the commits in sys/src/linux/FREEBSD.
Copied unchanged, under the source's paths (freebsd/..., drm-kmod/...); a
file whose notice is not permissive is left out and listed.  Writes
freebsd/NOTICE (each distinct notice once, with its files)."""
import os, re, shutil, subprocess, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'lib'))
import paths as nfpaths	# the distribution's paths.env

HERE = os.path.dirname(os.path.abspath(__file__))
D = os.path.normpath(os.path.join(HERE, '../../sys/src/linux'))
TREES = [os.path.expanduser(sys.argv[1] if len(sys.argv) > 1 else nfpaths.get('FREEBSD_SRC')),
	os.path.expanduser(sys.argv[2] if len(sys.argv) > 2 else nfpaths.get('DRMKMOD_SRC'))]
COMMITS = [l.split()[0] for l in open(os.path.join(D, 'FREEBSD')) if l.strip()]
NAMES = ['freebsd', 'drm-kmod']
WANT = [
	['sys/compat/linuxkpi/common/include/', 'sys/compat/linuxkpi/common/src/'],
	['linuxkpi/bsd/', 'drivers/gpu/drm/drm_sysfs.c', 'drivers/gpu/drm/drm_lease.c',
	 'include/drm/drm_sysfs.h', 'drivers/gpu/drm/i915/intel_uncore_trace.h',
	 'drivers/dma-buf/dma-buf.c', 'drivers/dma-buf/dma-fence.c',
	 'drivers/dma-buf/dma-fence-array.c', 'drivers/dma-buf/dma-fence-chain.c'],
]

OK = re.compile(r'Public domain|Permission to use, copy, modify|Redistribution and use in source and binary'
	r'|Permission is hereby granted|SPDX-License-Identifier:[^\n]*\b(MIT|BSD|ISC|X11)\b', re.I)
GPL = re.compile(r'GPL|General Public License')

def git(tree, *a):
	return subprocess.run(['git', '-C', tree] + list(a), capture_output=True, check=True).stdout

def notice(text):
	"""the leading comments, up to the first code"""
	out, pos = [], 0
	for m in re.finditer(r'\s*(//[^\n]*|/\*.*?\*/)', text, re.S):
		if m.start() != pos:
			break
		out.append(m.group(1).strip())
		pos = m.end()
	return '\n'.join(out)

def main():
	dst = os.path.join(D, 'freebsd')
	if os.path.exists(dst):
		shutil.rmtree(dst)
	taken, rejected, notices = [], [], {}
	for tree, commit, name, want in zip(TREES, COMMITS, NAMES, WANT):
		files = git(tree, 'ls-tree', '-r', '--name-only', commit, '--', *want).decode().split('\n')
		for f in filter(None, files):
			data = git(tree, 'show', '%s:%s' % (commit, f))
			t = data.decode(errors='replace')
			n = notice(t)
			if not t.strip() or (not n and not GPL.search(t)):
				n = ''	# empty stubs, files without notice are FreeBSD's (BSD-2 tree)
			elif not OK.search(n) or GPL.search(n):
				rejected.append(name + '/' + f)
				continue
			p = os.path.join(dst, name, f)
			os.makedirs(os.path.dirname(p), exist_ok=True)
			open(p, 'wb').write(data)
			taken.append(name + '/' + f)
			if n:
				notices.setdefault(n, []).append(name + '/' + f)
	with open(os.path.join(dst, 'NOTICE'), 'w') as o:
		o.write('sys/src/linux/freebsd holds files of FreeBSD (freebsd/, git %s) and of\n'
			'drm-kmod (drm-kmod/, git %s, only its BSD parts), unchanged, each under\n'
			'its own notice; they follow, each once, with the files they cover.\n'
			'Our LinuxKPI (sys/src/linux/kpi) is derived from them and from OpenBSD\'s\n'
			'(openbsd/) and keeps the notices in the derived files.\n\n' % tuple(COMMITS))
		for n, fs in sorted(notices.items(), key=lambda x: x[1][0]):
			o.write('==== ' + '\n==== '.join(fs) + '\n' + n + '\n\n')
	print('%d files, %d rejected%s' % (len(taken), len(rejected),
		''.join('\n  rejected: ' + r for r in rejected)))

main()
