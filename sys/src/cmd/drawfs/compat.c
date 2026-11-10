/*
 * drawfs: the kernel's error machinery and helpers, for devdraw.c
 */
#include "compat.h"

jmp_buf	errlab[NERR];
int	nerrlab;
char	errbuf[ERRMAX];

char Enomem[] = "out of memory";
char Ebadarg[] = "bad arg in system call";
char Einuse[] = "device or object already in use";
char Enodev[] = "no free devices";
char Eisdir[] = "file is a directory";
char Ebadusefd[] = "inappropriate use of fd";
char Eblock[] = "drawfs: would block";

#undef malloc

void*
emallocz(ulong n)
{
	void *v;

	if((v = mallocz(n, 1)) == nil)
		sysfatal("out of memory");
	setmalloctag(v, getcallerpc(&n));
	return v;
}

void
nexterror(void)
{
	if(nerrlab <= 0)
		sysfatal("drawfs: error with no waserror: %s", errbuf);
	longjmp(errlab[--nerrlab], 1);
}

void
error(char *e)
{
	strecpy(errbuf, errbuf+sizeof errbuf, e);
	nexterror();
}

long
kreadstr(ulong off, char *buf, ulong n, char *str)
{
	int size;

	size = strlen(str);
	if(off >= size)
		return 0;
	if(off+n > size)
		n = size-off;
	memmove(buf, str+off, n);
	return n;
}
