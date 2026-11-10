/*
 * drawfs' core (core.c) for a program that serves draw(3) itself: wsys
 * (src/drawcore.rs). Paths are devdraw's qid paths (slot<<4 | type);
 * a read that has nothing yet (refresh) gives -2; errors in err.
 */
extern int	drawcore_init(void*, int, int, int, int, char*, char*, int);
extern int	drawcore_width(void);
extern void*	drawcore_open(uvlong, int, uvlong*, char*, int);
extern long	drawcore_read(void*, void*, long, vlong, char*, int);
extern long	drawcore_write(void*, void*, long, char*, int);
extern void	drawcore_close(void*);
extern int	drawcore_clientslot(int);
extern int	drawcore_clientid(int);
extern int	drawcore_dirty(int*, int);
extern int	drawcore_wake(void);
extern int	drawcore_stats(char*, int);
