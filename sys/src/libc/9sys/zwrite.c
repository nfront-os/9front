/* NIX: zread(2), from the NIX libc (MIT license, see /lib/legal/nix) */
#include	<u.h>
#include	<libc.h>
#include	<zio.h>

int
zwrite(int fd, Zio io[], int nio)
{
	return zpwrite(fd, io, nio, -1LL);
}
