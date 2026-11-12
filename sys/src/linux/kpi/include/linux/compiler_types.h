/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: linux/compiler_types.h, ours.  Only the names that
 * vendor/, replace/ or kpi/ use, for gcc.  The annotations of Linux's
 * checkers (sparse, lockdep, KCSAN, the randomizer) mean nothing here
 * and are empty; the rest are gcc attributes and builtins.
 */
#ifndef _KPI_LINUX_COMPILER_TYPES_H
#define _KPI_LINUX_COMPILER_TYPES_H

/* checker annotations: address spaces, lock contexts, RCU, bounds */
#ifndef __user
#define __user
#endif
#ifndef __iomem
#define __iomem
#endif
#ifndef __force
#define __force
#endif
#ifndef __rcu
#define __rcu
#endif
#ifndef __acquires
#define __acquires(lock)
#endif
#ifndef __releases
#define __releases(lock)
#endif
#ifndef __counted_by
#define __counted_by(member)
#endif
#ifndef __nonstring
#define __nonstring
#endif
#ifndef __deprecated
#define __deprecated
#endif

/* placement: no init sections, no read-mostly segment on 9front */
#ifndef __init
#define __init
#endif
#ifndef __exit
#define __exit
#endif
#ifndef __initconst
#define __initconst
#endif
#ifndef __read_mostly
#define __read_mostly
#endif
#ifndef __ro_after_init
#define __ro_after_init
#endif

/* gcc's attributes under Linux's names */
#ifndef __always_inline
#define __always_inline	inline __attribute__((__always_inline__))
#endif
#ifndef noinline
#define noinline	__attribute__((__noinline__))
#endif
#ifndef __inline
#define __inline	inline
#endif
#ifndef __maybe_unused
#define __maybe_unused	__attribute__((__unused__))
#endif
#ifndef __always_unused
#define __always_unused	__maybe_unused
#endif
#ifndef __unused
#define __unused	__maybe_unused
#endif
#ifndef __used
#define __used	__attribute__((__used__))
#endif
#ifndef __must_check
#define __must_check	__attribute__((__warn_unused_result__))
#endif
#ifndef __packed
#define __packed	__attribute__((__packed__))
#endif
#ifndef __aligned
#define __aligned(n)	__attribute__((__aligned__(n)))
#endif
#ifndef __section
#define __section(name)	__attribute__((__section__(name)))
#endif
#ifndef __pure
#define __pure	__attribute__((__pure__))
#endif
#ifndef __noreturn
#define __noreturn	__attribute__((__noreturn__))
#endif
#ifndef __weak
#define __weak	__attribute__((__weak__))
#endif
#ifndef __malloc
#define __malloc	__attribute__((__malloc__))
#endif
#ifndef __printf
#define __printf(fmt, args)	__attribute__((__format__(__printf__, fmt, args)))
#endif
#ifndef __mode
#define __mode(m)	__attribute__((__mode__(m)))
#endif
#ifndef __cleanup
#define __cleanup(fn)	__attribute__((__cleanup__(fn)))
#endif
#ifndef fallthrough
#define fallthrough	__attribute__((__fallthrough__))
#endif

/* gcc's diagnostics pragmas: the drivers' few uses go without */
#ifndef __diag_push
#define __diag_push()
#endif
#ifndef __diag_pop
#define __diag_pop()
#endif
#ifndef __diag_ignore_all
#define __diag_ignore_all(option, why)
#endif

/* branch hints, barriers */
#ifndef likely
#define likely(c)	__builtin_expect(!!(c), 1)
#endif
#ifndef unlikely
#define unlikely(c)	__builtin_expect(!!(c), 0)
#endif
#ifndef barrier
#define barrier()	__asm__ __volatile__("" : : : "memory")
#endif
#ifndef barrier_data
/* a barrier that also makes gcc assume *p is read */
#define barrier_data(p)	__asm__ __volatile__("" : : "r"(p) : "memory")
#endif
#ifndef OPTIMIZER_HIDE_VAR
/* gcc forgets what it knew of v's value */
#define OPTIMIZER_HIDE_VAR(v)	__asm__("" : "=r"(v) : "0"(v))
#endif
#ifndef unreachable
#define unreachable()	__builtin_unreachable()
#endif

/* names and types at compile time */
#ifndef __PASTE
#define __kpi_paste(a, b)	a##b
#define __PASTE(a, b)	__kpi_paste(a, b)
#endif
#ifndef __UNIQUE_ID
#define __UNIQUE_ID(prefix)	__PASTE(__PASTE(__kpi_unique_, prefix), __COUNTER__)
#endif
#ifndef __same_type
#define __same_type(a, b)	__builtin_types_compatible_p(__typeof__(a), __typeof__(b))
#endif
#ifndef __must_be_array
#define __must_be_array(a)	0
#endif
#ifndef __is_constexpr
/*
 * 1 if x is an integer constant expression: then (void *)(x * 0) is a
 * null pointer constant and the conditional has the type int *, else
 * it has void * (sizeof 1 as an extension of gcc's)
 */
#define __is_constexpr(x)	\
	(sizeof(int) == sizeof(*(1 ? (void *)((long)(x) * 0l) : (int *)1)))
#endif

/* 9front: true only for a constant that is true (Linux 6.15) */
#define const_true(x)	__builtin_choose_expr(__is_constexpr(x), (x), 0)

/* 9front: no __counted_by here, so no counter to find (Linux 6.15) */
#ifndef __flex_counter
#define __flex_counter(member)	((void *)0)
#endif

#endif
