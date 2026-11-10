/*
 * drawfs: what devdraw.c takes from the kernel, in user space.
 */
#ifdef _POSIX_SOURCE
/* APE's build (sys/src/ape/lib/devdraw: wsys' devdraw, core.c) */
#include <u.h>
#include <libc.h>
#include <setjmp.h>
#include <draw.h>
#include <memdraw.h>
#include <memlayer.h>
#ifndef ERRMAX
#define	ERRMAX	128
#endif
#ifndef QTDIR
#define	QTDIR	0x80
#define	QTFILE	0x00
#endif
typedef struct Ref Ref;
struct Ref
{
	long	ref;
};
#define	incref(r)	__atomic_add_fetch(&(r)->ref, 1, __ATOMIC_SEQ_CST)
#define	decref(r)	__atomic_sub_fetch(&(r)->ref, 1, __ATOMIC_SEQ_CST)
typedef int Rendez;
#else
#include <u.h>
#include <libc.h>
#include <draw.h>
#include <memdraw.h>
#include <memlayer.h>
#include <thread.h>
#endif

/* a channel: what devdraw keeps of an open file (a fid of drawfs.c) */
typedef struct Chan Chan;
struct Chan
{
	Qid	qid;
	int	flag;
	int	mode;
	vlong	offset;
	int	iounit;
};
enum { COPEN = 0x0001 };

/* the kernel's malloc zeroes, its smalloc does not fail */
#define	malloc(n)	mallocz(n, 1)
#define	smalloc(n)	emallocz(n)
extern void	*emallocz(ulong);

/* QLock, Rendez: libc's (the refresh reads wait in drawfs.c); Ref, incref, decref: libthread's (lib9p's), APE's above */
#define	eqlock(l)	qlock(l)
#define	panic	sysfatal
#undef	IOUNIT	/* libc's; devdraw.c has its own */

/* errors: the kernel's waserror, error and nexterror, by setjmp */
enum { NERR = 32 };
extern jmp_buf	errlab[NERR];
extern int	nerrlab;
extern char	errbuf[ERRMAX];
#define	waserror()	(setjmp(errlab[nerrlab++]))
#define	poperror()	(nerrlab--)
extern void	nexterror(void);
extern void	error(char*);

extern char Enomem[], Ebadarg[], Einuse[], Enodev[], Eisdir[], Ebadusefd[];
extern char Eblock[];	/* a refresh read with nothing to read: drawfs.c answers it later */

#define	iprint	print
#define	openmode(m)	((m)&3)
extern long	kreadstr(ulong, char*, ulong, char*);	/* devdraw.c's readstr */

/* the screen and the color map: drawfs.c's */
extern Memdata	*attachscreen(Rectangle*, ulong*, int*, int*, int*);
extern void	flushmemscreen(Rectangle);
extern void	getcolor(ulong, ulong*, ulong*, ulong*);
extern int	setcolor(ulong, ulong, ulong, ulong);
#define	mouseresize()
#define	ishwimage(i)	0
extern void	refreshwake(void);

/* devdraw.c's, for drawfs.c */
extern int	drawinit(void);
extern Chan	*drawopen(Chan*, int);
extern void	drawclose(Chan*);
extern long	drawread(Chan*, void*, long, vlong);
extern long	drawwrite(Chan*, void*, long, vlong);
extern void	drawcmap(void);
extern void	drawflush(void);
extern int	drawclientslot(int);
extern int	drawclientid(int);
