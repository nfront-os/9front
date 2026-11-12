/* Public domain. */

#ifndef _LINUX_KMEMLEAK_H
#define _LINUX_KMEMLEAK_H

static inline void
kmemleak_update_trace(const void *p)
{
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#define kmemleak_ignore(p)	do { } while (0)
#define kmemleak_alloc(p, s, c, g)	do { } while (0)
#define kmemleak_free(p)	do { } while (0)

#endif
