/*
 * readlink with POSIX's ssize_t result (unistd.h binds readlink here):
 * 9front has no symbolic links, so no file is one.
 */
#include <errno.h>
#include <sys/types.h>

ssize_t
__apecompat_readlink(const char *path, char *buf, size_t n)
{
	(void)path;
	(void)buf;
	(void)n;
	errno = EINVAL;
	return -1;
}
