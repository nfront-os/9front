#!/usr/bin/env python3
"""
A library for APE into the tree from its release, as sys/src/<lib>/VENDOR.json
describes it (the libraries scripts/build/ape.sh builds with ../9front-gcc's
cross compiler):

	name, version, url, sha256, tar (the archive, from the tree's root:
	../9front-ui/src/...), top (its directory), key (who signed it), keep
	(files to keep beside the sources: licences), libraries (each: name,
	library, objtag, cflags, sources: files of the release), headers (each:
	from, a file or directory of the release or of plan9/, to, a directory
	from the root: sys/include/ape/..., exclude: subdirectories not), gen (commands that make files of
	the release in it before anything is copied: [dir, argv...], dir from
	its top)

sys/src/<lib>/plan9/ holds what 9front needs instead (configuration
headers): it comes before the release's directories in each library's
include path. Of the release only the sources and the files they include
(the cross compiler's -MM with those include paths) go into
sys/src/<lib>/vendor/, as they are, and the files of keep; then
sys/src/<lib>/build.json (compatibility manifests: build.exe -D with
CC=$CROSS/$TRIPLE-gcc builds them into /$objtype/lib.gcc/ape).

usage: CC=... WORK=... scripts/ape/vendor.py ROOT lib	(scripts/ape/vendor.sh lib)
"""
import hashlib, json, os, shutil, subprocess, sys, tarfile

root = os.path.abspath(sys.argv[1])
lib = sys.argv[2]
here = os.path.join(root, 'sys/src', lib)
cc, work = os.environ['CC'], os.environ['WORK']
spec = json.load(open(os.path.join(here, 'VENDOR.json')))

tar = os.path.normpath(os.path.join(root, spec['tar']))
if hashlib.sha256(open(tar, 'rb').read()).hexdigest() != spec['sha256']:
	sys.exit('vendor.py: %s: not the sha256 of VENDOR.json' % tar)
os.makedirs(work, exist_ok=True)
tree = os.path.join(work, spec['top'])
shutil.rmtree(tree, ignore_errors=True)
with tarfile.open(tar) as f:
	f.extractall(work, filter='data')
for g in spec.get('gen', []):
	subprocess.run(g[1:], cwd=os.path.join(tree, g[0]), check=True)

plan9 = os.path.join(here, 'plan9')

def incs(l):
	"""a library's include path for -MM: plan9/ first, then the release's dirs"""
	out = ['-I' + plan9]
	for f in l.get('cflags', []):
		if f.startswith('-Ivendor/'):
			out.append('-I' + os.path.join(tree, f[len('-Ivendor/'):]))
		elif f.startswith('-D') or f.startswith('-U'):
			out.append(f)
	return out

def deps(src, l):
	out = subprocess.run([cc, '-std=gnu11', '-MM', '-MG'] + incs(l) + [src], capture_output=True, text=True)
	if out.returncode != 0:
		sys.exit('vendor.py: %s: %s' % (src, out.stderr))
	return [os.path.normpath(w) for w in out.stdout.replace('\\\n', ' ').split()[1:]]

need = set()
for l in spec['libraries']:
	for s in l['sources']:
		p = os.path.join(tree, s)
		if not os.path.exists(p):
			sys.exit('vendor.py: no %s in the release' % s)
		need.add(p)
		need.update(d if os.path.isabs(d) else os.path.join(tree, d) for d in deps(p, l))
vendor = os.path.join(here, 'vendor')
shutil.rmtree(vendor, ignore_errors=True)
n = 0
for p in sorted(need) + [os.path.join(tree, k) for k in spec.get('keep', [])]:
	if not p.startswith(tree + '/') or not os.path.isfile(p):
		continue
	rel = os.path.relpath(p, tree)
	os.makedirs(os.path.dirname(os.path.join(vendor, rel)), exist_ok=True)
	shutil.copy2(p, os.path.join(vendor, rel))
	n += 1

# the public headers, as the release installs them (plan9/'s instead where it has them)
for h in spec.get('headers', []):
	src = os.path.join(plan9, h['from'][len('plan9/'):]) if h['from'].startswith('plan9/') else os.path.join(tree, h['from'])
	dst = os.path.join(root, h['to'])
	if os.path.isdir(src):
		for dp, dn, fn in os.walk(src):
			dn[:] = [d for d in dn if d not in h.get('exclude', [])]
			for f in fn:
				if f.endswith('.h'):
					d = os.path.join(dst, os.path.relpath(dp, src))
					os.makedirs(d, exist_ok=True)
					shutil.copy2(os.path.join(dp, f), os.path.join(d, f))
	else:
		os.makedirs(dst, exist_ok=True)
		shutil.copy2(src, os.path.join(dst, os.path.basename(src)))

build = []
for l in spec['libraries']:
	build.append({
		'Name': l['name'],
		'Compat': True,
		'Cflags': ['-Iplan9'] + l.get('cflags', []),
		'Library': l['library'],
		'Install': '/$ARCH/lib.gcc/ape',
		'Objtag': l.get('objtag', 'v'),
		'SourceFiles': ['vendor/' + s for s in l['sources']],
	})
with open(os.path.join(here, 'build.json'), 'w') as f:
	json.dump(build, f, indent='\t')
	f.write('\n')
print('vendor.py: %d files of %s %s in sys/src/%s/vendor' % (n, spec['name'], spec['version'], lib))
