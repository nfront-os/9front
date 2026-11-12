/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: the part of the OpenBSD kernel's API that
 * OpenBSD's LinuxKPI headers (kpi/include, from sys/src/linux/openbsd)
 * build on, here on 9front (kpi/obsd/*.c, kpi/plan9).  sys/types.h:
 * amd64, LP64 as gcc compiles the Linux side.
 */
#ifndef _OBSD_SYS_TYPES_H
#define _OBSD_SYS_TYPES_H

#include <sys/cdefs.h>
#include <sys/_null.h>

typedef signed char		int8_t;
typedef unsigned char		uint8_t;
typedef short			int16_t;
typedef unsigned short		uint16_t;
typedef int			int32_t;
typedef unsigned int		uint32_t;
typedef long long		int64_t;
typedef unsigned long long	uint64_t;
typedef uint8_t		u_int8_t;
typedef uint16_t	u_int16_t;
typedef uint32_t	u_int32_t;
typedef uint64_t	u_int64_t;
typedef long			intptr_t;
typedef unsigned long		uintptr_t;
typedef unsigned long		__uintptr_t;
typedef long long		intmax_t;
typedef unsigned long long	uintmax_t;
typedef long			__ptrdiff_t;
typedef long			ptrdiff_t;
typedef unsigned long		size_t;
typedef long			ssize_t;
typedef long long		off_t;
typedef long long		quad_t;
typedef unsigned long long	u_quad_t;

typedef unsigned char	u_char;
typedef unsigned short	u_short;
typedef unsigned int	u_int;
typedef unsigned long	u_long;
typedef unsigned char	unchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef unsigned long	ulong;

typedef unsigned long	paddr_t;
typedef unsigned long	psize_t;
typedef unsigned long	vaddr_t;
typedef unsigned long	vsize_t;
typedef char		*caddr_t;
typedef int		dev_t;
typedef int		pid_t;
typedef unsigned int	uid_t;
typedef unsigned int	gid_t;
typedef unsigned int	mode_t;
typedef long long	time_t;
typedef int		clockid_t;
typedef int		register_t;

#define offsetof(s, m)	__builtin_offsetof(s, m)

typedef _Bool	bool;	/* a type, not OpenBSD's macro: the Linux code pastes the word bool */
#define true	1
#define false	0

#endif
