#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

/* 9front has no symbolic links: lstat is stat, no file is a link */

int
lstat(const char *name, struct stat *ans)
{
	return stat(name, ans);
}

int
symlink(const char *name1, const char *name2)
{
	errno = EPERM;
	return -1;
}

ssize_t
readlink(const char *name, char *buf, size_t size)
{
	errno = EINVAL;
	return -1;
}
