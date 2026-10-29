#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/*
 * mkstemps: the XXXXXX before a suffix of n bytes become a name
 * nobody has (O_EXCL); mkstemp: no suffix. They make their names
 * themselves: mktemp is in libbsd, which libap must not need.
 */
int
mkstemps(char *template, int n)
{
	static char set[] = "abcdefghijklmnopqrstuvwxyz0123456789";
	static unsigned long seq;
	unsigned long x;
	size_t l;
	char *p;
	int i, j, fd;

	l = strlen(template);
	if(n < 0 || l < (size_t)n + 6 || strncmp(template+l-n-6, "XXXXXX", 6) != 0){
		errno = EINVAL;
		return -1;
	}
	p = template+l-n-6;
	for(i = 0; i < 100; i++){
		x = (unsigned long)getpid()*2654435761UL + ++seq*40503UL + (unsigned long)i;
		for(j = 0; j < 6; j++){
			p[j] = set[x % (sizeof set - 1)];
			x /= sizeof set - 1;
		}
		if((fd = open(template, O_RDWR | O_CREAT | O_EXCL, 0600)) >= 0)
			return fd;
		if(errno != EEXIST)
			break;
	}
	memcpy(p, "XXXXXX", 6);
	return -1;
}

int
mkstemp(char *template)
{
	return mkstemps(template, 0);
}
