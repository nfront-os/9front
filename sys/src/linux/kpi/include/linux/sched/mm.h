/* Public domain. */

#ifndef _LINUX_SCHED_MM_H
#define _LINUX_SCHED_MM_H

#include <sys/types.h>
#include <sys/systm.h>
#include <sys/malloc.h>

static inline void
might_alloc(const unsigned int flags)
{
	if (flags & M_WAITOK)
		assertwaitok();
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/sched.h>
#define memalloc_noreclaim_save()	0
#define memalloc_noreclaim_restore(f)	do { (void)(f); } while (0)
#define memalloc_nofs_save()		0
#define memalloc_nofs_restore(f)	do { (void)(f); } while (0)
#define memalloc_noio_save()		0
#define memalloc_noio_restore(f)	do { (void)(f); } while (0)
#define fs_reclaim_acquire(g)		do { } while (0)
#define fs_reclaim_release(g)		do { } while (0)

#endif
