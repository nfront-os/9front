"""scripts/lib/paths.py: where nfront's other parts are, as
scripts/lib/paths.sh finds them (paths.env of the distribution, the
environment first, the old ../9front-* defaults last).

	import paths; paths.get('GCC_DIR')"""
import os

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../..'))
DEFAULTS = {'GCC_DIR': '9front-gcc', 'RUST_DIR': '9front-rust', 'GO_DIR': '9front-go',
	'MCP_DIR': '9front-mcp', 'DISTFILES': '9front-ui/src', 'TREE_GCC': '9front-gccwt',
	'PLAN9PORT': 'plan9port'}

def _load():
	vals = {}
	for f in (os.environ.get('NFRONT_PATHS'), os.path.join(ROOT, '../dist/paths.env'),
	    os.path.join(ROOT, '../9front-distribution/paths.env')):
		if f and os.path.isfile(f):
			top = os.path.dirname(os.path.dirname(os.path.realpath(f)))	# physical: through a symlink too
			for line in open(f):
				line = line.strip()
				if line and not line.startswith('#') and '=' in line:
					k, v = line.split('=', 1)
					vals[k] = os.path.normpath(v if v.startswith('/') else os.path.join(top, v))
			break
	top = os.path.normpath(os.path.join(ROOT, '..'))
	for k, v in DEFAULTS.items():
		vals.setdefault(k, os.path.join(top, v))
	return vals

_vals = _load()

def get(name):
	"""the path name stands for: the environment's, else paths.env's or the default"""
	return os.environ.get(name) or _vals.get(name)

def archive(root, name):
	"""an archive as VERSION or VENDOR.json names it: $DISTFILES/x (the
	distribution's archives), else a path from the tree's root"""
	if name.startswith('$DISTFILES/'):
		return os.path.join(get('DISTFILES'), name[len('$DISTFILES/'):])
	return os.path.normpath(os.path.join(root, name))
