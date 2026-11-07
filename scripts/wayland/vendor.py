#!/usr/bin/env python3
"""libwayland into the tree: see vendor.sh.  usage: CC=... WORK=... vendor.py ROOT"""
import hashlib, json, os, shutil, subprocess, sys, tarfile

root = os.path.abspath(sys.argv[1])
wl = os.path.join(root, 'sys/src/wayland')
cc, work = os.environ['CC'], os.environ['WORK']
ver = dict(l.split(None, 1) for l in open(os.path.join(wl, 'VERSION')).read().splitlines() if l.strip())

def unpack(tarkey, shakey, name):
    t = os.path.normpath(os.path.join(root, ver[tarkey]))
    if hashlib.sha256(open(t, 'rb').read()).hexdigest() != ver[shakey]:
        sys.exit('vendor.py: %s: not the sha256 of VERSION' % t)
    d = os.path.join(work, name)
    shutil.rmtree(d, ignore_errors=True)
    os.makedirs(work, exist_ok=True)
    with tarfile.open(t) as f:
        f.extractall(work, filter='data')
    return d

tree = unpack('tar', 'sha256', 'wayland-' + ver['version'])
prot = unpack('protocols-tar', 'protocols-sha256', 'wayland-protocols-' + ver['protocols'])
scanv = subprocess.run(['wayland-scanner', '--version'], capture_output=True, text=True).stderr.split()[-1]
if scanv != ver['version']:
    sys.exit('vendor.py: the host\'s wayland-scanner is %s, not %s' % (scanv, ver['version']))

lines = [l.split() for l in open(os.path.join(wl, 'SOURCES')) if l.strip() and not l.startswith('#')]
gen = os.path.join(wl, 'gen')
shutil.rmtree(gen, ignore_errors=True)
os.makedirs(gen)
v = ver['version'].split('.')
h = open(os.path.join(tree, 'src/wayland-version.h.in')).read()
for k, val in (('WAYLAND_VERSION_MAJOR', v[0]), ('WAYLAND_VERSION_MINOR', v[1]),
               ('WAYLAND_VERSION_MICRO', v[2]), ('WAYLAND_VERSION', ver['version'])):
    h = h.replace('@%s@' % k, val)
open(os.path.join(gen, 'wayland-version.h'), 'w').write(h)
protos = []
for kind, path in lines:
    if kind != 'protocol':
        continue
    xml = os.path.join(tree if path.startswith('protocol/') else prot, path)
    name = os.path.basename(path)[:-len('.xml')]
    for mode, out in (('client-header', name + '-client-protocol.h'),
                      ('server-header', name + '-server-protocol.h'),
                      ('private-code', name + '-protocol.c')):
        subprocess.run(['wayland-scanner', mode, xml, os.path.join(gen, out)], check=True)
    shutil.copy2(xml, os.path.join(gen, os.path.basename(path)))
    protos.append(name)

inc = ['-I' + os.path.join(wl, 'plan9'), '-I' + gen, '-I' + os.path.join(tree, 'src')]
def deps(p):
    out = subprocess.run([cc, '-std=gnu11', '-MM', '-MG'] + inc + [p], capture_output=True, text=True, check=True).stdout
    return [os.path.normpath(w) for w in out.replace('\\\n', ' ').split()[1:]]
need = set()
for kind, path in lines:
    if kind != 'protocol':
        need.add(os.path.join(tree, path))
        need.update(deps(os.path.join(tree, path)))
vendor = os.path.join(wl, 'vendor')
shutil.rmtree(vendor, ignore_errors=True)
n = 0
for p in sorted(need):
    if not p.startswith(tree + '/'):
        continue
    rel = os.path.relpath(p, tree)
    os.makedirs(os.path.dirname(os.path.join(vendor, rel)), exist_ok=True)
    shutil.copy2(p, os.path.join(vendor, rel))
    n += 1
# the sources include it as ../config.h: ours, from plan9/
shutil.copy2(os.path.join(wl, 'plan9/config.h'), os.path.join(vendor, 'config.h'))
shutil.copy2(os.path.join(tree, 'COPYING'), os.path.join(vendor, 'COPYING'))
shutil.copy2(os.path.join(tree, 'README.md'), os.path.join(vendor, 'README.md'))
shutil.copy2(os.path.join(prot, 'COPYING'), os.path.join(gen, 'COPYING.wayland-protocols'))

# the public headers, as upstream installs them
pub = os.path.join(root, 'sys/include/ape')
for f in ('wayland-client.h', 'wayland-client-core.h', 'wayland-server.h', 'wayland-server-core.h', 'wayland-util.h'):
    shutil.copy2(os.path.join(tree, 'src', f), os.path.join(pub, f))
# libwayland-egl's (wl_egl_window for EGL's Wayland platform)
for f in ('wayland-egl.h', 'wayland-egl-core.h', 'wayland-egl-backend.h'):
    shutil.copy2(os.path.join(tree, 'egl', f), os.path.join(pub, f))
# libwayland-cursor's (cursor images from themes, a built-in one without)
shutil.copy2(os.path.join(tree, 'cursor', 'wayland-cursor.h'), os.path.join(pub, 'wayland-cursor.h'))
shutil.copy2(os.path.join(gen, 'wayland-version.h'), os.path.join(pub, 'wayland-version.h'))
for name in protos:
    for side in ('client', 'server'):
        shutil.copy2(os.path.join(gen, '%s-%s-protocol.h' % (name, side)), os.path.join(pub, '%s-%s-protocol.h' % (name, side)))

def files(which):
    return ['vendor/' + p for k, p in lines if k in which]
cflags = ['-O2', '-Wall', '-fno-strict-aliasing', '-Iplan9', '-Igen', '-Ivendor/src']
build = [{
    'Name': 'wayland-client',
    'Compat': True,
    'Cflags': cflags,
    'Library': 'libwayland-client.a',
    'Install': '/$ARCH/lib.gcc/ape',
    'Objtag': 'c',
    'SourceFiles': files(('client', 'both')) + ['gen/%s-protocol.c' % n for n in protos],
}, {
    'Name': 'wayland-server',
    'Compat': True,
    'Cflags': cflags,
    'Library': 'libwayland-server.a',
    'Install': '/$ARCH/lib.gcc/ape',
    'Objtag': 's',
    'SourceFiles': files(('server', 'both')) + ['gen/%s-protocol.c' % n for n in protos],
}, {
    'Name': 'wayland-egl',
    'Compat': True,
    'Cflags': cflags + ['-Ivendor/egl'],
    'Library': 'libwayland-egl.a',
    'Install': '/$ARCH/lib.gcc/ape',
    'Objtag': 'e',
    'SourceFiles': files(('egl',)),
}, {
    'Name': 'wayland-cursor',
    'Compat': True,
    'Cflags': cflags + ['-Ivendor/cursor', '-DICONDIR="/lib/icons"'],
    'Library': 'libwayland-cursor.a',
    'Install': '/$ARCH/lib.gcc/ape',
    'Objtag': 'k',
    'SourceFiles': files(('cursor',)),
}]
with open(os.path.join(wl, 'build.json'), 'w') as f:
    json.dump(build, f, indent='\t')
    f.write('\n')
print('vendor.py: %d files of wayland %s in sys/src/wayland/vendor, protocols %s in gen, headers in /sys/include/ape'
      % (n, ver['version'], ' '.join(protos)))
