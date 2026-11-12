/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: OpenBSD kernel API, atomic_*(9) and membar_*(9), on gcc's __atomic builtins */
#ifndef _OBSD_SYS_ATOMIC_H
#define _OBSD_SYS_ATOMIC_H

#include <sys/types.h>

#define _OBSD_ATOMIC(name, type)					\
static inline type							\
atomic_cas_##name(volatile type *p, type o, type n)			\
{									\
	__atomic_compare_exchange_n(p, &o, n, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST); \
	return o;							\
}									\
static inline type							\
atomic_swap_##name(volatile type *p, type n)				\
{									\
	return __atomic_exchange_n(p, n, __ATOMIC_SEQ_CST);		\
}									\
static inline void							\
atomic_inc_##name(volatile type *p)					\
{									\
	__atomic_add_fetch(p, 1, __ATOMIC_SEQ_CST);			\
}									\
static inline void							\
atomic_dec_##name(volatile type *p)					\
{									\
	__atomic_sub_fetch(p, 1, __ATOMIC_SEQ_CST);			\
}									\
static inline void							\
atomic_add_##name(volatile type *p, type v)				\
{									\
	__atomic_add_fetch(p, v, __ATOMIC_SEQ_CST);			\
}									\
static inline void							\
atomic_sub_##name(volatile type *p, type v)				\
{									\
	__atomic_sub_fetch(p, v, __ATOMIC_SEQ_CST);			\
}									\
static inline type							\
atomic_inc_##name##_nv(volatile type *p)				\
{									\
	return __atomic_add_fetch(p, 1, __ATOMIC_SEQ_CST);		\
}									\
static inline type							\
atomic_dec_##name##_nv(volatile type *p)				\
{									\
	return __atomic_sub_fetch(p, 1, __ATOMIC_SEQ_CST);		\
}									\
static inline type							\
atomic_add_##name##_nv(volatile type *p, type v)			\
{									\
	return __atomic_add_fetch(p, v, __ATOMIC_SEQ_CST);		\
}									\
static inline type							\
atomic_sub_##name##_nv(volatile type *p, type v)			\
{									\
	return __atomic_sub_fetch(p, v, __ATOMIC_SEQ_CST);		\
}

_OBSD_ATOMIC(int, unsigned int)
_OBSD_ATOMIC(uint, unsigned int)
_OBSD_ATOMIC(long, unsigned long)
_OBSD_ATOMIC(ulong, unsigned long)
_OBSD_ATOMIC(uint64, uint64_t)
#undef _OBSD_ATOMIC

static inline void *
atomic_cas_ptr(volatile void *p, void *o, void *n)
{
	__atomic_compare_exchange_n((void *volatile *)p, &o, n, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
	return o;
}

static inline void *
atomic_swap_ptr(volatile void *p, void *n)
{
	return __atomic_exchange_n((void *volatile *)p, n, __ATOMIC_SEQ_CST);
}

static inline void
atomic_setbits_int(volatile unsigned int *p, unsigned int v)
{
	__atomic_or_fetch(p, v, __ATOMIC_SEQ_CST);
}

static inline void
atomic_clearbits_int(volatile unsigned int *p, unsigned int v)
{
	__atomic_and_fetch(p, ~v, __ATOMIC_SEQ_CST);
}

static inline void
atomic_setbits_long(volatile unsigned long *p, unsigned long v)
{
	__atomic_or_fetch(p, v, __ATOMIC_SEQ_CST);
}

static inline void
atomic_clearbits_long(volatile unsigned long *p, unsigned long v)
{
	__atomic_and_fetch(p, ~v, __ATOMIC_SEQ_CST);
}

/* single loads and stores the compiler keeps (OpenBSD has them here) */
#define READ_ONCE(x)		(*(const volatile __typeof(x) *)&(x))
#define WRITE_ONCE(x, v)	do { *(volatile __typeof(x) *)&(x) = (v); } while (0)

#define __membar(s)	__asm volatile(s ::: "memory")

#define membar_enter()		__atomic_thread_fence(__ATOMIC_SEQ_CST)
#define membar_exit()		__atomic_thread_fence(__ATOMIC_SEQ_CST)
#define membar_producer()	__atomic_thread_fence(__ATOMIC_RELEASE)
#define membar_consumer()	__atomic_thread_fence(__ATOMIC_ACQUIRE)
#define membar_sync()		__atomic_thread_fence(__ATOMIC_SEQ_CST)
#define membar_enter_after_atomic()	membar_enter()
#define membar_exit_before_atomic()	membar_exit()

#endif
