/*
 * httpfs: a replica's server tree over HTTP, read-only, for
 * replica/pull (replica(1)'s servermount).
 *
 * The server publishes, under one base URL (scripts/replica/publish.py):
 *	index		every file of the tree served here:
 *			path mode uid gid mtime length sha256
 *			(quoted as tokenize(2) reads; a directory has sha256 -)
 *	index.sig	index's RSA signature (PKCS #1 v1.5, SHA-256)
 *	obj/xx/sha256	the files' contents, by their SHA-256
 *
 * httpfs fetches the index through webfs(4), checks its signature with the
 * public key (PEM, RSA PUBLIC KEY) and serves the tree it describes. A
 * file's contents come from obj/ as they are read; a read that would end
 * the file checks the SHA-256 of all of it first and fails if it differs,
 * so replica/applylog (which spools a file before installing it) never
 * installs a file other than the one the signed index names. webfs does
 * not check TLS certificates: the signature is what makes HTTP safe.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include <mp.h>
#include <libsec.h>

typedef struct Ent Ent;
typedef struct Rd Rd;

struct Ent {
	uchar	sha[SHA2_256dlen];
	vlong	length;
};

/* a fid's read of a file: the connection to its object, how far, the hash so far */
struct Rd {
	int	cfd;	/* webfs clone (the connection's ctl) */
	int	bfd;	/* its body */
	vlong	pos;
	DigestState *ds;
	int	done;	/* checked: the whole file read and its hash right */
};

static char *base;
static char *web = "/mnt/web";
static int debug;

static void
usage(void)
{
	fprint(2, "usage: replica/httpfs [-D] [-m mtpt] [-s srvname] [-w webmtpt] -k pubkey url\n");
	exits("usage");
}

/* open a webfs connection to url: its clone file and body */
static int
webopen(char *url, int *cfd)
{
	char buf[64], *s;
	int n, fd;

	s = smprint("%s/clone", web);
	*cfd = open(s, ORDWR);
	free(s);
	if(*cfd < 0)
		return -1;
	if((n = read(*cfd, buf, sizeof buf-1)) <= 0){
		close(*cfd);
		return -1;
	}
	buf[n] = 0;
	if(fprint(*cfd, "url %s", url) < 0){
		close(*cfd);
		return -1;
	}
	s = smprint("%s/%d/body", web, atoi(buf));
	fd = open(s, OREAD);
	free(s);
	if(fd < 0){
		close(*cfd);
		return -1;
	}
	return fd;
}

/* the whole of base/name, NUL-terminated */
static char*
fetch(char *name, long *np)
{
	char *url, *b;
	long n, m;
	int cfd, fd;

	url = smprint("%s/%s", base, name);
	fd = webopen(url, &cfd);
	if(fd < 0){
		werrstr("%s: %r", url);
		free(url);
		return nil;
	}
	b = nil;
	n = 0;
	for(;;){
		b = erealloc9p(b, n + 8192 + 1);
		if((m = read(fd, b+n, 8192)) < 0){
			werrstr("%s: %r", url);
			free(b);
			b = nil;
			break;
		}
		if(m == 0)
			break;
		n += m;
	}
	close(fd);
	close(cfd);
	free(url);
	if(b != nil){
		b[n] = 0;
		*np = n;
	}
	return b;
}

static RSApub*
readkey(char *file)
{
	char *s, *p;
	uchar *der;
	int fd, len;
	long n;
	Dir *d;
	RSApub *k;

	if((fd = open(file, OREAD)) < 0 || (d = dirfstat(fd)) == nil)
		sysfatal("%s: %r", file);
	n = d->length;
	free(d);
	s = emalloc9p(n+1);
	if(readn(fd, s, n) != n)
		sysfatal("read %s: %r", file);
	s[n] = 0;
	close(fd);
	p = s;
	if((der = decodePEM(p, "RSA PUBLIC KEY", &len, nil)) == nil)
		sysfatal("%s: no RSA PUBLIC KEY", file);
	if((k = asn1toRSApub(der, len)) == nil)
		sysfatal("%s: bad RSA public key", file);
	free(der);
	free(s);
	return k;
}

static int
dehex(char *s, uchar *b, int n)
{
	int i, c, v;

	if(strlen(s) != 2*n)
		return -1;
	for(i = 0; i < 2*n; i++){
		c = s[i];
		if(c >= '0' && c <= '9')
			v = c - '0';
		else if(c >= 'a' && c <= 'f')
			v = c - 'a' + 10;
		else
			return -1;
		if(i&1)
			b[i/2] |= v;
		else
			b[i/2] = v<<4;
	}
	return 0;
}

static void
mktree(Tree *t, char *index)
{
	char *line, *next, *f[8], *p, *name;
	int nf, lineno;
	ulong mode;
	File *dir, *fp;
	Ent *e;

	lineno = 0;
	for(line = index; *line; line = next){
		lineno++;
		if((next = strchr(line, '\n')) == nil)
			sysfatal("index: line %d: no newline", lineno);
		*next++ = 0;
		nf = tokenize(line, f, nelem(f));
		if(nf != 7)
			sysfatal("index: line %d: %d fields", lineno, nf);
		name = f[0];
		if(name[0] == '/' || strstr(name, "..") != nil || name[0] == 0)
			sysfatal("index: line %d: bad path %q", lineno, name);
		mode = strtoul(f[1], 0, 8);
		if((p = strrchr(name, '/')) != nil){
			*p++ = 0;
			incref(t->root);	/* walkfile closes the file it starts from */
			if((dir = walkfile(t->root, name)) == nil)
				sysfatal("index: line %d: no directory %q", lineno, name);
		}else{
			p = name;
			dir = t->root;
			incref(dir);
		}
		e = nil;
		if((mode & DMDIR) == 0){
			e = emalloc9p(sizeof *e);
			if(dehex(f[6], e->sha, sizeof e->sha) < 0)
				sysfatal("index: line %d: bad sha256 %q", lineno, f[6]);
			e->length = strtoll(f[5], 0, 10);
		}
		if((fp = createfile(dir, p, f[2], mode, e)) == nil)
			sysfatal("index: line %d: %q: %r", lineno, p);
		free(fp->gid);
		fp->gid = estrdup9p(f[3]);
		free(fp->muid);
		fp->muid = estrdup9p(f[2]);
		fp->mtime = fp->atime = strtoul(f[4], 0, 10);
		if(e != nil)
			fp->length = e->length;
		closefile(fp);
		closefile(dir);
	}
}

static void
rdclose(Rd *rd)
{
	if(rd->bfd >= 0)
		close(rd->bfd);
	if(rd->cfd >= 0)
		close(rd->cfd);
	rd->bfd = rd->cfd = -1;
	free(rd->ds);
	rd->ds = nil;
	rd->pos = 0;
}

static int
rdopen(Rd *rd, Ent *e)
{
	char *url, sha[2*SHA2_256dlen+1];
	int i;

	rdclose(rd);
	for(i = 0; i < SHA2_256dlen; i++)
		snprint(sha+2*i, 3, "%.2x", e->sha[i]);
	url = smprint("%s/obj/%.2s/%s", base, sha, sha);
	rd->bfd = webopen(url, &rd->cfd);
	if(rd->bfd < 0){
		werrstr("%s: %r", url);
		free(url);
		return -1;
	}
	free(url);
	rd->ds = sha2_256(nil, 0, nil, nil);
	return 0;
}

/*
 * n bytes of the object at rd->pos into buf, hashed; the read that
 * reaches the end checks the hash before handing out its bytes
 */
static long
rdread(Rd *rd, Ent *e, char *buf, long n)
{
	uchar h[SHA2_256dlen];
	long m, got;

	if(n > e->length - rd->pos)
		n = e->length - rd->pos;
	for(got = 0; got < n; got += m){
		if((m = read(rd->bfd, buf+got, n-got)) < 0)
			return -1;
		if(m == 0){
			werrstr("object short: %lld of %lld bytes", rd->pos+got, e->length);
			return -1;
		}
	}
	sha2_256((uchar*)buf, n, nil, rd->ds);
	rd->pos += n;
	if(rd->pos == e->length){
		sha2_256(nil, 0, h, rd->ds);
		rd->ds = nil;
		if(tsmemcmp(h, e->sha, sizeof h) != 0){
			werrstr("contents differ from the signed index (sha256)");
			return -1;
		}
		rd->done = 1;
		close(rd->bfd);
		close(rd->cfd);
		rd->bfd = rd->cfd = -1;
	}
	return n;
}

static void
fsopen(Req *r)
{
	Rd *rd;

	if((r->ifcall.mode&3) == OWRITE || (r->ifcall.mode&3) == ORDWR || (r->ifcall.mode & (OTRUNC|ORCLOSE))){
		respond(r, "read-only file system");
		return;
	}
	if((r->fid->qid.type & QTDIR) == 0){
		rd = emalloc9p(sizeof *rd);
		rd->cfd = rd->bfd = -1;
		r->fid->aux = rd;
	}
	respond(r, nil);
}

static void
fsread(Req *r)
{
	Rd *rd;
	Ent *e;
	vlong off;
	long n, m;
	char *skip;
	char err[ERRMAX];

	rd = r->fid->aux;
	e = r->fid->file->aux;
	off = r->ifcall.offset;
	n = r->ifcall.count;
	if(off >= e->length){
		/* the end: an empty file (or one read to its end) is checked here */
		if(!rd->done && rd->pos == 0 && e->length == 0){
			if(rd->ds == nil)
				rd->ds = sha2_256(nil, 0, nil, nil);
			if(rdread(rd, e, nil, 0) < 0)
				goto Err;
		}
		r->ofcall.count = 0;
		respond(r, nil);
		return;
	}
	/* reads are sequential (fcp's are in order); back: from the start again */
	if(rd->bfd < 0 || off < rd->pos)
		if(rdopen(rd, e) < 0)
			goto Err;
	skip = nil;
	while(rd->pos < off){
		if(skip == nil)
			skip = emalloc9p(8192);
		m = off - rd->pos;
		if(m > 8192)
			m = 8192;
		if(rdread(rd, e, skip, m) < 0){
			free(skip);
			goto Err;
		}
	}
	free(skip);
	if((n = rdread(rd, e, r->ofcall.data, n)) < 0)
		goto Err;
	r->ofcall.count = n;
	respond(r, nil);
	return;
Err:
	rerrstr(err, sizeof err);
	rdclose(rd);
	respond(r, err);
}

static void
fsdestroyfid(Fid *fid)
{
	Rd *rd;

	if((rd = fid->aux) != nil){
		rdclose(rd);
		free(rd);
		fid->aux = nil;
	}
}

Srv fs = {
	.open=		fsopen,
	.read=		fsread,
	.destroyfid=	fsdestroyfid,
};

void
main(int argc, char **argv)
{
	char *mtpt, *srvname, *keyfile, *index, *sig, *err;
	uchar h[SHA2_256dlen];
	long nindex, nsig;
	RSApub *key;

	mtpt = "/n/replica";
	srvname = nil;
	keyfile = nil;
	ARGBEGIN{
	case 'D':
		chatty9p++;
		debug++;
		break;
	case 'k':
		keyfile = EARGF(usage());
		break;
	case 'm':
		mtpt = EARGF(usage());
		break;
	case 's':
		srvname = EARGF(usage());
		break;
	case 'w':
		web = EARGF(usage());
		break;
	default:
		usage();
	}ARGEND
	if(argc != 1 || keyfile == nil)
		usage();
	base = estrdup9p(argv[0]);
	while(strlen(base) > 0 && base[strlen(base)-1] == '/')
		base[strlen(base)-1] = 0;
	key = readkey(keyfile);

	if((index = fetch("index", &nindex)) == nil)
		sysfatal("%r");
	if((sig = fetch("index.sig", &nsig)) == nil)
		sysfatal("%r");
	sha2_256((uchar*)index, nindex, h, nil);
	if((err = X509rsaverifydigest((uchar*)sig, nsig, h, sizeof h, key)) != nil)
		sysfatal("%s/index: signature does not verify with %s: %s", base, keyfile, err);
	if(strlen(index) != nindex)
		sysfatal("index: NUL byte");
	free(sig);
	rsapubfree(key);

	fs.tree = alloctree("sys", "sys", DMDIR|0555, nil);
	mktree(fs.tree, index);
	free(index);
	postmountsrv(&fs, srvname, mtpt, MREPL);
	exits(nil);
}
