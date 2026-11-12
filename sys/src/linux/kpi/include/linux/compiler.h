/* Public domain. */

#ifndef _LINUX_COMPILER_H
#define _LINUX_COMPILER_H

#include <linux/kconfig.h>
#include <sys/atomic.h>		/* for READ_ONCE() WRITE_ONCE() */

#define unlikely(x)	__builtin_expect(!!(x), 0)
#define likely(x)	__builtin_expect(!!(x), 1)

#define __force
#define __acquires(x)
#define __releases(x)
#define __read_mostly
#define __iomem
#define __must_check
#define __init
#define __exit
#define __deprecated
#define __nonstring
#define __always_unused	__attribute__((__unused__))
#define __maybe_unused	__attribute__((__unused__))
#define __always_inline	inline __attribute__((__always_inline__))
#define noinline	__attribute__((__noinline__))
#define noinline_for_stack	 __attribute__((__noinline__))
#define fallthrough	do {} while (0)
#define __counted_by(x)
#define __cleanup(fn)	__attribute__((__cleanup__(fn)))

#define __PASTE(x,y) __CONCAT(x,y)

#ifndef __user
#define __user
#endif

#define barrier()	__asm volatile("" : : : "memory")

#define __printf(x, y)	__attribute__((__format__(__printf__,x,y)))	/* 9front: gcc has no __kprintf__ */

/* The Linux code doesn't meet our usual standards! */
#ifdef __clang__
#pragma clang diagnostic ignored "-Winitializer-overrides"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wgnu-variable-sized-type-not-at-end"
#else
#pragma GCC diagnostic ignored "-Wformat-zero-length"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#define __diag_push()
#define __diag_ignore_all(x, y)
#define __diag_pop()

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/compiler_types.h>
#define data_race(e)		(e)
#define __READ_ONCE(x)		READ_ONCE(x)
#define __WRITE_ONCE(x, v)	WRITE_ONCE(x, v)
#define compiletime_assert(c, m)	_Static_assert(c, m)
#define compiletime_assert_atomic_type(t)
#define prevent_tail_call_optimization()	barrier()
#define is_signed_type(t)	(((t)(-1)) < (t)1)
#define is_unsigned_type(t)	(!is_signed_type(t))
#define __ADDRESSABLE(sym)

#endif
