#!/usr/bin/env python3
"""
The top-level manifests of the build tool: build.json (everything for
$ARCH) and build.<objtype>.json for 386, amd64 and arm64, so that

	CC=gcc ARCH=amd64 sys/src/cmd/build/build.exe

builds what the system needs, in dependency order: libc, the libraries
(scripts/gen/genlibs.py --list), the kernel's klib*.a, the programs
(scripts/gen/gencmds.py --list: sys/src/mkfile's $SUBSYS $CMDS; APE's musl
part after libap's, scripts/gen/genmusl.py --list), on amd64 Linux's DRM
drivers on the LinuxKPI (sys/src/linux, aux/i915) and the Rust crates
(scripts/gen/rustcrates.py, scripts/build/cargo.sh: wsys), the files
the kernel embeds (scripts/gen-*.sh: rebootcode.i, apbootstrap.i, ...,
initcode, /boot/boot and $CONF.bootfs.paq), the kernel, its Plan 9
a.out where 9boot loads it (scripts/build/elf2aout.py), and the check that
everything mk install installs is there (scripts/build/check-install.py).
The build tool sets objtype, O, SUFFIX, CONF and TOOLPREFIX from $ARCH.
9front's boot loaders are built apart, in the system just made, with
its own compilers (scripts/guest/build-boot.sh).

Run after a mkfile change, with the other generators (tests/static
checks that the checked-in files are what this writes).

usage: scripts/gen/genbuild.py
"""
import json, os, subprocess, sys

import genclean, rustcrates

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))

# per objtype: the kernel's generators (as pc64/mkfile, pc/mkfile,
# arm64/mkfile make them before the kernel objects), its manifest and
# the ELF kernel and a.out
KERNEL = {
	'amd64': (['bash scripts/build/gen-rebootcode-i.sh', 'bash scripts/build/gen-apbootstrap-i.sh',
		'bash scripts/build/gen-etherm10g-i.sh', 'bash scripts/build/gen-sd53c8xx-i.sh',
		'bash scripts/build/gen-initcode-i.sh', 'bash scripts/build/gen-bootfs.sh pc64'],
		'/sys/src/9/pc64/build.json', 'amd64/bin/9pc64', 'amd64/9pc64'),
	'386': (['bash scripts/build/gen-rebootcode-pc-i.sh', 'bash scripts/build/gen-apbootstrap-i.sh pc',
		'bash scripts/build/gen-sd53c8xx-i.sh', 'bash scripts/build/gen-initcode-i.sh',
		'bash scripts/build/gen-bootfs.sh pc'],
		'/sys/src/9/pc/build.json', '386/bin/9pc', '386/9pc'),
	'arm64': (['bash scripts/build/gen-rebootcode-i.sh arm64', 'bash scripts/build/gen-initcode-i.sh',
		'bash scripts/build/gen-bootfs.sh qemu arm64'],
		'/sys/src/9/arm64/build.json', 'arm64/bin/9qemu', 'arm64/9qemu'),
}

def listing(script, objtype):
	env = dict(os.environ, objtype=objtype)
	r = subprocess.run([sys.executable, os.path.join(ROOT, 'scripts', 'gen', script), '--list'],
		cwd=ROOT, env=env, capture_output=True, text=True)
	if r.returncode != 0:
		sys.exit('genbuild: %s --list: %s' % (script, r.stderr.strip()))
	return r.stdout.split()

def write(path, builds):
	s = json.dumps(builds, indent='\t') + '\n'
	p = os.path.join(ROOT, path)
	if not os.path.exists(p) or open(p).read() != s:
		open(p, 'w').write(s)
		print(path)

def main():
	klibs = sorted('/sys/src/%s/klib.json' % d for d in os.listdir(os.path.join(ROOT, 'sys/src'))
		if d.startswith('lib') and os.path.exists(os.path.join(ROOT, 'sys/src', d, 'klib.json')))
	for objtype, (gens, kern, elf, aout) in KERNEL.items():
		sfx = '' if objtype == 'amd64' else '.' + objtype
		libs = ['/sys/src/%s/%s%s.json' % (l, l, sfx) for l in listing('genlibs.py', objtype)]
		cmds = ['/' + c for c in listing('gencmds.py', objtype)]
		# APE's musl part (gcc only, no mkfile): appended to libap after
		# ape/lib/ap/gnu, before the programs that link libap
		gnu = [i for i, c in enumerate(cmds) if c.startswith('/sys/src/ape/lib/ap/gnu/')]
		cmds[gnu[-1] + 1:gnu[-1] + 1] = ['/' + c for c in listing('genmusl.py', objtype)]
		write('build.%s.json' % objtype, [
			{'Name': 'prep', 'Pre': ['bash scripts/build/prep-objtype.sh']},
			{'Name': 'libc', 'Projects': ['/sys/src/libc/libc%s.json' % sfx]},
			{'Name': 'libs', 'Projects': libs},
			{'Name': 'klibs', 'Projects': klibs},
			{'Name': 'cmds', 'Projects': cmds},
		] + ([
			# Linux's DRM drivers on the LinuxKPI (scripts/linux/genbuild.py)
			{'Name': 'linux', 'Projects': ['/sys/src/linux/build.json', '/sys/src/cmd/aux/gpu/i915/build.json', '/sys/src/cmd/aux/gpu/amdgpu/build.json']},
		] if objtype == 'amd64' else []) + ([
			# the Rust crates (scripts/gen/rustcrates.py: rust.json), with ../9front-rust
			{'Name': 'rust', 'Pre': ['bash scripts/build/cargo.sh %s %s' % (d, ' '.join(p))
				for d, p in rustcrates.crates()]},
		] if objtype in rustcrates.OBJTYPES and rustcrates.crates() else []) + [
			{'Name': 'kernelgen', 'Pre': gens, 'Clean': genclean.clean(gens, ROOT)},
			{'Name': 'kernel', 'Projects': [kern]},
			{'Name': 'install', 'Post': ['python3 scripts/build/elf2aout.py %s %s' % (elf, aout),
				'python3 scripts/build/check-install.py -r']},
		])
	write('build.json', [{'Name': 'all', 'Projects': ['/build.$ARCH.json']}])

if __name__ == '__main__':
	main()
