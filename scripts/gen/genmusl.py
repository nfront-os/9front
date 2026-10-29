#!/usr/bin/env python3
"""Generate the gcc build-tool manifests of APE's musl part,
sys/src/ape/lib/musl/musl[.386|.arm64].json: what musl gives APE (C99's float
and long double math, <fenv.h>, <complex.h>, the wide string functions and
<wctype.h>; and the engine of C99's printf and scanf, bridged to APE's FILE),
appended to libap.a. Not built by 9front's own compilers (C99 in musl's
headers): gcc only, as 9front's gcc port's toolchain had it (its compat).

The sources per architecture are scripts/gen/musl/<objtype>.list: musl's file
for each function libap does not have (an object of musl whose functions libap
has all is not there; amd64 and 386 as the toolchain chose them, comparing the
objects' symbols with libap's; arm64 the same functions, aarch64's file where
musl has one). The objects are x.musl.o (Objtag): beside libap's of the same
name. Entry points that libap or programs have under the same names are renamed
(-D): the engine's printf & co are reached through APE's (ap/gnu/stdio.c).

usage: genmusl.py [--list]	(--list: the manifests, for genbuild.py)
"""
import json
import os
import sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
DIR = 'sys/src/ape/lib/musl'
ARCHES = {'amd64': ('x86_64', ''), '386': ('i386', '.386'), 'arm64': ('aarch64', '.arm64')}

# the tree's flags that make its ABI (/$ARCH/cflags.json, gcc.json), not the rest of
# them (-O0, -std=gnu17, -fno-builtin: they would come after musl's and win)
TREEFLAGS = ['-fno-pie', '-static', '-fno-stack-protector', '-D__plan9__', '-Ulinux',
	'-U__linux', '-U__linux__', '-U__gnu_linux__', '-g']
ARCHFLAGS = {
	'amd64': ['-mcmodel=small'],
	'386': ['-m32', '-march=pentium', '-mfpmath=387', '-mno-sse', '-mincoming-stack-boundary=2'],
	'arm64': ['-fsigned-char', '-mno-outline-atomics'],
}

# musl's CFLAGS_C99FSE, CFLAGS_AUTO (but -march), -D_XOPEN_SOURCE=700 (musl's Makefile)
CFLAGS = [
	'-std=c99', '-nostdinc', '-ffreestanding', '-fexcess-precision=standard',
	'-frounding-math', '-fno-strict-aliasing', '-Wa,--noexecstack', '-D_XOPEN_SOURCE=700',
	'-O2', '-fno-align-jumps', '-fno-align-functions', '-fno-align-loops', '-fno-align-labels',
	'-fira-region=one', '-fira-hoist-pressure', '-freorder-blocks-algorithm=simple',
	'-fno-prefetch-loop-arrays', '-fno-tree-ch', '-fomit-frame-pointer', '-fno-unwind-tables',
	'-fno-asynchronous-unwind-tables', '-ffunction-sections', '-fdata-sections',
	'-Wno-pointer-to-int-cast', '-Werror=implicit-function-declaration', '-Werror=implicit-int',
	'-Werror=pointer-sign', '-Werror=pointer-arith', '-Werror=int-conversion',
	'-Werror=incompatible-pointer-types', '-Werror=discarded-qualifiers',
	'-Werror=discarded-array-qualifiers',
	# libap has its pow10(int) and signgam
	'-Dpow10=__musl_pow10', '-Dsigngam=__musl_signgam',
]

# the engine: its entry points as APE's ap/gnu/stdio.c and wstdio.c call them; what
# libap or the wide functions of ap/gnu have under the same names, out of the way
ENGINE = [
	'src/stdio/vfprintf.c', 'src/stdio/vsnprintf.c', 'src/stdio/vfscanf.c',
	'src/stdio/vsscanf.c', 'src/stdio/fprintf.c', 'src/stdio/fscanf.c',
	'src/stdio/snprintf.c', 'src/stdio/vfwprintf.c', 'src/stdio/vswprintf.c',
	'src/stdio/vfwscanf.c', 'src/stdio/vswscanf.c', 'src/stdio/__towrite.c',
	'src/stdio/__toread.c', 'src/stdio/__uflow.c', 'src/stdio/fwrite.c',
	'src/internal/floatscan.c', 'src/internal/intscan.c', 'src/internal/shgetc.c',
	'src/stdlib/strtod.c', 'src/stdlib/wcstod.c', 'src/stdlib/wcstol.c',
	'ape/bridge.c',
]
ENGINE_RENAMES = {
	'vfprintf': '__apecompat_mvfprintf_', 'vsnprintf': '__apecompat_mvsnprintf',
	'vsscanf': '__apecompat_mvsscanf', 'strtof': '__apecompat_mstrtof',
	'strtold': '__apecompat_mstrtold', 'vswprintf': '__apecompat_mvswprintf',
	'vswscanf': '__apecompat_mvswscanf', 'wctomb': '__apecompat_wctomb',
	'mbtowc': '__apecompat_mbtowc',
}
for n in ('fprintf', 'fscanf', 'fwrite', 'fwrite_unlocked', 'snprintf', 'strtod', 'vfscanf',
		'vfwprintf', 'vfwscanf', 'fputwc', 'fwide', 'getwc', 'ungetwc'):
	ENGINE_RENAMES[n] = '__musl_' + n

def manifests(objtype):
	march, sfx = ARCHES[objtype]
	srcs = open(os.path.join(ROOT, 'scripts/gen/musl/%s.list' % objtype)).read().split()
	inc = ['arch/' + march, 'arch/generic', 'src/include', 'src/internal',
		'obj/%s/include' % march, 'include']
	flags = TREEFLAGS + ARCHFLAGS[objtype] + CFLAGS
	base = {
		'Install': '/$ARCH/lib.gcc/ape/',
		'Library': 'libap.a',
		'Append': True,
		'Objtag': 'musl',
		'IncludePath': inc,
	}
	m = dict(base, Name='ape-lib-musl-libap.a',
		Pre=['bash ../../../../../scripts/build/gen-musl-bits.sh ' + march],
		Clean=['obj/%s/include/bits/*.h' % march],
		Cflags=flags, SourceFiles=srcs)
	e = dict(base, Name='ape-lib-musl-engine-libap.a',
		Cflags=flags + ['-D%s=%s' % kv for kv in sorted(ENGINE_RENAMES.items())],
		SourceFiles=ENGINE)
	return 'musl%s.json' % sfx, [m, e]

def main():
	objtype = os.environ.get('objtype', 'amd64')
	name, builds = manifests(objtype)
	if '--list' in sys.argv[1:]:
		print(os.path.join(DIR, name))
		return
	for ot in ARCHES:
		name, builds = manifests(ot)
		s = json.dumps(builds, indent='\t') + '\n'
		p = os.path.join(ROOT, DIR, name)
		if not os.path.exists(p) or open(p).read() != s:
			open(p, 'w').write(s)
			print(os.path.join(DIR, name))

if __name__ == '__main__':
	main()
