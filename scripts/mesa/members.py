#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
#
# members.py MESABUILD NM SO...: the objects Mesa's ICDs (the .so targets
# of the meson build in MESABUILD) are made of, in link order, each once:
# the objects of the link lines, then the members of their static
# libraries. 9front links them all into one relocatable object (the
# drivers' entry point tables refer to their functions weakly, which
# takes nothing out of an archive), so what a static link would leave out
# has to go here: some sources are in two libraries (blorp and blorp_elk,
# Vulkan's instance and lite_instance, isl per hardware generation), and
# a later object whose strong symbols an earlier one defines already is
# dropped, as the first definition in link order is the one a link takes.
# An object that only partly overlaps is an error. The two can differ in
# what they were compiled with (vk_instance.c: the lite instance of
# venus, without the GLSL types, the full one of hasvk): the drivers'
# order decides which stays (hasvk's: the full one serves venus too),
# and each such pair is listed on standard error.
# Prints the objects' paths (relative to MESABUILD), one per line.

import shlex
import subprocess
import sys


def ninja(*args):
	return subprocess.run(['ninja', '-C', MESA] + list(args), check=True,
		capture_output=True, text=True).stdout


MESA = NM = None


def inputs(lib):
	objs = []
	sect = False
	for line in ninja('-t', 'query', lib).split('\n'):
		s = line.strip()
		if s.startswith('input:'):
			sect = True
		elif s.startswith('outputs:'):
			sect = False
		elif sect and s.endswith('.o'):
			objs.append(s)
	return objs


def strong(obj):
	"""the strong global symbols obj defines"""
	out = subprocess.run([NM, '-g', '--defined-only', obj], cwd=MESA, check=True,
		capture_output=True, text=True).stdout
	syms = set()
	for line in out.split('\n'):
		f = line.split()
		# 386: gcc's __x86.get_pc_thunk.* are in every PIC object, each in
		# a COMDAT group of its own: the link keeps one, no overlap
		if len(f) == 3 and f[1] not in ('W', 'V', 'w', 'v', 'U', 'i') and not f[2].startswith('__x86.get_pc_thunk.'):
			syms.add(f[2])
	return syms


def defines(cmd):
	return sorted(w for w in shlex.split(cmd) if w.startswith('-D'))


def members(mesa, nm, sos):
	"""the objects, paths relative to mesa"""
	global MESA, NM
	MESA, NM = mesa, nm
	order = []
	for so in sos:
		words = shlex.split(ninja('-t', 'commands', so).strip().split('\n')[-1])
		order += [w for w in words if w.endswith('.o')]
		for lib in dict.fromkeys(w for w in words if w.endswith('.a') and not w.startswith('/')):
			order += inputs(lib)
	order = list(dict.fromkeys(order))
	import json, os
	cc = {c['output']: c['command'] for c in json.load(open(os.path.join(MESA, 'compile_commands.json')))}
	defined = {}
	keep = []
	bad = []
	for o in order:
		s = strong(o)
		dup = [x for x in s if x in defined]
		if s and len(dup) == len(s):
			first = defined[dup[0]]
			if o in cc and first in cc and defines(cc[o]) != defines(cc[first]):
				d = sorted(set(defines(cc[o])) ^ set(defines(cc[first])))
				print('members: %s stands for %s (differ: %s)' % (first, o, ' '.join(d[:6])), file=sys.stderr)
			continue	# the same again: the link takes the first
		if dup:
			bad.append('%s: %s also in %s' % (o, ' '.join(sorted(dup)[:4]), defined[dup[0]]))
			continue
		for x in s:
			defined[x] = o
		keep.append(o)
	if bad:
		sys.exit('members: objects that overlap only partly:\n\t' + '\n\t'.join(bad))
	return keep


if __name__ == '__main__':
	print('\n'.join(members(sys.argv[1], sys.argv[2], sys.argv[3:])))
