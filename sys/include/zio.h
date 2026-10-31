/*
 * NIX: zero-copy I/O, zread(2), zp(2) (MIT license, see /lib/legal/nix).
 * NIX declared these in libc.h; here a header of their own,
 * as zread and zwrite are names of functions in 9front programs.
 */
typedef struct Zio Zio;
struct Zio
{
	void*	data;
	ulong	size;
};

extern	void	zfree(Zio[], int);
extern	int	zp(int*);
extern	int	zpread(int, Zio[], int, usize, vlong);
extern	int	zpwrite(int, Zio[], int, vlong);
extern	int	zread(int, Zio[], int, usize);
extern	int	zwrite(int, Zio[], int);
