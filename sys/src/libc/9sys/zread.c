/* NIX: zread(2), from the NIX libc (MIT license, see /lib/legal/nix) */
#include	<u.h>
#include	<libc.h>
#include	<zio.h>

int
zread(int fd, Zio io[], int nio, usize count)
{
	return zpread(fd, io, nio, count, -1LL);
}
