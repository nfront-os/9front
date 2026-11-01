/*
 * NIX: newseg(2), after NIX's libseg (MIT license, see /lib/legal/nix):
 * create a global shared segment (segment(3)) unless it exists, attach it.
 * Deviations from NIX: no debug prints; "addr va len" with va 0
 * lets the system pick the address (NIX: va had to be a power of 2
 * or 0); errors are returned, not fatal; a segment attached already
 * (also before an exec) gives its address.
 */
#include <u.h>
#include <libc.h>
#include <seg.h>

enum
{
	Maxname = 80,
	Extra = 20,
};

/*
 * segattach failed: the segment may be attached already, also from
 * before an exec (9front keeps a global segment over exec, whatever
 * segattach's attr said). Its address from its ctl ("va base len"),
 * then is there a segment of ours at it (/proc/n/segment)?
 */
static void*
attached(char *sname, char *s)
{
	char buf[4096], err[ERRMAX], *f[8], *l, *e;
	uvlong base, size;
	int fd, n;

	rerrstr(err, sizeof err);
	strecpy(s, sname+Maxname+Extra, "/ctl");
	fd = open(sname, OREAD);
	*s = 0;
	if(fd < 0)
		goto Err;
	n = read(fd, buf, sizeof buf-1);
	close(fd);
	if(n <= 0)
		goto Err;
	buf[n] = 0;
	if(tokenize(buf, f, nelem(f)) < 3 || strcmp(f[0], "va") != 0)
		goto Err;
	base = strtoull(f[1], nil, 0);
	size = strtoull(f[2], nil, 0);
	snprint(buf, sizeof buf, "/proc/%d/segment", getpid());
	if((fd = open(buf, OREAD)) < 0)
		goto Err;
	n = readn(fd, buf, sizeof buf-1);
	close(fd);
	if(n <= 0)
		goto Err;
	buf[n] = 0;
	for(l = buf; l < buf+n; l = e+1){
		if((e = strchr(l, '\n')) == nil)
			e = buf+n;
		*e = 0;
		if(tokenize(l, f, nelem(f)) >= 3 && strtoull(f[1], nil, 16) == base
		&& strtoull(f[2], nil, 16) - base == size)
			return (void*)base;
	}
Err:
	werrstr("%s", err);
	return nil;
}

void*
newseg(char *name, uvlong va, ulong len)
{
	static int once;
	int fd, cfd;
	char sname[Maxname+Extra], *s;
	void *p;

	/* race, but ok */
	if(once++ == 0)
		if(bind("#g", "/mnt/seg", MREPL|MCREATE) < 0){
			once = 0;
			return nil;
		}
	s = seprint(sname, sname+Maxname, "/mnt/seg/%s", name);
	if(s >= sname+Maxname-1){
		werrstr("name too long");
		return nil;
	}
	if(access(sname, AEXIST) < 0){
		fd = create(sname, OREAD, 0640|DMDIR);
		if(fd < 0)
			return nil;
		close(fd);
		strecpy(s, sname+sizeof sname, "/ctl");
		cfd = open(sname, OWRITE);
		*s = 0;
		if(cfd < 0)
			return nil;
		if(fprint(cfd, "addr %#llux %#lux\n", va, len) < 0){
			close(cfd);
			return nil;
		}
		close(cfd);
	}
	/* a global segment is attached at its own address (segattach(2)) */
	p = segattach(SG_CEXEC, name, (void*)va, len);
	if(p == (void*)-1)
		return attached(sname, s);
	return p;
}
