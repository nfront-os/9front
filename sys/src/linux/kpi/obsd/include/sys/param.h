/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: OpenBSD kernel API, sys/param.h */
#ifndef _OBSD_SYS_PARAM_H
#define _OBSD_SYS_PARAM_H

#include <sys/types.h>
#include <sys/limits.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/endian.h>
#include <machine/param.h>

#define NBBY		8
#define MAXCOMLEN	24
#define MAXPATHLEN	1024
#define NODEV		(-1)

#define nitems(a)	(sizeof(a) / sizeof((a)[0]))
#define howmany(x, y)	(((x) + ((y) - 1)) / (y))
#define roundup(x, y)	((((x) + ((y) - 1)) / (y)) * (y))
#define rounddown(x, y)	(((x) / (y)) * (y))
#define powerof2(x)	((((x) - 1) & (x)) == 0)
#ifndef MIN
#define MIN(a, b)	(((a) < (b)) ? (a) : (b))
#define MAX(a, b)	(((a) > (b)) ? (a) : (b))
#endif
#define setbit(a, i)	((a)[(i) / NBBY] |= 1 << ((i) % NBBY))
#define clrbit(a, i)	((a)[(i) / NBBY] &= ~(1 << ((i) % NBBY)))
#define isset(a, i)	((a)[(i) / NBBY] & (1 << ((i) % NBBY)))
#define isclr(a, i)	(((a)[(i) / NBBY] & (1 << ((i) % NBBY))) == 0)

/* sleep priorities (tsleep): only PCATCH means anything here */
#define PSWP	0
#define PVM	4
#define PINOD	8
#define PRIBIO	16
#define PVFS	20
#define PZERO	22
#define PSOCK	24
#define PWAIT	32
#define PLOCK	36
#define PPAUSE	40
#define PUSER	50
#define PCATCH	0x100

#endif
