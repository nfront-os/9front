/*
 * read performance counter index of the core we run on,
 * pmc(2), pmc(3); after NIX's libc/port/rdpmc.c (Lsub, Bell Labs,
 * Sandia, Vita Nuova; MIT license, see /lib/legal/nix).
 * Through the file on every architecture: NIX's amd64 version
 * used RDPMC, which faults where the kernel did not allow it.
 * NIX found the processor with getcore(2), in the Tos; here
 * #ε/core tells.
 */
#include <u.h>
#include <libc.h>

/* the processor we run on, from #ε/core */
static int
mycore(void)
{
	int fd, n;
	char buf[16];

	if((fd = open("#ε/core", OREAD)) < 0)
		return -1;
	n = read(fd, buf, sizeof(buf) - 1);
	close(fd);
	if(n <= 0)
		return -1;
	buf[n] = '\0';
	return atoi(buf);
}

uvlong
rdpmc(int index)
{
	int fd, n, core;
	char name[40];

	if((core = mycore()) < 0)
		return ~0ULL;
	snprint(name, sizeof(name), "#ε/core%4.4d/ctr%2.2d", core, index);
	if((fd = open(name, OREAD)) < 0)
		return ~0ULL;
	n = read(fd, name, sizeof(name) - 1);
	close(fd);
	if(n <= 0)
		return ~0ULL;
	name[n] = '\0';
	return strtoull(name, nil, 0);
}
