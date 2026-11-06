#!/usr/bin/env python3
"""GLFW into the tree: see vendor.sh.  usage: CC=... WORK=... vendor.py ROOT"""
import hashlib, json, os, shutil, subprocess, sys, zipfile

root = os.path.abspath(sys.argv[1])
glfw = os.path.join(root, 'sys/src/glfw')
cc, work = os.environ['CC'], os.environ['WORK']

ver = dict(l.split(None, 1) for l in open(os.path.join(glfw, 'VERSION')).read().splitlines() if l.strip())
zipped = os.path.normpath(os.path.join(root, ver['zip']))
if hashlib.sha256(open(zipped, 'rb').read()).hexdigest() != ver['sha256']:
    sys.exit('vendor.py: %s: not the sha256 of VERSION' % zipped)

# the release with the overlay over it
tree = os.path.join(work, 'tree')
shutil.rmtree(tree, ignore_errors=True)
shutil.rmtree(os.path.join(work, 'glfw-' + ver['version']), ignore_errors=True)
os.makedirs(work, exist_ok=True)
with zipfile.ZipFile(zipped) as z:
    z.extractall(work)
os.rename(os.path.join(work, 'glfw-' + ver['version']), tree)
ov = os.path.join(glfw, 'overlay')
for d, _, fs in os.walk(ov):
    for f in fs:
        rel = os.path.relpath(os.path.join(d, f), ov)
        shutil.copy2(os.path.join(d, f), os.path.join(tree, rel))

defs = ['-D_GLFW_PLAN9', '-D_DEFAULT_SOURCE']
ours = sorted(os.path.relpath(os.path.join(d, f), glfw)
              for d, _, fs in os.walk(os.path.join(glfw, 'plan9')) for f in fs if f.endswith('.c'))
theirs = [l.strip() for l in open(os.path.join(glfw, 'SOURCES')) if l.strip() and not l.startswith('#')]

def deps(path):
    out = subprocess.run([cc, '-std=gnu11', '-MM', '-MG'] + defs +
                         ['-I' + os.path.join(glfw, 'plan9'), '-I' + os.path.join(tree, 'include'),
                          '-I' + os.path.join(tree, 'src'), path],
                         capture_output=True, text=True, check=True).stdout
    return [os.path.normpath(w) for w in out.replace('\\\n', ' ').split()[1:]]

need = set()
for s in theirs:
    need.add(os.path.join(tree, s))
    need.update(deps(os.path.join(tree, s)))
for s in ours:
    need.update(deps(os.path.join(glfw, s)))

vendor = os.path.join(glfw, 'vendor')
shutil.rmtree(vendor, ignore_errors=True)
n = 0
for p in sorted(need):
    if not p.startswith(tree + '/src/') and not p.startswith(tree + '/include/'):
        continue    # ours stay; GLFW's public headers also go to /sys/include/ape/GLFW
                    # (internal.h includes them as ../include/GLFW)
    rel = os.path.relpath(p, tree)
    os.makedirs(os.path.dirname(os.path.join(vendor, rel)), exist_ok=True)
    shutil.copy2(p, os.path.join(vendor, rel))
    n += 1
for f in ('LICENSE.md', 'README.md'):
    shutil.copy2(os.path.join(tree, f), os.path.join(vendor, f))
pub = os.path.join(root, 'sys/include/ape/GLFW')
shutil.rmtree(pub, ignore_errors=True)
shutil.copytree(os.path.join(tree, 'include/GLFW'), pub)

build = [{
    'Name': 'glfw',
    'Compat': True,
    'Cflags': ['-O2', '-Wall', '-fno-strict-aliasing'] + defs + ['-Iplan9', '-Ivendor/src'],
    'Library': 'libglfw3.a',
    'Install': '/$ARCH/lib.gcc/ape',
    'SourceFiles': ['vendor/' + s for s in theirs] + ours,
}]
with open(os.path.join(glfw, 'build.json'), 'w') as f:
    json.dump(build, f, indent='\t')
    f.write('\n')
print('vendor.py: %d files of GLFW %s in sys/src/glfw/vendor, %d to build, its headers in /sys/include/ape/GLFW'
      % (n, ver['version'], len(theirs) + len(ours)))
