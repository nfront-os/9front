#!/usr/bin/env python3
"""
The tree's parts built with ../9front-gcc's cross compiler for APE (POSIX
on 9front): the libraries of sys/src/mesa (libvulkan.a), sys/src/libffi,
sys/src/wayland, sys/src/sdl3, sys/src/glfw, sys/src/freetype, sys/src/pixman,
sys/src/expat, sys/src/xkbcommon, sys/src/fontconfig, sys/src/fcft, sys/src/foot (compatibility manifests; the last seven vendored by
scripts/ape/vendor.sh from their VENDOR.json,
build.exe -D) and the programs made of them, which scripts/build/ape.sh
installs in /$objtype/bin. scripts/gen/genbuild.py makes the build step,
scripts/build/check-install.py expects the programs.

usage: scripts/gen/apeprogs.py	lists the programs: path source
"""
import os, sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.insert(0, os.path.join(ROOT, 'scripts', 'lib'))
import paths	# GCC_DIR, RUST_DIR, ... (the distribution's paths.env)

# the architectures ape.sh builds for (scripts/mesa/common.sh: its sysroot view)
OBJTYPES = ['amd64']

# the libraries' manifests, in the order they depend on each other
LIBS = ['sys/src/mesa', 'sys/src/libffi', 'sys/src/wayland', 'sys/src/sdl3', 'sys/src/glfw',
	'sys/src/freetype', 'sys/src/pixman', 'sys/src/expat', 'sys/src/xkbcommon', 'sys/src/fontconfig', 'sys/src/fcft', 'sys/src/foot']

# program: its source directory
PROGS = {'wlsys': 'sys/src/cmd/wlsys', 'foot': 'sys/src/foot'}

# programs of a vendored release, linked from its library (LIBS made it): their
# link arguments (scripts/build/ape.sh); the others scripts/wayland/build.sh makes
LINKS = {
	'foot': ['-Wl,-u,main', '-lfoot', '-lfcft', '-lfontconfig', '-lfreetype', '-lexpat',
		'-lpixman-1', '-lxkbcommon', '-lwayland-cursor', '-lwayland-client', '-lffi', '-lm'],
}

TRIPLES = {'amd64': 'x86_64-pc-plan9', '386': 'i686-pc-plan9'}

def cross(objtype):
	"""is there the cross compiler of 9front-gcc (GCC_DIR) for objtype?"""
	d = paths.get('GCC_DIR')
	t = TRIPLES.get(objtype)
	return t is not None and os.access(os.path.join(d, 'build', objtype, 'cross', 'bin', t + '-gcc'), os.X_OK)

if __name__ == '__main__':
	for p, d in sorted(PROGS.items()):
		print(p, d)
