#!/usr/bin/env python3
"""
The tree's parts built with ../9front-gcc's cross compiler for APE (POSIX
on 9front): the libraries of sys/src/mesa (libvulkan.a), sys/src/libffi,
sys/src/wayland, sys/src/sdl3, sys/src/glfw (compatibility manifests,
build.exe -D) and the programs made of them, which scripts/build/ape.sh
installs in /$objtype/bin. scripts/gen/genbuild.py makes the build step,
scripts/build/check-install.py expects the programs.

usage: scripts/gen/apeprogs.py	lists the programs: path source
"""
import os

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))

# the architectures ape.sh builds for (scripts/mesa/common.sh: its sysroot view)
OBJTYPES = ['amd64']

# the libraries' manifests, in the order they depend on each other
LIBS = ['sys/src/mesa', 'sys/src/libffi', 'sys/src/wayland', 'sys/src/sdl3', 'sys/src/glfw']

# program: its source directory
PROGS = {'wlsys': 'sys/src/cmd/wlsys'}

TRIPLES = {'amd64': 'x86_64-pc-plan9', '386': 'i686-pc-plan9'}

def cross(objtype):
	"""is there ../9front-gcc's cross compiler for objtype?"""
	d = os.environ.get('GCC_DIR') or os.path.join(ROOT, '..', '9front-gcc')
	t = TRIPLES.get(objtype)
	return t is not None and os.access(os.path.join(d, 'build', objtype, 'cross', 'bin', t + '-gcc'), os.X_OK)

if __name__ == '__main__':
	for p, d in sorted(PROGS.items()):
		print(p, d)
