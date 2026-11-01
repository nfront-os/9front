#!/usr/bin/env python3
"""kbuild.py: the objects a Linux Makefile tree builds with sys/src/linux/kconfig.

  kbuild.py --autoconf	print kpi/include/generated/autoconf.h
  kbuild.py TREE DIR...	print the .c files (relative to TREE) of the
			objects that DIR's Makefiles build, one per line

A small evaluator of Kbuild: VAR := / = / +=, $(CONFIG_X), $(var),
$(addprefix)/$(addsuffix), ifdef/ifndef/ifeq/ifneq/else/endif, include
(amdgpu's Makefile brings in display, pm, ... so), obj-y += x.o (with
x-y / x-objs: its parts) and dir/ (recursion).  $(call ...) and
$(shell ...) give nothing (they are compiler options)."""
import os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
KCONFIG = os.path.join(HERE, '../../sys/src/linux/kconfig')

def config(path=KCONFIG):
	c = {}
	paths = [path]
	if os.environ.get('KPI_AMDGPU'):	# census.py -a
		paths.append(path + '.amdgpu')
	for l in (l for p in paths for l in open(p)):
		l = l.strip()
		if l and not l.startswith('#'):
			k, _, v = l.partition('=')
			c[k] = v
	return c

def lines(path):
	t = open(path).read().replace('\\\n', ' ')
	return [l for l in t.split('\n') if l.strip() and not l.lstrip().startswith('#')]

def func(name, args, v, cfg):
	"""the make functions amdgpu's Makefiles use"""
	a = [x for x in re.split(r',', args, maxsplit=1)]
	if name == 'addprefix' and len(a) == 2:
		return ' '.join(a[0].strip() + w for w in a[1].split())
	if name == 'addsuffix' and len(a) == 2:
		return ' '.join(w + a[0].strip() for w in a[1].split())
	return ''	# $(call ...), $(shell ...): compiler options, not objects

def expand(s, v, cfg):
	for _ in range(16):
		s2 = re.sub(r'\$\((CONFIG_\w+)\)', lambda m: cfg.get(m.group(1), 'n') if cfg.get(m.group(1), 'n') in 'ym' else 'n', s)
		s2 = re.sub(r'\$\((\w[\w-]*)\)', lambda m: v.get(m.group(1), ''), s2)
		# the innermost function calls (no $ in their arguments any more)
		s2 = re.sub(r'\$\((addprefix|addsuffix|call|shell|patsubst|filter|subst|wildcard|foreach|strip)\s+([^$()]*)\)',
			lambda m: func(m.group(1), m.group(2), v, cfg), s2)
		if s2 == s:
			break
		s = s2
	return s

def truth(l, v, cfg):
	"""an ifdef/ifndef/ifeq/ifneq line"""
	m = re.match(r'(ifdef|ifndef)\s+(\S+)', l)
	if m:
		name = m.group(2)
		val = cfg.get(name, 'n') if name.startswith('CONFIG_') else v.get(name, '')
		on = val not in ('', 'n')
		return on if m.group(1) == 'ifdef' else not on
	m = re.match(r'(ifeq|ifneq)\s*\((.*),(.*)\)\s*$', l)
	if m:
		x = expand(m.group(2), v, cfg).strip()
		y = expand(m.group(3), v, cfg).strip()
		if x == 'n':
			x = ''
		if y == 'n':
			y = ''
		return (x == y) if m.group(1) == 'ifeq' else (x != y)
	return False

def evaluate(path, cfg, v=None, tree=None, d=None):
	"""the variables of a Makefile; include brings in others (the same
	variables), the conditionals are kept"""
	if v is None:
		v = {}
		if tree is not None:
			v['srctree'] = tree
			v['src'] = d
			v['obj'] = d
	stack = []	# for each open conditional: is its part active
	for l in lines(path):
		s = l.strip()
		if s.startswith(('ifdef', 'ifndef', 'ifeq', 'ifneq')):
			stack.append(all(stack) and truth(s, v, cfg))
			continue
		if s.startswith('else'):
			if stack:
				outer = all(stack[:-1])
				stack[-1] = outer and not stack[-1]
			continue
		if s.startswith('endif'):
			if stack:
				stack.pop()
			continue
		if not all(stack):
			continue
		if l.startswith('\t'):
			continue
		if s.startswith(('include ', '-include ', 'sinclude ')):
			for inc in expand(s.split(None, 1)[1], v, cfg).split():
				p = os.path.normpath(inc)
				if os.path.isfile(p):
					evaluate(p, cfg, v)
			continue
		m = re.match(r'\s*([\w$()-]+)\s*(:=|\+=|=)\s*(.*)$', l)
		if not m:
			continue
		name = expand(m.group(1), v, cfg)
		val = expand(m.group(3), v, cfg)
		if m.group(2) == '+=':
			v[name] = (v.get(name, '') + ' ' + val).strip()
		else:
			v[name] = val.strip()
	return v

def objects(tree, d, cfg):
	"""the .c files the Makefile in d builds (y and m alike)"""
	mk = os.path.join(tree, d, 'Makefile')
	if not os.path.exists(mk):	# a directory not imported: nothing of it is built
		return []
	v = evaluate(mk, cfg, tree=os.path.abspath(tree), d=d)
	out = []
	def parts(o):
		base = o[:-2]
		p = (v.get(base + '-y', '') + ' ' + v.get(base + '-m', '') + ' ' + v.get(base + '-objs', '')).split()
		if not p:
			return [o]
		r = []
		for x in p:
			r += parts(x) if x != o else [x]
		return r
	for o in (v.get('obj-y', '') + ' ' + v.get('obj-m', '')).split():
		if o.endswith('/'):
			out += objects(tree, os.path.join(d, o[:-1]), cfg)
		elif o.endswith('.o'):
			for p in parts(o):
				c = os.path.normpath(os.path.join(d, p[:-2] + '.c'))
				if c not in out:
					out.append(c)
	return out

def includes(tree, d, cfg):
	"""the -I directories of d's Makefile (ccflags-y, subdir-ccflags-y and
	what it includes): amdgpu's for all of amd/"""
	mk = os.path.join(tree, d, 'Makefile')
	if not os.path.exists(mk):
		return []
	v = evaluate(mk, cfg, tree=os.path.abspath(tree), d=d)
	out = []
	for k in ('ccflags-y', 'subdir-ccflags-y'):
		for w in re.findall(r'-I\s*(\S+)', v.get(k, '')):
			p = os.path.normpath(w)
			if p not in out and os.path.isdir(p):
				out.append(p)
	return out

def autoconf(cfg):
	"""include/generated/autoconf.h: a #define for each option"""
	out = ['/* made by scripts/linux/kbuild.py from sys/src/linux/kconfig */']
	for k, v in cfg.items():
		if v == 'y':
			out.append('#define %s 1' % k)
		elif v == 'm':
			out.append('#define %s_MODULE 1' % k)
		elif v != 'n':
			out.append('#define %s %s' % (k, v))
	return '\n'.join(out) + '\n'

if __name__ == '__main__':
	cfg = config()
	if sys.argv[1:2] == ['--autoconf']:
		sys.stdout.write(autoconf(cfg))
		sys.exit(0)
	for d in sys.argv[2:]:
		for c in objects(sys.argv[1], d, cfg):
			print(c)
