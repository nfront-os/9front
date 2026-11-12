#!/usr/bin/env python3
"""gentrace.py [LINUXTREE]: empty trace points for sys/src/linux.

Linux's trace point headers (i915_trace.h, drm_trace.h, ...) are GPL;
without tracing every trace_X() is a no-op.  This writes our own
replace/include/<header> for each, with a no-op for each trace_X the
vendor code calls (and trace_X_enabled), so a new Linux version needs
only a rerun."""
import os, re, glob, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'lib'))
import paths as nfpaths	# the distribution's paths.env

HERE = os.path.dirname(os.path.abspath(__file__))
D = os.path.normpath(os.path.join(HERE, '../../sys/src/linux'))
# our header -> Linux's, whose #include lines it repeats (other files
# count on what they bring along); the trace point code itself is not used
HEADERS = {
	'i915_trace.h': 'drivers/gpu/drm/i915/i915_trace.h',
	'intel_display_trace.h': 'drivers/gpu/drm/i915/display/intel_display_trace.h',
	'display/intel_display_trace.h': 'drivers/gpu/drm/i915/display/intel_display_trace.h',
	'drm_trace.h': 'drivers/gpu/drm/drm_trace.h',
	'intel_uncore_trace.h': 'drivers/gpu/drm/i915/intel_uncore_trace.h',	# Linux 6.10 on
}
LX = os.path.expanduser(sys.argv[1] if len(sys.argv) > 1 else nfpaths.get('LINUX'))

names, funcs = set(), set()
for f in glob.glob(os.path.join(D, 'vendor/**/*.[ch]'), recursive=True):
	t = open(f, errors='replace').read()
	names |= set(re.findall(r'\btrace_([a-z0-9_]+)\s*\(', t))
	# functions of the driver's own that are called trace_X (not trace points)
	funcs |= set(re.findall(r'^(?:[a-z][\w \*]*\s)?\**trace_([a-z0-9_]+)\s*\([^;]*$', t, re.M))
names -= funcs
names = sorted(n[:-len('_enabled')] if n.endswith('_enabled') else n for n in names)
names = sorted(set(names))
for h, lx in HEADERS.items():
	g = '_KPI_TRACE_' + re.sub(r'\W', '_', h.upper())
	out = ['/* SPDX-License-Identifier: MIT */',
		'/* LinuxKPI for 9front: %s, made by scripts/linux/gentrace.py: no tracing */' % h,
		'#ifndef ' + g, '#define ' + g, '']
	for inc in re.findall(r'^#include\s+([<"][^>"]+[>"])', open(os.path.join(LX, lx)).read(), re.M):
		if 'trace' in inc:
			continue
		rel = os.path.normpath(os.path.join(os.path.dirname(lx), inc[1:-1]))
		if inc[0] == '"' and os.path.exists(os.path.join(LX, rel)):	# beside Linux's header: from the tree's root (-I vendor)
			inc = '"%s"' % rel
		out.append('#include ' + inc)
	out.append('')
	for n in names:
		out.append('#define trace_%s(...)\tdo { } while (0)' % n)
		out.append('#define trace_%s_enabled()\t0' % n)
	out += ['', '#endif', '']
	p = os.path.join(D, 'replace/include', h)
	os.makedirs(os.path.dirname(p), exist_ok=True)
	open(p, 'w').write('\n'.join(out))
print('%d trace points in %d headers' % (len(names), len(HEADERS)))
