"""
The architecture the gcc port builds for: $objtype (default amd64), as
9front's /$objtype/mkfile defines it, plus what the host needs for it.

  OBJTYPE   386, amd64, arm64
  O         the object letter of 9front's compilers (8, 6, 7): x.$O,
            $O.out, the program-local libraries' lib*.a$O
  TOOLPREFIX  the gcc/binutils prefix for the build tool
  SUFFIX    ''  for amd64, '.<objtype>' otherwise: generated manifests
            that depend on the architecture are cmds<SUFFIX>.json, so
            the amd64 ones (checked in) keep their names

usage: import arch; arch.OBJTYPE ...
       python3 scripts/arch.py [var]   print (all, or one) as sh assignments
"""
import os
import sys

ARCHS = {
	# objtype: (O, TOOLPREFIX, kernel config(s), qemu)
	'386':	('8', '', 'pc', 'qemu-system-i386'),
	'amd64':	('6', '', 'pc64', 'qemu-system-x86_64'),
	'arm64':	('7', 'aarch64-linux-musl-', 'qemu', 'qemu-system-aarch64'),
}

OBJTYPE = os.environ.get('objtype') or 'amd64'
if OBJTYPE not in ARCHS:
	sys.exit('arch.py: unsupported objtype %s' % OBJTYPE)
O, TOOLPREFIX, KERNEL, QEMU = ARCHS[OBJTYPE]
SUFFIX = '' if OBJTYPE == 'amd64' else '.' + OBJTYPE

def main():
	v = {'objtype': OBJTYPE, 'O': O, 'TOOLPREFIX': TOOLPREFIX, 'KERNEL': KERNEL,
		'QEMU': QEMU, 'SUFFIX': SUFFIX}
	if sys.argv[1:]:
		print(v[sys.argv[1]])
	else:
		for k, x in v.items():
			print("%s='%s'" % (k, x))

if __name__ == '__main__':
	main()
