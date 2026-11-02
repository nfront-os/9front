#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
#
# vendor.py SRC BUILD ROOT: Mesa's Vulkan drivers (venus, hasvk, radv) as sources in the tree, to
# be built by build.exe (here with ../9front-gcc's cross gcc, on 9front with
# its native gcc), without meson, ninja or python (scripts/mesa/vendor.sh).
#
# From the meson build in BUILD (scripts/mesa/build.sh) it takes the objects
# the ICDs are made of (the links of libvulkan_virtio.so,
# libvulkan_intel_hasvk.so and libvulkan_radeon.so, the archives' members, each once: members.py),
# their sources and every header they read (ninja's dependency log), and the
# files meson's generators wrote (entry point tables, format tables, ...).
# It writes:
#   sys/src/mesa/vendor/...         the sources, headers and generated files,
#                                   under their paths in Mesa (a generated
#                                   file where meson's build directory has it)
#   sys/include/ape/vulkan,vk_video the Vulkan headers, for every APE program
#   .../mesa$SUFFIX.json            per directory, one block per meson library
#                                   and language: its flags, as meson's
#                                   compile commands have them, paths relative
#   sys/src/mesa/plan9/loader/vkcmds.[ch]   the loader's trampolines
#   sys/src/mesa/build$SUFFIX.json  all of it, the loader and libmesaplan9 as
#                                   one Whole libvulkan.a in /$ARCH/lib.gcc/ape
# Per objtype (amd64, 386: build-386, SUFFIX .386, as the tree's manifests);
# the sources are the same for all, a file that differs is an error.
# Files that come out the same keep their time (build.exe -D).

import json
import os
import shlex
import shutil
import subprocess
import sys

SRC, BUILD, ROOT = [os.path.realpath(a) for a in sys.argv[1:4]]
MESA = os.path.join(BUILD, 'mesa')
VENDOR = os.path.join(ROOT, 'sys/src/mesa/vendor')
APEINC = os.path.join(ROOT, 'sys/include/ape')
P9 = os.path.join(ROOT, 'sys/src/mesa/plan9')
OBJTYPE = os.environ.get('objtype', 'amd64')
SUFFIX = {'amd64': '', '386': '.386'}[OBJTYPE]
conflicts = []
PUBLIC = ('include/vulkan/', 'include/vk_video/')	# go to /sys/include/ape
PLAN9_HEADERS = ('include/drm-uapi/amdgpu_drm.h',)	# plan9/drm's, under vendor/
# hasvk first, then radv: their objects stand for venus' equal ones (members.py)
SOS = ['src/intel/vulkan_hasvk/libvulkan_intel_hasvk.so', 'src/amd/vulkan/libvulkan_radeon.so',
	'src/virtio/vulkan/libvulkan_virtio.so']


def ninja(*args):
	return subprocess.run(['ninja', '-C', MESA] + list(args), check=True,
		capture_output=True, text=True).stdout


def objects():
	"""the objects of the ICDs, in link order, each once"""
	sys.path.insert(0, os.path.dirname(os.path.realpath(__file__)))
	import members
	return members.members(MESA, os.environ['NM'], SOS)


def here(t):
	"""t in this tree: the meson build may have been set up from another
	checkout of it (its cross.ini names that one's sys/src/mesa/plan9)"""
	k = t.find('/sys/src/mesa/')
	if k > 0 and not t.startswith(ROOT + '/'):
		u = ROOT + t[k:]
		if os.path.exists(u):
			return u
	return t


def where(f):
	"""a file the build read: (its place in the tree, kind) or None (the compiler's)"""
	a = here(os.path.realpath(os.path.join(MESA, f)))
	if a.startswith(SRC + '/'):
		rel = a[len(SRC) + 1:]
		for p in PUBLIC:
			if rel.startswith(p):
				return os.path.join(APEINC, rel[len('include/'):])
		return os.path.join(VENDOR, rel)
	if a.startswith(MESA + '/'):
		return os.path.join(VENDOR, a[len(MESA) + 1:])
	if a.startswith(P9 + '/'):
		return a
	return None


def copy(src, dst):
	"""src to dst unless it is the same already; another objtype's
	different file there is a conflict"""
	try:
		with open(src, 'rb') as a, open(dst, 'rb') as b:
			if a.read() == b.read():
				return
		if os.environ.get('VENDOR_FRESH') != '1':
			conflicts.append(dst)
			return
	except OSError:
		pass
	os.makedirs(os.path.dirname(dst), exist_ok=True)
	shutil.copyfile(src, dst)


def write(path, text):
	try:
		if open(path).read() == text:
			return
	except OSError:
		pass
	os.makedirs(os.path.dirname(path), exist_ok=True)
	open(path, 'w').write(text)


def flags(cmd, srcdir):
	"""meson's compile flags for a source in srcdir: defines, options,
	includes relative to srcdir; without the compiler, outputs, depfiles,
	the source and what build.exe passes itself"""
	words = shlex.split(cmd)[1:]
	out = []
	i = 0
	while i < len(words):
		w = words[i]
		i += 1
		if w in ('-o', '-MQ', '-MF'):
			i += 1
			continue
		if w in ('-c', '-MD', '-Winvalid-pch', '-fdiagnostics-color=always', '-pipe') or not w.startswith('-'):
			continue
		if w.startswith('-I'):
			d = w[2:]
			t = here(os.path.realpath(os.path.join(MESA, d)))
			if t.startswith(SRC + '/include') or t == SRC + '/include':
				t = os.path.join(VENDOR, 'include')
			elif t.startswith(SRC + '/'):
				t = os.path.join(VENDOR, t[len(SRC) + 1:])
			elif t.startswith(MESA + '/'):
				t = os.path.join(VENDOR, t[len(MESA) + 1:])
			elif not t.startswith(ROOT + '/'):
				continue	# the cross compiler's own
			if not os.path.isdir(t):
				continue	# meson's private object directories
			w = '-I' + os.path.relpath(t, srcdir)
			if w in out:
				continue
		out.append(w)
	return out


def main():
	cc = {c['output']: c for c in json.load(open(os.path.join(MESA, 'compile_commands.json')))}
	objs = objects()

	# every file the objects were made from
	files = set()
	for line in ninja('-t', 'deps', *objs).split('\n'):
		if line.startswith('    '):
			files.add(line.strip())
	for o in objs:
		files.add(cc[o]['file'])
	placed = {}
	for f in sorted(files):
		t = where(f)
		if t is None or t.startswith(P9 + '/'):
			continue
		placed[f] = t
		copy(os.path.join(MESA, f), t)
	# what plan9/drm (built here, not by meson) reads besides: amdgpu9.c's uapi
	for rel in PLAN9_HEADERS:
		copy(os.path.join(SRC, rel), os.path.join(VENDOR, rel))

	# one block per (directory, meson library, language)
	blocks = {}
	order = []
	for o in objs:
		c = cc[o]
		src = where(c['file'])
		d = os.path.dirname(src)
		lib = o.split('.p/')[0]
		ext = os.path.splitext(src)[1]
		lang = 'asm' if ext == '.S' else 'c++' if ext == '.cpp' else 'c'
		key = (d, lib, lang)
		if key not in blocks:
			blocks[key] = {'flags': flags(c['command'], d), 'files': []}
			order.append(key)
		blocks[key]['files'].append(os.path.basename(src))

	# a source in several libraries of a directory (isl, hasvk per hardware
	# generation: other -DGFX_VERx10): their objects x.<library>.o (Objtag)
	seen = {}
	for key in order:
		for f in blocks[key]['files']:
			seen.setdefault((key[0], f), set()).add(key)
	for key in order:
		d, lib, lang = key
		b = blocks[key]
		if any(len(seen[(d, f)]) > 1 for f in b['files']):
			b['tag'] = os.path.basename(lib).replace('.a', '').replace('lib', '', 1).replace('.', '_')
		t = '.' + b['tag'] if 'tag' in b else ''
		b['objects'] = [os.path.join(d, os.path.splitext(f)[0] + t + '.o') for f in b['files']]

	manifests = {}
	for key in order:
		d, lib, lang = key
		b = blocks[key]
		name = 'mesa-' + os.path.relpath(d, VENDOR).replace('/', '-') + '-' + os.path.basename(lib).replace('.', '-') + ('-' + lang if lang != 'c' else '')
		m = {'Name': name, 'Compat': True}
		if 'tag' in b:
			m['Objtag'] = b['tag']
		if lang != 'asm':
			m['Cflags'] = b['flags']
		m['SourceFiles'] = sorted(b['files'])
		manifests.setdefault(d, []).append(m)
	for d, ms in manifests.items():
		write(os.path.join(d, 'mesa%s.json' % SUFFIX), json.dumps(ms, indent='\t') + '\n')

	# the loader's trampolines, from the same vk.xml
	subprocess.run(['python3', os.path.join(P9, 'loader/genloader.py'),
		os.path.join(SRC, 'src/vulkan/registry/vk.xml'), os.path.join(P9, 'loader/vkcmds.c')], check=True)

	top = os.path.join(ROOT, 'sys/src/mesa')
	allobjs = [os.path.relpath(o, top) for k in order for o in blocks[k]['objects']]
	allobjs += ['plan9/loader/vkloader.o', 'plan9/loader/vkcmds.o',
		'plan9/lib/dl.o', 'plan9/lib/plan9compat.o', 'plan9/lib/plan9win.o',
		'plan9/drm/drm9.o', 'plan9/drm/amdgpu9.o']
	projects = ['/' + os.path.relpath(os.path.join(d, 'mesa%s.json' % SUFFIX), ROOT) for d in manifests]
	projects += ['/sys/src/mesa/plan9/loader/build.json', '/sys/src/mesa/plan9/lib/build.json',
		'/sys/src/mesa/plan9/drm/build.json']
	build = [{
		'Name': 'mesa-libvulkan',
		'Projects': projects,
		'Library': 'libvulkan.a',
		'Whole': True,
		# ld -r: the emulation (on Linux the host's ld)
		'Oflags': {'amd64': ['-m', 'elf_x86_64'], '386': ['-m', 'elf_i386']}[OBJTYPE],
		'Install': '/$ARCH/lib.gcc/ape',
		'ObjectFiles': allobjs,
	}]
	write(os.path.join(top, 'build%s.json' % SUFFIX), json.dumps(build, indent='\t') + '\n')
	print('vendor %s: %d objects, %d files, %d manifests' % (OBJTYPE, len(allobjs), len(placed), len(manifests)))
	if conflicts:
		sys.exit('vendor: differs from what is vendored (another objtype? a new Mesa: VENDOR_FRESH=1):\n\t' + '\n\t'.join(conflicts))


main()
