/*
 * POSIX.1-2008's *at functions on 9front for the current directory or
 * an absolute path. A path relative to another directory's descriptor
 * would need that directory's name (fd2path); that is ENOSYS for now.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int
here(int fd, const char *path)
{
	if(fd == AT_FDCWD || path[0] == '/')
		return 1;
	errno = ENOSYS;
	return 0;
}

int
openat(int fd, const char *path, int flags, ...)
{
	va_list arg;
	mode_t mode;

	if(!here(fd, path))
		return -1;
	mode = 0;
	if(flags & O_CREAT){
		va_start(arg, flags);
		mode = va_arg(arg, int);
		va_end(arg);
	}
	return open(path, flags, mode);
}

int
mkfifoat(int fd, const char *path, mode_t mode)
{
	if(!here(fd, path))
		return -1;
	return mkfifo(path, mode);
}
