#!/usr/bin/env python3
"""
Check that everything `mk install' in sys/src installs is in place: the
libraries of sys/src/mkfile's LIBS (the gcc archives in /$objtype/lib.gcc,
and the kernel's klib*.a),
every target of the programs' mkfiles (scripts/gen/gencmds.py: cmd, games,
ape, /acme -- programs where their install rules put them, and the
files those rules install, scripts/build/mkinstall.py) and the kernel.

  missing  expected, not in the tree (or in the staged distribution)
  norust   a Rust crate's program (scripts/gen/rustcrates.py) not built: no Rust
           toolchain (scripts/build/cargo.sh skips them)
  nocross  a program of APE's parts (scripts/gen/apeprogs.py: wlsys) not built: no
           cross compiler of ../9front-gcc (scripts/build/ape.sh skips them)
  stale    in /$objtype/bin or /acme/bin/$objtype, but nothing installs
           it there (left from an older build: telnet before it moved
           from /amd64/bin/ip to /amd64/bin, as ip/mkfile installs it)

$objtype (scripts/gen/arch.py) selects the architecture.
Targets in scripts/gen/cmds-broken.txt (.386.txt, ...) are reported as broken, not missing.

usage: scripts/build/check-install.py [-r] [-d dist] [-i iso]
       scripts/build/check-install.py -l	list the expected files
	-r	remove stale files
	-d	also check the distribution tree staged by scripts/guest/stage-dist.sh
		(all but the programs' local archives, /sys/src/.../lib*.a6)
	-i	also check the files of a CD image (scripts/guest/mkiso.sh; xorriso
		lists them)
exit status 1 if anything is missing (or stale without -r)
"""
import glob
import json
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'gen'))
import gencmds as g
import apeprogs
import rustcrates
from arch import OBJTYPE as A

# the kernel mk install puts in /$objtype, and the ELF the build keeps
# beside the programs for boot-cli.sh's multiboot / QEMU -kernel
KERNELS = {'amd64': ['9pc64'], '386': ['9pc'], 'arm64': ['9qemu', '9qemu.u']}[A]

LINUX = {'amd64': ['sys/src/linux/build.json', 'sys/src/cmd/aux/gpu/i915/build.json', 'sys/src/cmd/aux/gpu/amdgpu/build.json']}

def expected():
	"""{9front path: target name}"""
	out = {}
	for l in g.variables(g.readmk('.'))['LIBS']:
		out['/%s/lib.gcc/%s.a' % (A, l)] = l
	for k in sorted(glob.glob(os.path.join(g.SRC, 'lib*', 'klib.json'))):	# the kernel's
		for b in json.load(open(k)):
			out['/%s/lib.gcc/' % A + b['Library']] = os.path.relpath(k, g.SRC)
	for k in KERNELS:
		out['/%s/%s' % (A, k)] = 'kernel'
	for d in g.dirs([]):
		for name, e in g.entries(d):
			if 'library' in e:
				p = (e.get('libinstall') or '/sys/src/%s/' % e['libdir']) + e['library']
			elif 'install' in e:
				for t, recipe, prereq, stem, dest in g.installfiles(d):
					if dest:
						out[dest] = name
				continue
			elif 'single' in e:
				p = e['Install'] + e['single'][:-2]
			else:
				p = e['Install'] + (e.get('program') or os.path.basename(name))
			out[p.replace('/$ARCH', '/' + A)] = name
	# Linux's DRM drivers on the LinuxKPI: not from mkfiles, their
	# manifests from scripts/linux/genbuild.py (amd64 only)
	for m in LINUX.get(A, []):
		for b in json.load(open(os.path.join(g.SRC, '..', '..', m))):
			if 'Library' in b:
				out[b['Install'].replace('/$ARCH', '/' + A) + b['Library']] = m
			elif 'Program' in b:
				out[b['Install'].replace('/$ARCH', '/' + A) + b['Program']] = m
	return out

def apeprogs_():
	"""{path: source} of the programs scripts/build/ape.sh installs"""
	if A not in apeprogs.OBJTYPES:
		return {}
	return {'/%s/bin/%s' % (A, p): d for p, d in apeprogs.PROGS.items()}

def rustprogs():
	"""{path: crate} of the Rust crates' programs (scripts/gen/rustcrates.py):
	built only where there is a Rust toolchain"""
	if A not in rustcrates.OBJTYPES:
		return {}
	return {'/%s/bin/%s' % (A, p): d for d, progs in rustcrates.crates() for p in progs}

def isofiles(iso):
	r = subprocess.run(['xorriso', '-indev', iso, '-find', '/'],
		capture_output=True, text=True, check=True)
	return set(l[1:-1].replace("'\\''", "'") for l in r.stdout.split('\n') if l.startswith("'/"))

def main():
	args = sys.argv[1:]
	if args == ['-l']:
		# the Rust and APE programs marked ?: check-install.rc does not miss them without Rust or gcc
		print('\n'.join(sorted(expected()) + sorted('?' + p for p in rustprogs()) + sorted('?' + p for p in apeprogs_())))
		return
	remove = '-r' in args
	dist = args[args.index('-d') + 1] if '-d' in args else None
	iso = isofiles(args[args.index('-i') + 1]) if '-i' in args else None
	exp = expected()
	rust = rustprogs()
	haverust = rustcrates.rustc()
	broken = g.loadbroken()
	bad = 0
	for p, name in sorted(exp.items()):
		# program-local archives are intermediates (the release's mk clean)
		local = p.startswith('/sys/src/')
		where = [('tree', g.ROOT)] + ([('dist', dist)] if dist and not local else [])
		if iso is not None and not local:
			where.append(('iso', None))
		for what, root in where:
			if not (p in iso if root is None else os.path.exists(root + p)):
				if re.sub(r'^(\.\./)+', '', name) in broken:
					print('broken  %s (%s)' % (p, name))
				else:
					print('missing %s %s (%s)' % (what, p, name))
					bad = 1
				break
	for p, name in sorted(rust.items()):
		if not os.path.exists(g.ROOT + p):
			if haverust:
				print('missing tree %s (%s)' % (p, name))
				bad = 1
			else:
				print('norust  %s (%s: no Rust toolchain, ../9front-rust)' % (p, name))
	exp.update(rust)
	ape = apeprogs_()
	havecross = apeprogs.cross(A)
	for p, name in sorted(ape.items()):
		if not os.path.exists(g.ROOT + p):
			if havecross:
				print('missing tree %s (%s)' % (p, name))
				bad = 1
			else:
				print('nocross %s (%s: no cross compiler, ../9front-gcc)' % (p, name))
	exp.update(ape)
	# stale: what no rule installs in the program directories (the kernel
	# build's ELF amd64/bin/9pc64 is for boot-cli.sh's multiboot)
	keep = {'/%s/bin/%s' % (A, KERNELS[0])}
	for top in ('/%s/bin' % A, '/acme/bin/' + A):
		for dp, dn, fn in os.walk(g.ROOT + top):
			for f in fn:
				p = os.path.join(dp, f)[len(g.ROOT):]
				if p in exp or p in keep:
					continue
				if remove:
					os.remove(g.ROOT + p)
					print('removed %s' % p)
				else:
					print('stale   %s' % p)
					bad = 1
	n = len(exp)
	print('%d files expected%s' % (n, ', ok' if not bad else ''))
	sys.exit(bad)

if __name__ == '__main__':
	main()
