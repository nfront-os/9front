/*
 * sysconf with what APE's lacks (<unistd.h>: link name
 * __apecompat_sysconf): the number of processors, the lines of
 * /dev/sysstat (cons(3): one a processor), the page size, and the
 * pages of memory (/dev/swap: "N memory", "N pagesize"); the rest is
 * APE's.
 */
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern long __apesysconf(int) __asm__("sysconf");	/* libap's */

static long
nprocs(void)
{
	char buf[1024];
	long n;
	int fd, i, k;

	fd = open("/dev/sysstat", O_RDONLY);
	if(fd < 0)
		return 1;
	n = 0;
	while((k = read(fd, buf, sizeof buf)) > 0)
		for(i = 0; i < k; i++)
			if(buf[i] == '\n')
				n++;
	close(fd);
	return n > 0 ? n : 1;
}

/* the value of the line "N name" of /dev/swap, -1 if none */
static long long
swapval(const char *name)
{
	char buf[1024], *s, *e;
	size_t l, n;
	int fd, k;

	if((fd = open("/dev/swap", O_RDONLY)) < 0)
		return -1;
	k = read(fd, buf, sizeof buf - 1);
	close(fd);
	if(k <= 0)
		return -1;
	buf[k] = 0;
	l = strlen(name);
	for(s = buf; *s; s = e){
		if((e = strchr(s, '\n')) != NULL)
			*e++ = 0;
		else
			e = s + strlen(s);
		n = strlen(s);
		if(n > l && s[n-l-1] == ' ' && strcmp(s+n-l, name) == 0)
			return strtoll(s, NULL, 10);
	}
	return -1;
}

long
sysconf(int name)
{
	switch(name){
	case _SC_NPROCESSORS_CONF:
	case _SC_NPROCESSORS_ONLN:
		return nprocs();
	case _SC_PAGESIZE:
		return 4096;
	case _SC_PHYS_PAGES:{
		long long mem;

		mem = swapval("memory");
		return mem > 0 ? mem / 4096 : -1;
	}
	}
	return __apesysconf(name);
}

/* BSD's getpagesize (libbacktrace's mmap): sysconf's page size */
int
getpagesize(void)
{
	return 4096;
}
