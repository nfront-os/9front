/* Public domain. */

#ifndef _LINUX_MINMAX_H
#define _LINUX_MINMAX_H

#include <linux/types.h>

#define swap(a, b) \
	do { __typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while(0)

#define min_t(t, a, b) ({ \
	t __min_a = (a); \
	t __min_b = (b); \
	__min_a < __min_b ? __min_a : __min_b; })

#define max_t(t, a, b) ({ \
	t __max_a = (a); \
	t __max_b = (b); \
	__max_a > __max_b ? __max_a : __max_b; })

#define MIN_T(t, a, b) min_t(t, a, b)
#define MAX_T(t, a, b) max_t(t, a, b)

#define clamp_t(t, x, a, b) min_t(t, max_t(t, x, a), b)
#define clamp(x, a, b) clamp_t(__typeof(x), x, a, b)
#define clamp_val(x, a, b) clamp_t(__typeof(x), x, a, b)

#define min(a, b) MIN(a, b)
#define max(a, b) MAX(a, b)
#define min3(x, y, z) MIN(x, MIN(y, z))
#define max3(x, y, z) MAX(x, MAX(y, z))

#define min_not_zero(a, b) (a == 0) ? b : ((b == 0) ? a : min(a, b))

/*
 * 9front: the array evaluated once, before the loop's counter exists,
 * and the macro's own names: OpenBSD's evaluated _array at each element
 * inside a loop over an int i, so i915's
 * min_array(((unsigned long[]){ ..., page_count - i, ... }), 3) took its
 * page index for the loop's counter (shmem_sg_alloc_table never advanced
 * for a one-page object)
 */
#define __kpi_minmax_array(_op, _array, _nitems)			\
({									\
	typeof(&(_array)[0]) __ma_a = (_array);				\
	typeof(__ma_a[0]) __ma_r = __ma_a[0];				\
	for (size_t __ma_i = 1; __ma_i < (size_t)(_nitems); __ma_i++)	\
		if (__ma_a[__ma_i] _op __ma_r)				\
			__ma_r = __ma_a[__ma_i];			\
	__ma_r;								\
})
#define min_array(_array, _nitems)	__kpi_minmax_array(<, _array, _nitems)
#define max_array(_array, _nitems)	__kpi_minmax_array(>, _array, _nitems)

/* 9front: min/max of the values as unsigned (Linux 6.12) */
#ifndef umin
#define umin(x, y)	((x) + 0u + 0ul + 0ull < (y) + 0u + 0ul + 0ull ? (x) + 0u + 0ul + 0ull : (y) + 0u + 0ul + 0ull)
#define umax(x, y)	((x) + 0u + 0ul + 0ull > (y) + 0u + 0ul + 0ull ? (x) + 0u + 0ul + 0ull : (y) + 0u + 0ul + 0ull)
#endif

#endif
