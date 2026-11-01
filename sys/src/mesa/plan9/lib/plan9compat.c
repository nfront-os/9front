/*
 * SPDX-License-Identifier: MIT
 *
 * What Mesa uses that has no meaning on 9front (headers in ../include):
 * flock, syslog, mmap, priorities. POSIX's own functions are in APE
 * (libap: poll, over select, since the gcc branch).
 */
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <sys/resource.h>

int
flock(int fd, int op)
{
	(void)fd;
	(void)op;
	return 0;
}

static const char *logident = "mesa";

void
openlog(const char *ident, int opt, int fac)
{
	(void)opt;
	(void)fac;
	if(ident != NULL)
		logident = ident;
}

void
syslog(int pri, const char *fmt, ...)
{
	va_list arg;

	(void)pri;
	fprintf(stderr, "%s: ", logident);
	va_start(arg, fmt);
	vfprintf(stderr, fmt, arg);
	va_end(arg);
	fputc('\n', stderr);
}

void
closelog(void)
{
}

void*
mmap(void *a, size_t n, int prot, int flags, int fd, off_t off)
{
	(void)a;
	(void)n;
	(void)prot;
	(void)flags;
	(void)fd;
	(void)off;
	errno = ENOSYS;
	return MAP_FAILED;
}

int
munmap(void *a, size_t n)
{
	(void)a;
	(void)n;
	errno = EINVAL;
	return -1;
}

int
getpriority(int which, int who)
{
	(void)which;
	(void)who;
	errno = 0;
	return 0;
}

int
setpriority(int which, int who, int prio)
{
	(void)which;
	(void)who;
	(void)prio;
	return 0;
}

