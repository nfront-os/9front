#!/usr/bin/env python3
"""kbuild.py: the objects a Linux Makefile tree builds with sys/src/linux/kconfig.

  kbuild.py --autoconf	print kpi/include/generated/autoconf.h
  kbuild.py TREE DIR...	print the .c files (relative to TREE) of the
			objects that DIR's Makefiles build, one per line

A small evaluator of Kbuild: VAR := / = / +=, $(CONFIG_X), $(var),
obj-y += x.o (with x-y / x-objs: its parts) and dir/ (recursion).
$(call ...), include and ifdef lines are ignored."""
import os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
KCONFIG = os.path.join(HERE, '../../sys/src/linux/kconfig')

def config(path=KCONFIG):
	c = {}
	for l in open(path):
		l = l.strip()
		if l and not l.startswith('#'):
			k, _, v = l.partition('=')
			c[k] = v
	return c

def lines(path):
	t = open(path).read().replace('\\\n', ' ')
	return [l for l in t.split('\n') if l.strip() and not l.lstrip().startswith('#')]

def expand(s, v, cfg):
	for _ in range(8):
		s2 = re.sub(r'\$\((CONFIG_\w+)\)', lambda m: cfg.get(m.group(1), 'n') if cfg.get(m.group(1), 'n') in 'ym' else 'n', s)
		s2 = re.sub(r'\$\((\w[\w-]*)\)', lambda m: v.get(m.group(1), ''), s2)
		if s2 == s:
			break
		s = s2
	return s

def evaluate(path, cfg):
	v = {}
	for l in lines(path):
		if l.startswith(('ifdef', 'ifndef', 'endif', 'else', 'include', 'ifeq', 'ifneq', '\t')) or '$(call' in l or '$(shell' in l or '$(patsubst' in l:
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
	v = evaluate(mk, cfg)
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
