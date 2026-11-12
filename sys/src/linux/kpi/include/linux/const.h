/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/const.h (ours, not Linux's) */
#ifndef _KPI_LINUX_CONST_H
#define _KPI_LINUX_CONST_H

#define _AC(X, Y)	(X##Y)
#define _AT(T, X)	((T)(X))
#define _UL(x)		_AC(x, UL)
#define _ULL(x)		_AC(x, ULL)
#define U64_C(x)	x ## ULL
#define U32_C(x)	x ## U
#define UL(x)		(_UL(x))
#define ULL(x)		(_ULL(x))
#define __KERNEL_DIV_ROUND_UP(n, d)	(((n) + ((d) - 1)) / (d))

#endif
