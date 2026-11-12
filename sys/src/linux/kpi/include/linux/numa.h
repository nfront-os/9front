/* Public domain. */

#ifndef _LINUX_NUMA_H
#define _LINUX_NUMA_H

#include <linux/nodemask.h>

#define NUMA_NO_NODE	(-1)

static inline int
dev_to_node(struct device *dev)
{
	return NUMA_NO_NODE;
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/types.h>
#define numa_node_id()	0

#ifndef MAX_NUMNODES
#define MAX_NUMNODES	1	/* 9front: one memory node */
#endif

#endif
