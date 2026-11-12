/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: OpenBSD kernel API, sys/cdefs.h: the compiler's attributes under BSD names (gcc) */
#ifndef _OBSD_SYS_CDEFS_H
#define _OBSD_SYS_CDEFS_H

#define __CONCAT(x, y)	x ## y
#define __STRING(x)	#x
#define __packed	__attribute__((__packed__))
#define __aligned(x)	__attribute__((__aligned__(x)))
#define __unused	__attribute__((__unused__))
#define __used		__attribute__((__used__))
#define __dead		__attribute__((__noreturn__))
#define __weak		__attribute__((__weak__))
#define __section(x)	__attribute__((__section__(x)))
#define __pure		__attribute__((__pure__))
#define __malloc	__attribute__((__malloc__))
#define __predict_true(x)	__builtin_expect(((x) != 0), 1)
#define __predict_false(x)	__builtin_expect(((x) != 0), 0)
#define __offsetof(t, m)	__builtin_offsetof(t, m)
#define __DECONST(t, v)	((t)(__uintptr_t)(const void *)(v))
#define __DEVOLATILE(t, v)	((t)(__uintptr_t)(volatile void *)(v))
#define __BEGIN_DECLS
#define __END_DECLS

#endif
