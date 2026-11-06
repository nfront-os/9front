#!/usr/bin/env python3
"""SDL3 into the tree: see vendor.sh.  usage: CC=... WORK=... vendor.py ROOT"""
import hashlib, json, os, shutil, subprocess, sys, tarfile

root = os.path.abspath(sys.argv[1])
sdl = os.path.join(root, 'sys/src/sdl3')
cc, work = os.environ['CC'], os.environ['WORK']

ver = dict(l.split(None, 1) for l in open(os.path.join(sdl, 'VERSION')).read().splitlines() if l.strip())
tarball = os.path.normpath(os.path.join(root, ver['tarball']))
if hashlib.sha256(open(tarball, 'rb').read()).hexdigest() != ver['sha256']:
    sys.exit('vendor.py: %s: not the sha256 of VERSION' % tarball)

# the release with the overlay over it
tree = os.path.join(work, 'tree')
shutil.rmtree(tree, ignore_errors=True)
os.makedirs(work, exist_ok=True)
with tarfile.open(tarball) as t:
    t.extractall(work, filter='data')
os.rename(os.path.join(work, 'SDL3-' + ver['version']), tree)
ov = os.path.join(sdl, 'overlay')
for d, _, fs in os.walk(ov):
    for f in fs:
        rel = os.path.relpath(os.path.join(d, f), ov)
        shutil.copy2(os.path.join(d, f), os.path.join(tree, rel))

defs = ['-D_REENTRANT', '-DUSING_GENERATED_CONFIG_H', '-DSDL_STATIC_LIB', '-DNDEBUG',
        '-DSDL_BUILD_MAJOR_VERSION=3', '-DSDL_BUILD_MINOR_VERSION=4', '-DSDL_BUILD_MICRO_VERSION=20',
        '-DEGL_NO_X11', '-DMESA_EGL_NO_X11_HEADERS']
config = os.path.join(sdl, 'plan9/include/build_config')
ours = sorted(os.path.relpath(os.path.join(d, f), sdl)
              for d, _, fs in os.walk(os.path.join(sdl, 'plan9')) for f in fs if f.endswith('.c'))
theirs = [l.strip() for l in open(os.path.join(sdl, 'SOURCES')) if l.strip() and not l.startswith('#')]

def deps(path):
    out = subprocess.run([cc, '-std=gnu11', '-MM', '-MG'] + defs +
                         ['-I' + config, '-I' + os.path.join(tree, 'include'), '-I' + os.path.join(tree, 'src'), path],
                         capture_output=True, text=True, check=True).stdout
    return [os.path.normpath(w) for w in out.replace('\\\n', ' ').split()[1:]]

need = set()
for s in theirs:
    need.add(os.path.join(tree, s))
    need.update(deps(os.path.join(tree, s)))
for s in ours:
    need.update(deps(os.path.join(sdl, s)))

vendor = os.path.join(sdl, 'vendor')
shutil.rmtree(vendor, ignore_errors=True)
n = 0
for p in sorted(need):
    if not p.startswith(tree + '/src/'):
        continue    # SDL's public headers go to /sys/include/ape/SDL3, ours stay
    rel = os.path.relpath(p, tree)
    os.makedirs(os.path.dirname(os.path.join(vendor, rel)), exist_ok=True)
    shutil.copy2(p, os.path.join(vendor, rel))
    n += 1
for f in ('LICENSE.txt', 'README.md', 'REVISION.txt'):
    shutil.copy2(os.path.join(tree, f), os.path.join(vendor, f))
pub = os.path.join(root, 'sys/include/ape/SDL3')
shutil.rmtree(pub, ignore_errors=True)
shutil.copytree(os.path.join(tree, 'include/SDL3'), pub)

build = [{
    'Name': 'sdl3',
    'Compat': True,
    'Cflags': ['-O2', '-Wall', '-fno-strict-aliasing'] + defs + ['-Iplan9/include/build_config', '-Ivendor/src'],
    'Library': 'libSDL3.a',
    'Install': '/$ARCH/lib.gcc/ape',
    'SourceFiles': ['vendor/' + s for s in theirs] + ours,
}]
with open(os.path.join(sdl, 'build.json'), 'w') as f:
    json.dump(build, f, indent='\t')
    f.write('\n')
print('vendor.py: %d files of SDL %s in sys/src/sdl3/vendor, %d to build, its headers in /sys/include/ape/SDL3'
      % (n, ver['version'], len(theirs) + len(ours)))
