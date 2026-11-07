#!/usr/bin/env python3
"""libffi into the tree: see vendor.sh.  usage: CC=... WORK=... vendor.py ROOT"""
import hashlib, json, os, shutil, subprocess, sys, tarfile

root = os.path.abspath(sys.argv[1])
ffi = os.path.join(root, 'sys/src/libffi')
cc, work = os.environ['CC'], os.environ['WORK']

ver = dict(l.split(None, 1) for l in open(os.path.join(ffi, 'VERSION')).read().splitlines() if l.strip())
tgz = os.path.normpath(os.path.join(root, ver['tar']))
if hashlib.sha256(open(tgz, 'rb').read()).hexdigest() != ver['sha256']:
    sys.exit('vendor.py: %s: not the sha256 of VERSION' % tgz)
tree = os.path.join(work, 'libffi-' + ver['version'])
shutil.rmtree(tree, ignore_errors=True)
os.makedirs(work, exist_ok=True)
with tarfile.open(tgz) as t:
    t.extractall(work, filter='data')

# ffi.h as configure makes it for amd64
v = ver['version']
num = '%d%02d%02d' % tuple(int(x) for x in v.split('.'))
h = open(os.path.join(tree, 'include/ffi.h.in')).read()
for k, val in (('VERSION', v), ('TARGET', 'X86_64'), ('HAVE_LONG_DOUBLE', '1'),
               ('FFI_EXEC_TRAMPOLINE_TABLE', '0'), ('FFI_VERSION_STRING', v),
               ('FFI_VERSION_NUMBER', num)):
    h = h.replace('@%s@' % k, val)
assert '@' not in h.replace('@file', '').replace('@code', ''), 'ffi.h: an @...@ left'
pub = os.path.join(root, 'sys/include/ape')
open(os.path.join(pub, 'ffi.h'), 'w').write(h)
shutil.copy2(os.path.join(tree, 'src/x86/ffitarget.h'), os.path.join(pub, 'ffitarget.h'))

theirs = [l.strip() for l in open(os.path.join(ffi, 'SOURCES')) if l.strip() and not l.startswith('#')]
inc = ['-I' + os.path.join(ffi, 'plan9'), '-I' + pub, '-I' + os.path.join(tree, 'include'),
       '-I' + os.path.join(tree, 'src/x86')]

def deps(path):
    out = subprocess.run([cc, '-MM', '-MG'] + inc + [path], capture_output=True, text=True, check=True).stdout
    return [os.path.normpath(w) for w in out.replace('\\\n', ' ').split()[1:]]

need = set()
for s in theirs:
    need.add(os.path.join(tree, s))
    need.update(deps(os.path.join(tree, s)))
vendor = os.path.join(ffi, 'vendor')
shutil.rmtree(vendor, ignore_errors=True)
n = 0
for p in sorted(need):
    if not p.startswith(tree + '/'):
        continue    # ours (plan9/) and the public headers in /sys/include/ape
    rel = os.path.relpath(p, tree)
    os.makedirs(os.path.dirname(os.path.join(vendor, rel)), exist_ok=True)
    shutil.copy2(p, os.path.join(vendor, rel))
    n += 1
for f in ('LICENSE', 'README.md'):
    shutil.copy2(os.path.join(tree, f), os.path.join(vendor, f))

build = [{
    'Name': 'libffi',
    'Compat': True,
    'Cflags': ['-O2', '-Wall', '-fno-strict-aliasing', '-Iplan9', '-Ivendor/include', '-Ivendor/src/x86'],
    'Library': 'libffi.a',
    'Install': '/$ARCH/lib.gcc/ape',
    'SourceFiles': ['vendor/' + s for s in theirs],
}]
with open(os.path.join(ffi, 'build.json'), 'w') as f:
    json.dump(build, f, indent='\t')
    f.write('\n')
print('vendor.py: %d files of libffi %s in sys/src/libffi/vendor, %d to build, ffi.h in /sys/include/ape'
      % (n, v, len(theirs)))
