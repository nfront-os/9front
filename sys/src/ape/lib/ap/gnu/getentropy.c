/*
 * POSIX.1-2024's getentropy: n (at most 256) random bytes from the
 * kernel's generator (/dev/random: 9front's, which does not block);
 * expat seeds its hashes so (HAVE_GETENTROPY), else it would warn of
 * poor entropy.
 */
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stddef.h>

int
getentropy(void *buf, size_t n)
{
	char *p;
	int fd, m;

	if(n > 256){
		errno = EINVAL;
		return -1;
	}
	if((fd = open("/dev/random", O_RDONLY)) < 0)
		return -1;
	for(p = buf; n > 0; p += m, n -= m)
		if((m = read(fd, p, n)) <= 0){
			close(fd);
			if(m == 0)
				errno = EIO;
			return -1;
		}
	close(fd);
	return 0;
}
