/*
 * realpath (POSIX), which APE lacks. 9front has no symbolic links: the
 * absolute name with "." and ".." removed, if the file exists.
 */
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

char*
realpath(const char *path, char *resolved)
{
	char buf[PATH_MAX], *p, *q, *e;
	size_t n;
	struct stat st;

	if(path == NULL){
		errno = EINVAL;
		return NULL;
	}
	if(stat(path, &st) < 0)
		return NULL;
	if(path[0] == '/')
		buf[0] = '\0';
	else if(getcwd(buf, sizeof buf) == NULL)
		return NULL;
	n = strlen(buf);
	if(n + 1 + strlen(path) + 1 > sizeof buf){
		errno = ENAMETOOLONG;
		return NULL;
	}
	buf[n++] = '/';
	strcpy(buf+n, path);

	/* clean in place: q writes, p reads one element at a time */
	q = buf;
	for(p = buf; *p != '\0'; p = e){
		while(*p == '/')
			p++;
		for(e = p; *e != '\0' && *e != '/'; e++)
			;
		if(e == p)
			break;
		if(e-p == 1 && p[0] == '.')
			continue;
		if(e-p == 2 && p[0] == '.' && p[1] == '.'){
			while(q > buf && *--q != '/')
				;
			continue;
		}
		*q++ = '/';
		memmove(q, p, e-p);
		q += e-p;
	}
	if(q == buf)
		*q++ = '/';
	*q = '\0';

	if(resolved == NULL)
		return strdup(buf);
	strcpy(resolved, buf);
	return resolved;
}
