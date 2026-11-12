/* Public domain. */

#ifndef _LINUX_STRINGIFY_H
#define _LINUX_STRINGIFY_H

#define ___stringify(x...)	#x
#define __stringify(x...)	___stringify(x)

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#define __stringify_1(x...)	#x
#define FILE_LINE		__FILE__ ":" __stringify(__LINE__)

#endif
