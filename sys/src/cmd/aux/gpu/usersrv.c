/*
 * SPDX-License-Identifier: MIT
 *
 * aux/amdgpu, aux/i915: gpu(4) for several users (each driver's fs.c
 * includes this file).  A user's requests are the user's because of the
 * srv file they came through, not because of the name in their attach
 * (anyone who opens a srv file can send any name; there is no
 * factotum here to prove one): /srv/srvname is the hostowner's (0600),
 * and "user name" on the driver's ctl posts /srv/srvname.name, the
 * user's (0600).  Every fid of an attach through it is that user's
 * (srvattach), a context is its creator's (its files 0600), and so are
 * the segments the driver makes for it (segown).
 */

/* is uid the hostowner? (as the kernel's iseve) */
static int
iseve9p(char *uid)
{
	static char eve[64];
	int fd, n;

	if(eve[0] == 0 && (fd = open("/dev/hostowner", OREAD)) >= 0){
		n = read(fd, eve, sizeof eve - 1);
		close(fd);
		eve[n > 0 ? n : 0] = 0;
	}
	return uid != nil && eve[0] != 0 && strcmp(uid, eve) == 0;
}

/* every fid of the attach is the srv's user (Srv.aux) */
static void
srvattach(Req *r)
{
	char *u;

	if((u = r->srv->aux) != nil){
		free(r->fid->uid);
		r->fid->uid = estrdup9p(u);
	}
	respond(r, nil);
}

/* the file (/srv's, #g's) the user's, mode */
static int
chownfile(char *path, char *user, ulong mode)
{
	Dir d;

	nulldir(&d);
	d.uid = user;
	d.mode = mode;
	return dirwstat(path, &d);
}

/*
 * a srv like proto for user, posted as /srv/srvname.user (the user's,
 * 0600); nil or the error
 */
static char*
postuser(Srv *proto, char *srvname, char *user)
{
	static char err[ERRMAX];
	char name[64], path[80];
	Srv *s;

	if(user == nil || *user == 0 || strchr(user, '/') != nil || strchr(user, '.') != nil)
		return "bad user name";
	snprint(name, sizeof name, "%s.%s", srvname, user);
	snprint(path, sizeof path, "/srv/%s", name);
	if(access(path, AEXIST) == 0)
		return nil;	/* posted already */
	s = emalloc9p(sizeof *s);
	memset(s, 0, sizeof *s);
	s->tree = proto->tree;
	s->attach = srvattach;
	s->open = proto->open;
	s->read = proto->read;
	s->write = proto->write;
	s->destroyfid = proto->destroyfid;
	s->aux = estrdup9p(user);
	threadpostsrv(s, name);
	if(chownfile(path, user, 0600) < 0){
		rerrstr(err, sizeof err);
		remove(path);
		return err;
	}
	return nil;
}

/* a segment the driver made for a client of user: the user's alone */
static void
segown(char *seg, char *user)
{
	char path[64];

	if(seg == nil || *seg == 0 || user == nil || strcmp(user, getuser()) == 0)
		return;
	/* the segment's owner and mode are its files' (its directory takes no wstat) */
	snprint(path, sizeof path, "#g/%s/ctl", seg);
	if(chownfile(path, user, 0600) < 0)
		fprint(2, "%s: segment %s to %s: %r\n", argv0, seg, user);
}
