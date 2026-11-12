/* Public domain. */

#ifndef _LINUX_POISON_H
#define _LINUX_POISON_H

#define POISON_INUSE	0xdb
#define POISON_FREE	0xdf

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#define POISON_END	0xa5

#endif
