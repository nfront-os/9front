#include "lib.h"
#include <string.h>
#include <stdlib.h>
#include "sys9.h"
#include "dir.h"

/*
 * Format:
  3 r  M    4 (0000000000457def 11 00)   8192      512 /rc/lib/rcmain
 */

static int
getfields(char *str, char **args, int max, int mflag)
{
	char r;
	int intok, narg;

	if(max <= 0)
		return 0;

	narg = 0;
	args[narg] = str;
	if(!mflag)
		narg++;
	intok = 0;
	for(;; str++) {
		r = *str;
		if(r == 0)
			break;
		if(r == ' ' || r == '\t'){
			if(narg >= max)
				break;
			*str = 0;
			intok = 0;
			args[narg] = str + 1;
			if(!mflag)
				narg++;
		} else {
			if(!intok && mflag)
				narg++;
			intok = 1;
		}
	}
	return narg;
}
/* "#d/<fd>ctl", without libfmt's snprint: libap must not need libfmt */
static void
ctlname(char *buf, int fd)
{
	char num[16], *p;
	unsigned u;

	p = num + sizeof num;
	*--p = 0;
	u = fd;
	do
		*--p = '0' + u%10;
	while((u /= 10) != 0);
	strcpy(buf, "#d/");
	strcat(buf, p);
	strcat(buf, "ctl");
}

int
_IOUNIT(int fd)
{
	int i, cfd;
	char buf[128], *args[10];

	ctlname(buf, fd);
	cfd = _OPEN(buf, OREAD);
	if(cfd < 0)
		return 0;
	i = _READ(cfd, buf, sizeof buf-1);
	_CLOSE(cfd);
	if(i <= 0)
		return 0;
	buf[i] = '\0';
	if(getfields(buf, args, 10, 1) != 10)
		return 0;
	return atoi(args[7]);
}
