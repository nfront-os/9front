#!/usr/bin/env python3
"""import-openbsd.py [OPENBSDTREE]: OpenBSD's LinuxKPI into sys/src/linux/openbsd.

The template of our LinuxKPI (kpi/): OpenBSD's drm LinuxKPI (the headers
under sys/dev/pci/drm/include that are OpenBSD's own, drm_linux.c,
linux_*.c) and the BSD headers they build on (sys/tree.h, sys/queue.h,
subr_tree.c), taken from the git tree OPENBSDTREE (default ~/Devel/openbsd)
at the commit in sys/src/linux/OPENBSD: the last state before OpenBSD's
update to Linux 6.12, so the one that fits vendor/'s Linux 6.6.  Copied
unchanged, under OpenBSD's paths; a file whose notice is not permissive
(ISC, BSD, MIT, public domain) is left out and listed.  Our adapted
copies live in kpi/; `diff -r` against this tree shows the adaptation.
Writes openbsd/NOTICE (each distinct notice once, with its files)."""
import os, re, shutil, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
D = os.path.normpath(os.path.join(HERE, '../../sys/src/linux'))
OB = os.path.expanduser(sys.argv[1] if len(sys.argv) > 1 else '~/Devel/openbsd')
COMMIT = open(os.path.join(D, 'OPENBSD')).read().split()[0]

DRM = 'sys/dev/pci/drm/'
# what we take: OpenBSD's own parts of its drm tree (not the Linux files
# it carries: those we have from Linux itself, in vendor/)
WANT = [
	DRM + 'include/linux/', DRM + 'include/asm/', DRM + 'include/acpi/',
	DRM + 'include/generated/', DRM + 'include/media/', DRM + 'include/video/',
	DRM + 'include/xen/', DRM + 'include/trace/',
	DRM + 'drm_linux.c', DRM + 'linux_list_sort.c', DRM + 'linux_radix.c',
	DRM + 'linux_sort.c', DRM + 'linux_sort_r.c', DRM + 'files.drm',
	# OpenBSD's own versions of Linux files that are GPL (our REJECTED)
	DRM + 'drm_managed.c', DRM + 'drm_gem_atomic_helper.c', DRM + 'drm_gem_framebuffer_helper.c',
	DRM + 'drm_gem_ttm_helper.c', DRM + 'drm_trace.h', DRM + 'display/drm_hdcp_helper.c',
	DRM + 'i915/i915_trace.h', DRM + 'i915/display/intel_display_trace.h',
	DRM + 'amd/amdgpu/amdgpu_vkms.c', DRM + 'amd/amdgpu/amdgpu_vkms.h',
	DRM + 'include/drm/drm_accel.h', DRM + 'include/drm/drm_gem_atomic_helper.h',
	DRM + 'include/drm/drm_gem_ttm_helper.h', DRM + 'include/drm/drm_lease.h',
	DRM + 'include/drm/drm_managed.h', DRM + 'include/drm/drm_mipi_dsi.h',
	DRM + 'include/drm/drm_of.h', DRM + 'include/drm/drm_simple_kms_helper.h',
	DRM + 'include/drm/drm_sysfs.h', DRM + 'include/drm/drm_writeback.h',
	DRM + 'include/drm/intel/i915_hdcp_interface.h', DRM + 'include/drm/intel/intel-gtt.h',
	DRM + 'include/sound/pcm.h', DRM + 'include/uapi/linux/media-bus-format.h',
	'sys/sys/tree.h', 'sys/sys/queue.h', 'sys/kern/subr_tree.c',
]

OK = re.compile(r'Public domain|Permission to use, copy, modify|Redistribution and use in source and binary'
	r'|Permission is hereby granted|SPDX-License-Identifier:[^\n]*\b(MIT|BSD|ISC|X11)\b', re.I)

def git(*a):
	return subprocess.run(['git', '-C', OB] + list(a), capture_output=True, check=True).stdout

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
	dst = os.path.join(D, 'openbsd')
	if os.path.exists(dst):
		shutil.rmtree(dst)
	files = git('ls-tree', '-r', '--name-only', COMMIT, '--', *WANT).decode().split('\n')
	taken, rejected, notices = [], [], {}
	for f in filter(None, files):
		data = git('show', '%s:%s' % (COMMIT, f))
		t = data.decode(errors='replace')
		n = notice(t)
		if f.endswith('files.drm') or not t.strip():
			n = ''	# the build list; empty stub headers
		elif not OK.search(n):
			rejected.append(f)
			continue
		p = os.path.join(dst, f)
		os.makedirs(os.path.dirname(p), exist_ok=True)
		open(p, 'wb').write(data)
		taken.append(f)
		if n:
			notices.setdefault(n, []).append(f)
	with open(os.path.join(dst, 'NOTICE'), 'w') as o:
		o.write('sys/src/linux/openbsd holds files of OpenBSD (git %s), unchanged, each\n'
			'under its own notice; they follow, each once, with the files they cover.\n'
			'Our LinuxKPI (sys/src/linux/kpi) is derived from them and keeps the\n'
			'notices in the derived files.\n\n' % COMMIT)
		for n, fs in sorted(notices.items(), key=lambda x: x[1][0]):
			o.write('==== ' + '\n==== '.join(fs) + '\n' + n + '\n\n')
	print('%d files of OpenBSD %s, %d rejected%s' % (len(taken), COMMIT[:11], len(rejected),
		''.join('\n  rejected: ' + r for r in rejected)))

main()
