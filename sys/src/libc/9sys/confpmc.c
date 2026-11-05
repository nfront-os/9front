/*
 * configure performance counter index of core, pmc(2), pmc(3);
 * after NIX's libc/port/confpmc.c (Lsub, Bell Labs, Sandia, Vita Nuova;
 * MIT license, see /lib/legal/nix). NIX found the processor we run on
 * with getcore(2), in the Tos; here #ε/core tells.
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

int
confpmc(int core, int index, int mode, char *desc)
{
	int fd;
	char name[40];

	if(core < 0 && (core = mycore()) < 0)
		return -1;
	snprint(name, sizeof(name), "#ε/core%4.4d/ctr%2.2dctl", core, index);
	if((fd = open(name, OWRITE)) < 0)
		return -1;
	if(fprint(fd, "reset\n") < 0)
		goto Err;
	if(mode&PmcOs)
	if(fprint(fd, "os\n") < 0)
		goto Err;
	if(mode&PmcUser)
	if(fprint(fd, "user\n") < 0)
		goto Err;
	if(desc != nil)
	if(fprint(fd, "set %s\n", desc) < 0)
		goto Err;
	if(mode&PmcEnable)
	if(fprint(fd, "enable\n") < 0)
		goto Err;
	close(fd);
	return 0;
Err:
	close(fd);
	return -1;
}
