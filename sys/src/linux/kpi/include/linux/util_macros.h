/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/util_macros.h (ours, not Linux's) */
#ifndef _KPI_LINUX_UTIL_MACROS_H
#define _KPI_LINUX_UTIL_MACROS_H

#include <linux/kernel.h>

#define __find_closest(x, a, as, op) ({ typeof(as) __fc_i, __fc_as = (as) - 1; typeof(x) __fc_x = (x); typeof(*a) const *__fc_a = (a); \
	for (__fc_i = 0; __fc_i < __fc_as; __fc_i++) { if (__fc_x op DIV_ROUND_CLOSEST(__fc_a[__fc_i] + __fc_a[__fc_i + 1], 2)) break; } (__fc_i); })
#define find_closest(x, a, as)		__find_closest(x, a, as, <=)
#define find_closest_descending(x, a, as)	__find_closest(x, a, as, >=)

/* 9front: in the for_each macros (Linux 6.11 moved it here from drm_util.h) */
#define for_each_if(condition) if (!(condition)) {} else

#endif
