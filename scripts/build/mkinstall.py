#!/usr/bin/env python3
"""
Install the files (not programs) that the install rule of a 9front
mkfile puts in place -- rc scripts copied to $BIN (git's %.rcinstall,
upas/binscripts' $BIN/%: %.rc, auth's $BIN/status), generated wrappers
(replica's ${SCRIPTS:%=$BIN/%}), scripts made by sed (postscript's
cropmarks: cropmarks.rc) -- by running the mkfile's own recipes.

scripts/gen/gencmds.py finds the targets and their rules (installfiles);
each recipe runs as mk runs it, with rc -e and the mkfile's variables,
$target, $prereq and $stem in the environment, in the source directory.
rc and the Plan 9 tools are plan9port's (../plan9port, as gen-src.sh);
the recipes' absolute paths (/$objtype/bin/git, /sys/lib/postscript)
are 9front paths, so they run in a bwrap namespace whose / holds the
tree's top-level directories (the host's /usr for the tools). Local
prerequisites a rule makes (cropmarks from cropmarks.rc) are made first.

usage: scripts/build/mkinstall.py dir... [-- target...]	(dirs relative to sys/src,
	as gencmds.py; the targets are for 9front, scripts/build/rc/mkinstall.rc)
"""
import os
import subprocess
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'gen'))
import gencmds as g

P9P = os.environ.get('PLAN9PORT', os.path.join(g.ROOT, '..', 'plan9port'))
P9P = os.path.abspath(P9P)
# host directories the tools need; the tree's lib (fonts) is not one a
# recipe installs into and would hide the host's
HOST = ('usr', 'etc')
SKIP = ('lib', 'tmp', 'scripts', 'tests')

def namespace(cwd):
	ns = ['bwrap', '--tmpfs', '/', '--ro-bind', '/usr', '/usr', '--ro-bind', '/etc', '/etc',
		'--symlink', 'usr/bin', '/bin', '--symlink', 'usr/lib', '/lib64', '--symlink', 'usr/lib', '/lib',
		'--dev', '/dev', '--proc', '/proc', '--tmpfs', '/tmp', '--ro-bind', P9P, P9P]
	for f in sorted(os.listdir(g.ROOT)):
		p = os.path.join(g.ROOT, f)
		if os.path.isdir(p) and not f.startswith('.') and f not in HOST + SKIP:
			ns += ['--bind', p, '/' + f]
	return ns + ['--chdir', cwd]

def run(d, v, target, recipe, prereq, stem):
	env = {'PATH': P9P + '/bin:/usr/bin', 'PLAN9': P9P, 'HOME': '/tmp'}
	for k, ws in v.items():
		env[k] = '\1'.join(ws)
	env.update(target=target, alltarget=target, prereq='\1'.join(prereq),
		newprereq='\1'.join(prereq), stem=stem)
	cwd = g.nine(d)
	print('%s: %s' % (cwd, target), flush=True)
	r = subprocess.run(namespace(cwd) + [P9P + '/bin/rc', '-e', '-c', recipe], env=env)
	if r.returncode != 0:
		sys.exit('mkinstall: %s: recipe for %s failed' % (d, target))

def make(d, v, rs, f, depth=0):
	"""a local prerequisite a rule makes (cropmarks: cropmarks.rc)"""
	r = g.findrule(rs, v, f)
	if r is None or depth > 8:
		return
	recipe, prereq, stem = r
	for p in prereq:
		if not p.startswith('/'):
			make(d, v, rs, p, depth + 1)
	run(d, v, f, recipe, prereq, stem)

def main():
	if len(sys.argv) < 2:
		sys.exit('usage: mkinstall.py dir...')
	# the targets after -- are for 9front's scripts/build/rc/mkinstall.rc
	args = sys.argv[1:]
	if '--' in args:
		args = args[:args.index('--')]
	for d in args:
		text = g.readmk(d)
		v = g.variables(text)
		rs = g.rules(text)
		for target, recipe, prereq, stem, dest in g.installfiles(d):
			for p in prereq:
				if not p.startswith('/'):
					make(d, v, rs, p)
			if dest:	# mk does not make the directory either; the install does
				os.makedirs(os.path.join(g.ROOT, os.path.dirname(dest).lstrip('/')), exist_ok=True)
			run(d, v, target, recipe, prereq, stem)

if __name__ == '__main__':
	main()
