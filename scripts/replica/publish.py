#!/usr/bin/env python3
"""
Publish a staged distribution tree as a replica for replica/pull over HTTP
(replica(1); the client side is replica/httpfs, sys/src/cmd/replica/httpfs.c).

The publication directory is served as it is by any HTTP server; it holds

	db		the server's replica db: path - mode uid gid mtime length,
			replica's format (sys/src/cmd/replica/db.c); not read by clients
	sums		path sha256, beside db
	log		replica's log: time gen verb path - mode uid gid mtime length
	obj/xx/sha256	every file's contents (and the log's), by SHA-256: written
			before the index names them; kept while this index or the
			one before names them (a client in the middle of a pull
			still finds its files), then removed
	index		the tree httpfs serves: log, root and root/path, each
			path mode uid gid mtime length sha256 (a directory: sha256 -)
	index.sig	index's RSA signature (PKCS #1 v1.5, SHA-256; openssl)

Each run compares the stage with db and appends to log what changed:
a (added), c (contents: mtime made later than db's, so applylog takes it), m
(mode, uid, gid), d (removed). A file whose contents did not change gets
db's mtime in the stage (os.utime), so the CD made from the stage carries
the same times as the replica: replica/applylog sees an installed file
as unmodified when its mtime and length are the db's.

The stage's metadata (mode, uid, gid as Plan 9 has them) come from
proto.py -l; a path without a line there keeps the stage's permissions
and is owned by sys. Paths the installed system configures are not
replicated (EXCLUDE).

With -c, the stage also gets the client's side, which the CD installs:
/dist/replica/nfront (the replica/pull configuration: URL, the key),
/dist/replica/nfront.pub (the public key) and /dist/replica/client/
(db and log as of this publication: the installed system is up to date).

usage: scripts/replica/publish.py -k key.pem [-c url] [-t time] stage listing pubdir
       scripts/replica/publish.py -g key.pem      (make a key: RSA 4096, private)
"""
import hashlib
import os
import shutil
import stat
import subprocess
import sys
import tempfile
import time

DMDIR = 0x80000000

# configured by the installation or by its users: not replicated
EXCLUDE = ('adm', 'cfg', 'cron', 'usr', 'mail', 'lib/ndb', 'sys/log', 'dist/replica',
	'tmp', 'n', 'mnt', 'gccproto')


def q(s):
	# %q of Plan 9's print (quotestrfmt): quoted when empty or with blanks or quotes
	if s and not any(c <= ' ' or c == "'" for c in s):
		return s
	return "'" + s.replace("'", "''") + "'"


def excluded(path):
	return any(path == e or path.startswith(e + '/') for e in EXCLUDE)


def sha256(path):
	h = hashlib.sha256()
	with open(path, 'rb') as f:
		for b in iter(lambda: f.read(1 << 20), b''):
			h.update(b)
	return h.hexdigest()


def tok(line):
	# tokenize(2): blank-separated fields, '...' quoted with '' for a quote
	out, i, n = [], 0, len(line)
	while True:
		while i < n and line[i] in ' \t\r\n':
			i += 1
		if i >= n:
			return out
		w, quoted = [], False
		while i < n and (quoted or line[i] not in ' \t\r\n'):
			if line[i] == "'":
				if quoted and line[i+1:i+2] == "'":
					w.append("'")
					i += 2
					continue
				quoted = not quoted
				i += 1
				continue
			w.append(line[i])
			i += 1
		out.append(''.join(w))


def readdb(path):
	db = {}
	if os.path.exists(path):
		for line in open(path):
			f = tok(line)
			if len(f) == 7:
				db[f[0]] = (int(f[2], 8), f[3], f[4], int(f[5]), int(f[6]))
	return db


def readsums(path):
	s = {}
	if os.path.exists(path):
		for line in open(path):
			f = tok(line)
			if len(f) == 2:
				s[f[0]] = f[1]
	return s


def stage_entries(stage, listing):
	meta = {}
	for line in open(listing):
		f = line.rstrip('\n').split('\t')
		if len(f) == 4:
			meta[f[0]] = (int(f[1], 8), f[2], f[3])
	ents = {}
	for dirpath, dirs, files in os.walk(stage):
		rel = os.path.relpath(dirpath, stage)
		rel = '' if rel == '.' else rel
		for d in list(dirs):
			p = os.path.join(rel, d)
			if excluded(p) or os.path.islink(os.path.join(dirpath, d)):
				dirs.remove(d)
				continue
			ents[p] = ('d', None)
		for n in files:
			p = os.path.join(rel, n)
			full = os.path.join(dirpath, n)
			if excluded(p) or os.path.islink(full):
				continue
			ents[p] = ('f', full)
	out = {}
	for p, (kind, full) in ents.items():
		st = os.stat(os.path.join(stage, p))
		if p in meta:
			mode, uid, gid = meta[p]
		else:
			mode, uid, gid = stat.S_IMODE(st.st_mode) & 0o777, 'sys', 'sys'
		if kind == 'd':
			mode |= DMDIR
		else:
			mode &= ~DMDIR
		out[p] = (mode, uid, gid, int(st.st_mtime), 0 if kind == 'd' else st.st_size, full)
	return out


def putobj(pub, src, sha):
	d = os.path.join(pub, 'obj', sha[:2])
	dst = os.path.join(d, sha)
	if os.path.exists(dst):
		return
	os.makedirs(d, exist_ok=True)
	tmp = dst + '.tmp'
	if os.path.exists(tmp):
		os.remove(tmp)
	try:	# a link where it can (the stage is made anew each time, never written in place)
		os.link(src, tmp)
	except OSError:
		shutil.copyfile(src, tmp)
		os.chmod(tmp, 0o644)
	os.rename(tmp, dst)


def writeatomic(path, text):
	tmp = path + '.tmp'
	with open(tmp, 'w') as f:
		f.write(text)
	os.rename(tmp, path)


def pubkey(key):
	return subprocess.run(['openssl', 'rsa', '-in', key, '-RSAPublicKey_out'],
		check=True, capture_output=True, text=True).stdout


def main():
	args = sys.argv[1:]
	key = url = None
	now = int(time.time())
	while args and args[0].startswith('-'):
		a = args.pop(0)
		if a == '-g':
			k = args.pop(0)
			if os.path.exists(k):
				sys.exit('publish: %s exists' % k)
			old = os.umask(0o077)
			subprocess.run(['openssl', 'genrsa', '-out', k, '4096'], check=True)
			os.umask(old)
			print(pubkey(k), end='')
			return
		elif a == '-k':
			key = args.pop(0)
		elif a == '-c':
			url = args.pop(0)
		elif a == '-t':
			now = int(args.pop(0))
		else:
			sys.exit(__doc__)
	if len(args) != 3 or key is None:
		sys.exit(__doc__)
	stage, listing, pub = args
	os.makedirs(pub, exist_ok=True)
	db = readdb(os.path.join(pub, 'db'))
	sums = readsums(os.path.join(pub, 'sums'))
	cur = stage_entries(stage, listing)

	# the log's time only goes forward (applylog's -T, the client's db)
	logf = os.path.join(pub, 'log')
	if os.path.exists(logf):
		for line in open(logf):
			t = int(line.split()[0])
			if t >= now:
				now = t + 1
	gen = 0
	entries = []

	def log(verb, path, mode, uid, gid, mtime, length):
		nonlocal gen
		entries.append('%d %d %s %s - %o %s %s %d %d\n' % (now, gen, verb, q(path), mode, q(uid), q(gid), mtime, length))
		gen += 1

	newdb, newsums = {}, {}
	nadd = nchg = nmeta = ndel = 0
	# removed (or a file become a directory and back): deepest first
	for p in sorted(db, reverse=True):
		if p not in cur or (db[p][0] & DMDIR) != (cur[p][0] & DMDIR):
			m = db[p]
			log('d', p, m[0], m[1], m[2], now, m[4])
			ndel += 1
	for p in sorted(cur):
		mode, uid, gid, mtime, length, full = cur[p]
		old = db.get(p)
		if old is not None and (old[0] & DMDIR) != (mode & DMDIR):
			old = None
		if mode & DMDIR:
			if old is None:
				log('a', p, mode, uid, gid, mtime, 0)
				newdb[p] = (mode, uid, gid, mtime, 0)
				nadd += 1
			else:
				if old[:3] != (mode, uid, gid):
					log('m', p, mode, uid, gid, old[3], 0)
					nmeta += 1
				newdb[p] = (mode, uid, gid, old[3], 0)
			continue
		sha = sha256(full)
		putobj(pub, full, sha)
		newsums[p] = sha
		if old is None:
			log('a', p, mode, uid, gid, mtime, length)
			nadd += 1
		elif sums.get(p) != sha:
			mtime = max(mtime, old[3] + 1)
			log('c', p, mode, uid, gid, mtime, length)
			nchg += 1
		else:
			mtime = old[3]
			if old[:3] != (mode, uid, gid):
				log('m', p, mode, uid, gid, mtime, length)
				nmeta += 1
		os.utime(full, (mtime, mtime))
		newdb[p] = (mode, uid, gid, mtime, length)

	# the log, then db and sums, then the signed index naming them
	with open(logf, 'a') as f:
		f.writelines(entries)
	dbtext = ''.join('%s - %o %s %s %d %d\n' % (q(p), m[0], q(m[1]), q(m[2]), m[3], m[4])
		for p, m in sorted(newdb.items()))
	writeatomic(os.path.join(pub, 'db'), dbtext)
	writeatomic(os.path.join(pub, 'sums'), ''.join('%s %s\n' % (q(p), s) for p, s in sorted(newsums.items())))
	logsha = sha256(logf)
	putobj(pub, logf, logsha)
	idx = ['log %o sys sys %d %d %s\n' % (0o644, now, os.path.getsize(logf), logsha),
		'root %o sys sys %d 0 -\n' % (DMDIR | 0o775, now)]
	for p, m in sorted(newdb.items()):
		idx.append('%s %o %s %s %d %d %s\n' % (q('root/' + p), m[0], q(m[1]), q(m[2]), m[3], m[4],
			'-' if m[0] & DMDIR else newsums[p]))
	# the objects to keep: the new index's and the previous one's
	keep = set(l.split()[-1] for l in idx)
	oldidx = os.path.join(pub, 'index')
	if os.path.exists(oldidx):
		keep |= set(tok(l)[-1] for l in open(oldidx) if len(tok(l)) == 7)
	tmp = os.path.join(pub, 'index.tmp')
	with open(tmp, 'w') as f:
		f.writelines(idx)
	subprocess.run(['openssl', 'dgst', '-sha256', '-sign', key, '-out', tmp + '.sig', tmp], check=True)
	os.rename(tmp + '.sig', os.path.join(pub, 'index.sig'))
	os.rename(tmp, os.path.join(pub, 'index'))
	nrm = 0
	for d in os.listdir(os.path.join(pub, 'obj')):
		for o in os.listdir(os.path.join(pub, 'obj', d)):
			if o not in keep:
				os.remove(os.path.join(pub, 'obj', d, o))
				nrm += 1
	print('publish: %s: %d added, %d changed, %d metadata, %d removed; %d entries; %d old objects removed' %
		(pub, nadd, nchg, nmeta, ndel, len(newdb), nrm))

	if url:
		# the client: the CD installs it with the system
		rd = os.path.join(stage, 'dist', 'replica')
		os.makedirs(os.path.join(rd, 'client'), exist_ok=True)
		with open(os.path.join(rd, 'nfront.pub'), 'w') as f:
			f.write(pubkey(key))
		with open(os.path.join(rd, 'nfront'), 'w') as f:
			f.write(CONFIG % {'url': url})
		os.chmod(os.path.join(rd, 'nfront'), 0o755)
		shutil.copyfile(os.path.join(pub, 'db'), os.path.join(rd, 'client', 'nfront.db'))
		shutil.copyfile(logf, os.path.join(rd, 'client', 'nfront.log'))


CONFIG = '''#!/bin/rc
# nfront's updates: the replica pull(1) brings in (replica(1)), served over
# HTTP by %(url)s; replica/httpfs checks its index's signature with
# /dist/replica/nfront.pub and each file against the index
url=%(url)s
s=/n/replica.nfront
serverroot=$s/root
serverlog=$s/log
fn servermount {
	if(! test -e /mnt/web/clone)
		webfs
	replica/httpfs -k /dist/replica/nfront.pub -m $s $url
}
clientroot=/
clientdb=/dist/replica/client/nfront.db
clientlog=/dist/replica/client/nfront.log
'''

if __name__ == '__main__':
	main()
