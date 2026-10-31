/*
 * NIX: newseg(2), after NIX's libseg (MIT license, see /lib/legal/nix):
 * create a global shared segment (segment(3)) unless it exists, attach it.
 * Deviations from NIX: no debug prints; "addr va len" with va 0
 * lets the system pick the address (NIX: va had to be a power of 2
 * or 0); errors are returned, not fatal.
 */
#include <u.h>
#include <libc.h>
#include <seg.h>

enum
{
	Maxname = 80,
	Extra = 20,
};

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
		return nil;
	return p;
}
