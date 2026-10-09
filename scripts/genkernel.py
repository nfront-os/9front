#!/usr/bin/env python3
"""
Generate a kernel manifest (sys/src/9/<dir>/build.json) for the gcc
build tool from 9front's own kernel mkfile and CONF files, one entry per
configuration of the mkfile's CONFLIST (mk all), named after it (the
build tool selects entries by name: build.exe build.json '^pi4$').

What mk does, and its counterpart here:
  OBJ, PORT          the mkfile's object lists; $DEVS as ../port/mkdevlist
                     derives it from the CONF's dev/link/ip/misc sections;
                     $CONF.root.$O/$CONF.rootc.$O and $CONF.$O are the build
                     tool's own <conf>.c (mkdevc, mkrootc: Kernel.Config and
                     Kernel.Ramfiles from the CONF)
  x.$O               x.c (or the GAS port x.S of 8a/7a's x.s) of the kernel
                     directory first; then the mkfile's regexp rules
                     ^($REPCC)\\.$O:R: ../pc/\\1.c (REPCC from ../port/mkfilelist,
                     minus its sed exclusions) and explicit rules
                     (cache.v8.$O: ../arm64/cache.v8.s); then portmkfile's
                     ../port and ../ip
  header rules       ^($REPCH)\\.h:R: ../pc/\\1.h (cp), x.h: ../dir/x.h with
                     a recipe writing an #include (scripts/kheader.sh) or
                     touching it
Sources from another kernel directory are copied in by Pre steps, as
pc64's hand-made manifest does: kencc compiles them with -I. -. (local
headers first, not those beside the source); gcc always searches the
source's own directory first.

The flags, libraries and link layout per architecture are below (ARCHS),
as the hand-made manifests had them; deviations needed by gcc for one
kernel go in EXTRA.

usage: scripts/genkernel.py dir [conf...]   (dir below sys/src/9, e.g. pc;
                                             default: the mkfile's CONFLIST)
"""
import json
import os
import re
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
K = os.path.join(ROOT, 'sys', 'src', '9')

WARN = ['-Wno-error=implicit-int', '-Wall', '-Wno-missing-braces', '-Wno-parentheses', '-Wno-unknown-pragmas',
	'-Wno-switch', '-Wno-sign-compare', '-Wno-sequence-point']

ARCHS = {
	'386': {
		'Cflags': ['-m32', '-fno-pie', '-fno-pic', '-mno-sse', '-mno-sse2', '-mno-mmx',
			'-fplan9-extensions', '-fcommon', '-fno-builtin', '-fno-asynchronous-unwind-tables',
			'-fno-stack-protector', '-fno-omit-frame-pointer', '-fno-zero-initialized-in-bss',
			'-Os', '-gdwarf-2'] + WARN,
		'Oflags': ['-m', 'elf_i386', '-T', 'kernel.ld', '-z', 'max-page-size=0x1000'],
		'Libs': ['libgcc.a'],
	},
	'amd64': {
		'Cflags': ['-m64', '-fno-pie', '-fno-pic', '-mcmodel=kernel', '-mno-red-zone',
			'-mno-sse', '-mno-sse2', '-mno-mmx', '-ffixed-r14', '-ffixed-r15',
			'-fplan9-extensions', '-fcommon', '-fno-builtin', '-fno-asynchronous-unwind-tables',
			'-fno-stack-protector', '-fno-omit-frame-pointer', '-fno-zero-initialized-in-bss',
			'-Os', '-gdwarf-2'] + WARN + ['-fno-reorder-functions'],
		'Oflags': ['-T', 'kernel.ld', '-z', 'max-page-size=0x1000'],
		'Libs': [],
	},
	'arm64': {
		# m, up in x27, x26 (dat.h); no FP/SIMD in the kernel (fpu.c saves
		# the user's), no outlined atomics (libgcc's need getauxval).
		# -mcmodel=large: symbol addresses as absolute constants, as 7l
		# emits them; the small model's PC-relative ones are physical in
		# the C code _start runs before the MMU is on (mmu0init: end).
		# -fsigned-char: Plan 9's char is signed (AAPCS64's is not)
		'Cflags': ['-fsigned-char', '-fno-pie', '-fno-pic', '-mcmodel=large', '-mgeneral-regs-only', '-mno-outline-atomics',
			'-ffixed-x26', '-ffixed-x27', '-fplan9-extensions', '-fcommon', '-fno-builtin',
			'-fno-asynchronous-unwind-tables', '-fno-stack-protector', '-fno-omit-frame-pointer',
			'-fno-zero-initialized-in-bss', '-Os', '-gdwarf-2'] + WARN,
		'Oflags': ['-T', 'kernel.ld', '-z', 'max-page-size=0x1000'],
		'Libs': ['libgcc.a'],
	},
}

# GCC port: objects every kernel gets besides the mkfile's: the ELF
# loader of sysexec (port/sysexecelf.c; the gcc-built programs are ELF)
PORTEXTRA = ['sysexecelf.$O']

# per kernel directory: further Pre steps, Clean globs (files of the
# scripts/gen-*-i.sh generators and gen-bootfs.sh)
EXTRA = {
	'arm64': {
		'Clean': ['rebootcode.i', '../port/initcode.i', 'boot', '$CONF.bootfs.paq'],
	},
	'pc': {
		# vgaradeon.c: front includes "/sys/src/cmd/aux/vga/radeon.h"
		'Pre': ['cp ../../cmd/aux/vga/radeon.h radeon.h'],
		'Clean': ['radeon.h', 'rebootcode.i', 'apbootstrap.i', 'etherm10g2k.i', 'etherm10g4k.i',
			'sd53c8xx.i', '../port/initcode.i', 'boot', '$CONF.bootfs.paq'],
	},
}

def mkvars(text):
	"""the mkfile's plain variable definitions (continuations joined,
	comments dropped: arm64's LIB lists a #/$objtype/lib/libfis.a)"""
	text = re.sub(r'#[^\n]*', '', text)
	text = text.replace('\\\n', ' ')
	return {m.group(1): m.group(2).split() for m in re.finditer(r'^(\w+)\s*=([^\n]*)$', text, re.M)}

def readconf(path):
	"""CONF sections: name -> [entry line, words joined by one blank]"""
	sec, out = None, {}
	for l in open(path):
		l = l.rstrip('\n')
		if not l.strip() or l.lstrip().startswith('#'):
			continue
		if not l[0].isspace():
			sec = l.split()[0]
			out.setdefault(sec, [])
			continue
		if sec == 'port':
			out[sec].append(l.strip())
		else:
			out[sec].append(' '.join(l.split('#')[0].split()))
	return out

def devlist(conf):
	"""../port/mkdevlist: the objects the CONF names"""
	objs = set()
	for sec, lines in conf.items():
		if not (sec.startswith('dev') or sec.startswith(('misc', 'link', 'ip'))):
			continue
		for l in lines:
			w = l.split()
			if not w:
				continue
			objs.add(('dev' + w[0]) if sec.startswith('dev') else w[0])
			objs.update(x for x in w[1:] if not re.match(r'[+=-]', x))
	return sorted(objs)

def local(d, text):
	"""the kernel directory's own files: tracked ones, and new ones (GAS
	ports not yet committed) -- but not the copies Pre steps make, files
	named like one in a directory the mkfile's rules copy from"""
	r = subprocess.run(['git', '-C', ROOT, 'ls-files', '--', 'sys/src/9/' + d],
		capture_output=True, text=True, check=True)
	own = {os.path.basename(f) for f in r.stdout.split() if os.path.dirname(f) == 'sys/src/9/' + d}
	dirs = set(re.findall(r'\.\./(\w+)/', text)) - {'port', 'ip', 'boot', d}
	for f in os.listdir(os.path.join(K, d)):
		if f not in own and not any(os.path.exists(os.path.join(K, x, f)) for x in dirs):
			own.add(f)
	return own

def copyrules(text, v, local):
	"""^($VAR)\\.$O:R: '../dir/\\1.c': stem -> ../dir, from ../port/mkfilelist"""
	rules = {}
	for m in re.finditer(r"^\^\(\$(\w+)\)\\\.\$O:R:\s*'?\.\./(\w+)/\\1\.c'?", text, re.M):
		var, d = m.group(1), m.group(2)
		dm = re.search(r'^%s\s*=\s*`\{\.\./port/mkfilelist\s+\.\./(\w+)([^}]*)\}' % var, text, re.M)
		if not dm or dm.group(1) != d:
			sys.exit('genkernel: %s: unknown definition' % var)
		excl = set()
		sm = re.search(r"sed\s+'s/\(([^)]*)\)", dm.group(2))
		if sm:
			excl = set(sm.group(1).split('|'))
		for f in sorted(os.listdir(os.path.join(K, d))):
			s = f[:-2]
			if f.endswith('.c') and f not in local and s not in excl:
				rules.setdefault(s, d)
	return rules

def explicitrules(text):
	"""stem.$O: ../dir/file.[cs] (cache.v8.$O: ../arm64/cache.v8.s)"""
	out = {}
	for m in re.finditer(r'^([\w.]+)\.\$O:\s*(\.\./\w+/[\w.]+\.[cs])\s*$', text, re.M):
		out[m.group(1)] = m.group(2)
	return out

def headerrules(text, v, d):
	"""Pre steps for the header rules of the mkfile"""
	pre = []
	for m in re.finditer(r"^\^\(\$(\w+)\)\\\.h:R:\s*'?\.\./(\w+)/\\1\.h'?", text, re.M):
		var = m.group(1)
		names = []
		for w in v.get(var.replace('REPCH', 'PCHEADERS'), []):
			names.append(w[:-2])
		for n in names:
			pre.append('cp ../%s/%s.h %s.h' % (m.group(2), n, n))
	for m in re.finditer(r'^([\w]+\.h)(?::D)?:\s*(\.\./\w+/[\w.]+\.h)\s*\n\t([^\n]*)', text, re.M):
		t, src, recipe = m.groups()
		if recipe.startswith('echo'):
			pre.append('bash ../../../../scripts/kheader.sh %s %s' % (t, src))
		elif recipe.startswith('touch'):
			pre.append('touch ' + t)
		elif recipe.startswith('cp'):
			pre.append('cp %s %s' % (src, t))
	return pre

def sources(d, text, v, objs):
	own = local(d, text)
	copies = copyrules(text, v, own)
	explicit = explicitrules(text)
	src, pre = [], []
	for o in objs:
		if o.endswith('.$O'):
			o = o[:-3]
		if o in explicit:
			f = explicit[o]
			if f.endswith('.s') and os.path.exists(os.path.join(K, d, f[:-2] + '.S')):
				f = f[:-2] + '.S'
			b = os.path.basename(f)
			if f.endswith('.s'):
				sys.exit('genkernel: %s: %s not ported (no .S)' % (d, f))
			pre.append('cp %s %s' % (f, b))
			src.append(b)
			continue
		for ext in ('.S', '.c'):
			if o + ext in own:
				src.append(o + ext)
				break
		else:
			if o + '.s' in own:
				sys.exit('genkernel: %s: %s.s not ported (no .S)' % (d, o))
			if o in copies:
				pre.append('cp ../%s/%s.c %s.c' % (copies[o], o, o))
				src.append(o + '.c')
			elif os.path.exists(os.path.join(K, 'port', o + '.c')):
				src.append('../port/%s.c' % o)
			elif os.path.exists(os.path.join(K, 'ip', o + '.c')):
				src.append('../ip/%s.c' % o)
			else:
				sys.exit('genkernel: %s: no source for %s.$O' % (d, o))
	return src, pre

def entry(d, conf, text, v, objtype):
	c = readconf(os.path.join(K, d, conf))
	objs = []
	for o in v['OBJ']:
		if o in ('$CONF.root.$O', '$CONF.rootc.$O'):
			continue
		if o == '$DEVS':
			objs += [x + '.$O' for x in devlist(c) if x + '.$O' not in objs]
		elif o == '$PORT':
			objs += [x for x in v['PORT'] + PORTEXTRA if x not in objs]
		elif o not in objs:
			objs.append(o)
	src, pre = sources(d, text, v, objs)
	a = ARCHS[objtype]
	ram = {}
	for l in c.get('bootdir', []):
		w = l.split()
		ram[w[1] if len(w) > 1 else os.path.basename(w[0])] = w[0].replace('$CONF', conf)
	libs = ['/%s/lib.gcc/k%s' % (objtype, os.path.basename(l)) for l in v['LIB']]
	libs += ['/%s/lib.gcc/%s' % (objtype, l) for l in a['Libs']]
	x = EXTRA.get(d, {})
	e = {
		'Name': conf,
		'Program': v['p'][0] + conf,
		'Install': '/%s/bin' % objtype,
		'Pre': headerrules(text, v, d) + pre + x.get('Pre', []),
		'Clean': [p.replace('$CONF', conf) for p in x.get('Clean', [])],
		'Cflags': a['Cflags'],
		'Oflags': a['Oflags'],
		'Libs': libs,
		'Kernel': {
			'Systab': '/sys/src/libc/9syscall/sys.h',
			'ConfFile': '/sys/src/9/%s/%s' % (d, conf),
			'Config': {
				'Code': c.get('port', []),
				'Dev': c.get('dev', []),
				'Link': c.get('link', []),
				'Ip': c.get('ip', []),
				'Misc': c.get('misc', []),
			},
			'Ramfiles': ram,
		},
		'SourceFiles': src,
	}
	if not e['Clean']:
		del e['Clean']
	return e

def main():
	if not sys.argv[1:]:
		sys.exit(__doc__.strip().split('\n')[-2])
	d = sys.argv[1]
	text = open(os.path.join(K, d, 'mkfile')).read()
	v = mkvars(text)
	objtype = v['objtype'][0]
	# CONFLIST may name configurations 9front does not ship (pc: pcf, pccpuf)
	confs = sys.argv[2:] or [c for c in v['CONFLIST'] if os.path.exists(os.path.join(K, d, c))]
	out = [entry(d, c, text, v, objtype) for c in confs]
	path = os.path.join(K, d, 'build.json')
	open(path, 'w').write(json.dumps(out, indent='\t') + '\n')
	for e in out:
		print('%s: %s, %d files' % (os.path.relpath(path, ROOT), e['Name'], len(e['SourceFiles'])))

if __name__ == '__main__':
	main()
