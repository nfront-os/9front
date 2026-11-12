/* Public domain. */

#ifndef _XEN_XEN_H
#define _XEN_XEN_H

static inline bool
xen_initial_domain(void)
{
	return false;
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#define xen_domain()	0
#define xen_pv_domain()	0

#endif
