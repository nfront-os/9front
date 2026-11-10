/*
 * GCC port: APE's libmemdraw (built from /sys/src/libmemdraw): its image
 * memory, libc's pool imagmem natively, from malloc here (pool.c);
 * nothing moves it.
 */
typedef struct Pool Pool;
struct Pool {
	char	*name;
	void	(*move)(void*, void*);
};

extern Pool	*imagmem;
extern void	*poolalloc(Pool*, ulong);
extern void	poolfree(Pool*, void*);
