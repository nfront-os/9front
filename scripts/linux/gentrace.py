#!/usr/bin/env python3
"""gentrace.py: empty trace points for sys/src/linux.

Linux's trace point headers (i915_trace.h, drm_trace.h, ...) are GPL;
without tracing every trace_X() is a no-op.  This writes our own
replace/include/<header> for each, with a no-op for each trace_X the
vendor code calls (and trace_X_enabled), so a new Linux version needs
only a rerun."""
import os, re, glob

HERE = os.path.dirname(os.path.abspath(__file__))
D = os.path.normpath(os.path.join(HERE, '../../sys/src/linux'))
HEADERS = ['i915_trace.h', 'intel_display_trace.h', 'display/intel_display_trace.h', 'drm_trace.h']

names = set()
for f in glob.glob(os.path.join(D, 'vendor/**/*.[ch]'), recursive=True):
	names |= set(re.findall(r'\btrace_([a-z0-9_]+)\s*\(', open(f, errors='replace').read()))
names = sorted(n[:-len('_enabled')] if n.endswith('_enabled') else n for n in names)
names = sorted(set(names))
for h in HEADERS:
	g = '_KPI_TRACE_' + re.sub(r'\W', '_', h.upper())
	out = ['/* SPDX-License-Identifier: MIT */',
		'/* LinuxKPI for 9front: %s, made by scripts/linux/gentrace.py: no tracing */' % h,
		'#ifndef ' + g, '#define ' + g, '']
	for n in names:
		out.append('#define trace_%s(...)\tdo { } while (0)' % n)
		out.append('#define trace_%s_enabled()\t0' % n)
	out += ['', '#endif', '']
	p = os.path.join(D, 'replace/include', h)
	os.makedirs(os.path.dirname(p), exist_ok=True)
	open(p, 'w').write('\n'.join(out))
print('%d trace points in %d headers' % (len(names), len(HEADERS)))
