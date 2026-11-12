/* Public domain. */

#ifndef _LINUX_BUILD_BUG_H
#define _LINUX_BUILD_BUG_H

#include <linux/compiler.h>

#define BUILD_BUG()
/*
 * 9front: an error when the condition stays true after optimising (the
 * Linux code has conditions, __builtin_constant_p ones too, that gcc
 * decides only then): a call to a function that is an error if made
 */
#define BUILD_BUG_ON(x) do {						\
	extern void __kpi_build_bug(void)				\
	    __attribute__((__error__("BUILD_BUG_ON failed")));		\
	if (x)								\
		__kpi_build_bug();					\
} while (0)
#define BUILD_BUG_ON_NOT_POWER_OF_2(x)	0
#define BUILD_BUG_ON_MSG(x, y)		do { } while (0)
#define BUILD_BUG_ON_INVALID(x)		((void)0)
#define BUILD_BUG_ON_ZERO(x)		0

#define static_assert(x, ...)

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#define __static_assert(expr, msg, ...)	_Static_assert(expr, msg)
#define ASSERT_STRUCT_OFFSET(type, field, expected_offset)	static_assert(offsetof(type, field) == (expected_offset))

#endif
