/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: OpenBSD kernel API, pool(9): fixed-size items from malloc (kpi/obsd/malloc.c) */
#ifndef _OBSD_SYS_POOL_H
#define _OBSD_SYS_POOL_H

#include <sys/types.h>
#include <sys/mutex.h>

#define PR_WAITOK	0x0001
#define PR_NOWAIT	0x0002
#define PR_LIMITFAIL	0x0004
#define PR_ZERO		0x0008
#define PR_WAITOK_ZERO	(PR_WAITOK | PR_ZERO)

struct pool {
	const char	*pr_wchan;
	size_t		pr_size;
	size_t		pr_align;
	int		pr_ipl;
	unsigned int	pr_nout;	/* items given out */
	struct spinlock	pr_mtx;
	void		*pr_free;	/* items given back (kpi/obsd/malloc.c) */
};

void	pool_init(struct pool *, size_t, unsigned int, int, int, const char *, void *);
void	pool_setipl(struct pool *, int);
void	pool_destroy(struct pool *);
void	*pool_get(struct pool *, int);
void	pool_put(struct pool *, void *);

#endif
