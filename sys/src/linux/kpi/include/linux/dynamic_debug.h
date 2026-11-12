/* Public domain. */

#ifndef _LINUX_DYNAMIC_DEBUG_H
#define _LINUX_DYNAMIC_DEBUG_H

#define DECLARE_DYNDBG_CLASSMAP(a, b, c, ...)

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#define DYNDBG_CLASSMAP_TYPE_DISJOINT_BITS 0

#endif
