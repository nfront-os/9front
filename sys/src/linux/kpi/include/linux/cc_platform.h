/* Public domain. */

#ifndef _LINUX_CC_PLATFORM_H
#define _LINUX_CC_PLATFORM_H

#include <linux/types.h>

#define CC_ATTR_MEM_ENCRYPT		1
#define CC_ATTR_GUEST_MEM_ENCRYPT	0

static inline bool
cc_platform_has(int x)
{
	return false;
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>

#endif
