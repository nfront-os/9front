/* truncate (POSIX), which APE lacks: over its ftruncate */
#include <fcntl.h>
#include <unistd.h>

int
truncate(const char *path, off_t len)
{
	int fd, r;

	fd = open(path, O_WRONLY);
	if(fd < 0)
		return -1;
	r = ftruncate(fd, len);
	close(fd);
	return r;
}
