/* $OpenBSD: atomic.h,v 1.34 2026/08/19 01:34:10 jsg Exp $ */
/**
 * \file drm_atomic.h
 * Atomic operations used in the DRM which may or may not be provided by the OS.
 * 
 * \author Eric Anholt <anholt@FreeBSD.org>
 */

/*-
 * Copyright 2004 Eric Anholt
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * VA LINUX SYSTEMS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _DRM_LINUX_ATOMIC_H_
#define _DRM_LINUX_ATOMIC_H_

#include <sys/types.h>
#include <sys/mutex.h>
#include <machine/intr.h>
#include <linux/types.h>
#include <linux/compiler.h>	/* via x86/include/asm/atomic.h */

/*
 * 9front: atomic_t, atomic64_t and atomic_long_t are Linux's structs
 * with a counter (linux/types.h): the Linux code here is unchanged and
 * uses .counter (i915_vma.h); OpenBSD changes it and takes plain ints.
 * The operations work on the counter, all three types alike.
 */
#define ATOMIC_INIT(x)		{ (x) }
#define __AC(p)			(&(p)->counter)

#define atomic_set(p, v)	WRITE_ONCE((p)->counter, (v))
#define atomic_read(p)		READ_ONCE((p)->counter)
#define atomic_inc(p)		((void)__sync_fetch_and_add(__AC(p), 1))
#define atomic_dec(p)		((void)__sync_fetch_and_sub(__AC(p), 1))
#define atomic_add(n, p)	((void)__sync_fetch_and_add(__AC(p), n))
#define atomic_sub(n, p)	((void)__sync_fetch_and_sub(__AC(p), n))
#define atomic_and(n, p)	((void)__sync_fetch_and_and(__AC(p), n))
#define atomic_or(n, p)		((void)__sync_fetch_and_or(__AC(p), n))
#define atomic_xor(n, p)	((void)__sync_fetch_and_xor(__AC(p), n))
#define atomic_add_return(n, p) __sync_add_and_fetch(__AC(p), n)
#define atomic_sub_return(n, p) __sync_sub_and_fetch(__AC(p), n)
#define atomic_sub_and_test(n, p)	(atomic_sub_return(n, p) == 0)
#define atomic_inc_return(v)	atomic_add_return(1, (v))
#define atomic_dec_return(v)	atomic_sub_return(1, (v))
#define atomic_dec_and_test(v)	(atomic_dec_return(v) == 0)
#define atomic_inc_and_test(v)	(atomic_inc_return(v) == 0)
#define atomic_cmpxchg(p, o, n)	__sync_val_compare_and_swap(__AC(p), o, n)
#define cmpxchg(p, o, n)	__sync_val_compare_and_swap(p, o, n)
#define cmpxchg64(p, o, n)	__sync_val_compare_and_swap(p, o, n)
#define cmpxchg_relaxed(p, o, n)	cmpxchg(p, o, n)
#define atomic_set_release(p, v)	atomic_set((p), (v))
#define atomic_read_acquire(p)		smp_load_acquire(__AC(p))
#define atomic_andnot(bits, p)		((void)__sync_fetch_and_and(__AC(p), ~(bits)))
#define atomic_fetch_inc(p)		__sync_fetch_and_add(__AC(p), 1)
#define atomic_fetch_add(n, p)		__sync_fetch_and_add(__AC(p), n)
#define atomic_fetch_sub(n, p)		__sync_fetch_and_sub(__AC(p), n)
#define atomic_fetch_and(n, p)		__sync_fetch_and_and(__AC(p), n)
#define atomic_fetch_or(n, p)		__sync_fetch_and_or(__AC(p), n)
#define atomic_fetch_xor(n, p)		__sync_fetch_and_xor(__AC(p), n)
#define atomic_fetch_andnot(n, p)	__sync_fetch_and_and(__AC(p), ~(n))

#define try_cmpxchg(p, op, n)						\
({									\
	__typeof(p) __op = (__typeof((p)))(op);				\
	__typeof(*(p)) __o = *__op;					\
	__typeof(*(p)) __p = __sync_val_compare_and_swap((p), (__o), (n)); \
	if (__p != __o)							\
		*__op = __p;						\
	(__p == __o);							\
})

#define atomic_try_cmpxchg(p, op, n)	try_cmpxchg(__AC(p), op, n)
#define try_cmpxchg64(p, op, n)	try_cmpxchg(p, op, n)	/* 9front: from OpenBSD's 6.18 */

#define atomic_xchg(p, n)						\
({									\
	__sync_synchronize();						\
	__sync_lock_test_and_set(__AC(p), n);				\
})

#define xchg(v, n)	__sync_lock_test_and_set(v, n)

#define atomic_add_unless(v, n, u)					\
({									\
	__typeof((v)->counter) __o, __u = (u);				\
	int __r = 1;							\
	do {								\
		__o = READ_ONCE((v)->counter);				\
		if (__o == __u) {					\
			__r = 0;					\
			break;						\
		}							\
	} while (__sync_val_compare_and_swap(__AC(v), __o, __o + (n)) != __o); \
	__r;								\
})

#define atomic_inc_not_zero(v)	atomic_add_unless((v), 1, 0)

static inline int
atomic_dec_if_positive(atomic_t *v)
{
	int r, o;

	do {
		o = v->counter;
		r = o - 1;
		if (r < 0)
			break;
	} while (__sync_val_compare_and_swap(&v->counter, o, r) != o);

	return r;
}

#define ATOMIC64_INIT(x)	{ (x) }

#define atomic64_set(p, v)		atomic_set(p, v)
#define atomic64_read(p)		atomic_read(p)
#define atomic64_xchg(v, n)		atomic_xchg(v, n)
#define atomic64_cmpxchg(v, o, n)	atomic_cmpxchg(v, o, n)
#define atomic64_try_cmpxchg(v, o, n)	atomic_try_cmpxchg(v, o, n)
#define atomic64_add(n, p)		atomic_add(n, p)
#define atomic64_sub(n, p)		atomic_sub(n, p)
#define atomic64_inc(p)			atomic_inc(p)
#define atomic64_dec(p)			atomic_dec(p)
#define atomic64_add_return(n, p)	atomic_add_return(n, p)
#define atomic64_sub_return(n, p)	atomic_sub_return(n, p)
#define atomic64_inc_return(p)		atomic_inc_return(p)
#define atomic64_fetch_add(n, p)	atomic_fetch_add(n, p)
#define atomic64_or(n, p)		atomic_or(n, p)
#define atomic64_and(n, p)		atomic_and(n, p)
#define atomic64_fetch_or(n, p)		atomic_fetch_or(n, p)
#define atomic64_add_unless(v, n, u)	atomic_add_unless(v, n, u)

#define atomic_long_set(p, v)		atomic_set(p, v)
#define atomic_long_read(p)		atomic_read(p)
#define atomic_long_xchg(v, n)		atomic_xchg(v, n)
#define atomic_long_cmpxchg(p, o, n)	atomic_cmpxchg(p, o, n)
#define atomic_long_try_cmpxchg(p, o, n)	atomic_try_cmpxchg(p, o, n)
#define atomic_long_add(i, v)		atomic_add(i, v)
#define atomic_long_sub(i, v)		atomic_sub(i, v)
#define atomic_long_inc(v)		atomic_inc(v)
#define atomic_long_dec(v)		atomic_dec(v)
#define atomic_long_add_return(i, v)	atomic_add_return(i, v)
#define atomic_long_sub_return(i, v)	atomic_sub_return(i, v)
#define atomic_long_inc_return(v)	atomic_inc_return(v)
#define atomic_long_dec_return(v)	atomic_dec_return(v)
#define atomic_long_dec_and_test(v)	atomic_dec_and_test(v)
#define atomic_long_inc_not_zero(v)	atomic_inc_not_zero(v)
#define atomic_long_add_unless(v, n, u)	atomic_add_unless(v, n, u)
#define atomic_long_fetch_add(n, v)	atomic_fetch_add(n, v)

static inline int
test_and_set_bit(u_int b, volatile void *p)
{
	unsigned int m = 1 << (b & 0x1f);
	unsigned int prev = __sync_fetch_and_or((volatile u_int *)p + (b >> 5), m);
	return (prev & m) != 0;
}

static inline void
clear_bit(u_int b, volatile void *p)
{
	atomic_clearbits_int(((volatile u_int *)p) + (b >> 5), 1 << (b & 0x1f));
}

static inline void
clear_bit_unlock(u_int b, volatile void *p)
{
	membar_enter();
	clear_bit(b, p);
}

static inline void
set_bit(u_int b, volatile void *p)
{
	atomic_setbits_int(((volatile u_int *)p) + (b >> 5), 1 << (b & 0x1f));
}

static inline void
__clear_bit(u_int b, volatile void *p)
{
	volatile u_int *ptr = (volatile u_int *)p;
	ptr[b >> 5] &= ~(1 << (b & 0x1f));
}

static inline void
__set_bit(u_int b, volatile void *p)
{
	volatile u_int *ptr = (volatile u_int *)p;
	ptr[b >> 5] |= (1 << (b & 0x1f));
}

static inline int
test_bit(u_int b, const volatile void *p)
{
	return !!(((volatile u_int *)p)[b >> 5] & (1 << (b & 0x1f)));
}

static inline int
__test_and_set_bit(u_int b, volatile void *p)
{
	unsigned int m = 1 << (b & 0x1f);
	volatile u_int *ptr = (volatile u_int *)p;
	unsigned int prev = ptr[b >> 5];
	ptr[b >> 5] |= m;
	
	return (prev & m) != 0;
}

static inline int
test_and_clear_bit(u_int b, volatile void *p)
{
	unsigned int m = 1 << (b & 0x1f);
	unsigned int prev = __sync_fetch_and_and((volatile u_int *)p + (b >> 5), ~m);
	return (prev & m) != 0;
}

static inline int
__test_and_clear_bit(u_int b, volatile void *p)
{
	volatile u_int *ptr = (volatile u_int *)p;
	int rv = !!(ptr[b >> 5] & (1 << (b & 0x1f)));
	ptr[b >> 5] &= ~(1 << (b & 0x1f));
	return rv;
}

static inline int
find_first_zero_bit(volatile void *p, int max)
{
	int b;
	volatile u_int *ptr = (volatile u_int *)p;

	for (b = 0; b < max; b += 32) {
		if (ptr[b >> 5] != ~0) {
			for (;;) {
				if ((ptr[b >> 5] & (1 << (b & 0x1f))) == 0)
					return b;
				b++;
			}
		}
	}
	return max;
}

static inline int
find_next_zero_bit(volatile void *p, int max, int b)
{
	volatile u_int *ptr = (volatile u_int *)p;

	for (; b < max; b += 32) {
		if (ptr[b >> 5] != ~0) {
			for (;;) {
				if ((ptr[b >> 5] & (1 << (b & 0x1f))) == 0)
					return b;
				b++;
			}
		}
	}
	return max;
}

static inline int
find_first_bit(volatile void *p, int max)
{
	int b;
	volatile u_int *ptr = (volatile u_int *)p;

	for (b = 0; b < max; b += 32) {
		if (ptr[b >> 5] != 0) {
			for (;;) {
				if (ptr[b >> 5] & (1 << (b & 0x1f)))
					return b;
				b++;
			}
		}
	}
	return max;
}

static inline int
find_next_bit(const volatile void *p, int max, int b)
{
	volatile u_int *ptr = (volatile u_int *)p;

	for (; b < max; b+= 32) {
		if (ptr[b >> 5] != 0) {
			for (;;) {
				if (ptr[b >> 5] & (1 << (b & 0x1f)))
					return b;
				b++;
			}
		}
	}
	return max;
}

#define for_each_set_bit(b, p, max) \
	for ((b) = find_first_bit((p), (max));			\
	     (b) < (max);					\
	     (b) = find_next_bit((p), (max), (b) + 1))

#define for_each_clear_bit(b, p, max) \
	for ((b) = find_first_zero_bit((p), (max));		\
	     (b) < (max);					\
	     (b) = find_next_zero_bit((p), (max), (b) + 1))

#if defined(__i386__)
#define rmb()	__asm volatile("lock; addl $0,-4(%%esp)" : : : "memory", "cc")
#define wmb()	__asm volatile("lock; addl $0,-4(%%esp)" : : : "memory", "cc")
#define mb()	__asm volatile("lock; addl $0,-4(%%esp)" : : : "memory", "cc")
#define smp_mb()	__asm volatile("lock; addl $0,-4(%%esp)" : : : "memory", "cc")
#define smp_rmb()	__membar("")
#define smp_wmb()	__membar("")
#define smp_store_mb(var, value)	do { (void)xchg(&var, value); } while (0)
#define smp_mb__after_atomic()	do { } while (0)
#define smp_mb__before_atomic()	do { } while (0)
#elif defined(__amd64__)
#define rmb()	__membar("lfence")
#define wmb()	__membar("sfence")
#define mb()	__membar("mfence")
#define smp_mb()	__asm volatile("lock; addl $0,-4(%%rsp)" : : : "memory", "cc")
#define smp_rmb()	__membar("")
#define smp_wmb()	__membar("")
#define smp_store_mb(var, value)	do { (void)xchg(&var, value); } while (0)
#define smp_mb__after_atomic()	do { } while (0)
#define smp_mb__before_atomic()	do { } while (0)
#elif defined(__aarch64__)
#define rmb()	__membar("dsb ld")
#define wmb()	__membar("dsb st")
#define mb()	__membar("dsb sy")
#define dma_rmb() __membar("dmb oshld")
#define dma_wmb() __membar("dmb oshst")
#define dma_mb() __membar("dmb osh")
#define smp_rmb() __membar("dmb ishld")
#define smp_wmb() __membar("dmb ishst")
#define smp_mb() __membar("dmb ish")
#elif defined(__arm__)
#define rmb()	__membar("dsb sy")
#define wmb()	__membar("dsb sy")
#define mb()	__membar("dsb sy")
#elif defined(__mips64__)
#define rmb()	mips_sync() 
#define wmb()	mips_sync()
#define mb()	mips_sync()
#elif defined(__powerpc64__)
#define rmb()	__membar("sync")
#define wmb()	__membar("sync")
#define mb()	__membar("sync")
#define smp_rmb()	__membar("lwsync")
#define smp_wmb()	__membar("lwsync")
#define smp_mb()	__membar("sync")
#elif defined(__powerpc__)
#define rmb()	__membar("sync")
#define wmb()	__membar("sync")
#define mb()	__membar("sync")
#define smp_wmb()	__membar("eieio")
#elif defined(__riscv)
#define rmb()	__membar("fence ir,ir")
#define wmb()	__membar("fence ow,ow")
#define mb()	__membar("fence iorw,iorw")
#define smp_rmb()	__membar("fence r,r")
#define smp_wmb()	__membar("fence w,w")
#define smp_mb()	__membar("fence rw,rw")
#elif defined(__sparc64__)
#define rmb()	membar_sync()
#define wmb()	membar_sync()
#define mb()	membar_sync()
#endif

#ifndef smp_rmb
#define smp_rmb()	rmb()
#endif

#ifndef smp_wmb
#define smp_wmb()	wmb()
#endif

#ifndef smp_mb
#define smp_mb()	mb()
#endif

#ifndef mmiowb
#define mmiowb()	wmb()
#endif

#ifndef smp_mb__before_atomic
#define smp_mb__before_atomic()	smp_mb()
#endif

#ifndef smp_mb__after_atomic
#define smp_mb__after_atomic()	smp_mb()
#endif

#ifndef smp_store_mb
#define smp_store_mb(x, v)	do { WRITE_ONCE(x, v); smp_mb(); } while (0)
#endif

#ifndef smp_store_release
#define smp_store_release(x, v)	do { smp_mb(); WRITE_ONCE(*x, v); } while(0)
#endif

#ifndef smp_load_acquire
#define smp_load_acquire(x)			\
({						\
	__typeof(*x) _v = READ_ONCE(*x);	\
	smp_mb();				\
	_v;					\
})
#endif

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#define ATOMIC_LONG_INIT(i)	ATOMIC64_INIT(i)
#define __KPI_ATOMIC(pfx, T, t) \
static inline T pfx##_read(const t *v) { return __atomic_load_n(&v->counter, __ATOMIC_RELAXED); } \
static inline void pfx##_set(t *v, T i) { __atomic_store_n(&v->counter, i, __ATOMIC_RELAXED); } \
static inline void pfx##_set_release(t *v, T i) { __atomic_store_n(&v->counter, i, __ATOMIC_RELEASE); } \
static inline T pfx##_read_acquire(const t *v) { return __atomic_load_n(&v->counter, __ATOMIC_ACQUIRE); } \
static inline void pfx##_add(T i, t *v) { __atomic_fetch_add(&v->counter, i, __ATOMIC_SEQ_CST); } \
static inline void pfx##_sub(T i, t *v) { __atomic_fetch_sub(&v->counter, i, __ATOMIC_SEQ_CST); } \
static inline void pfx##_inc(t *v) { pfx##_add(1, v); } \
static inline void pfx##_dec(t *v) { pfx##_sub(1, v); } \
static inline T pfx##_add_return(T i, t *v) { return __atomic_add_fetch(&v->counter, i, __ATOMIC_SEQ_CST); } \
static inline T pfx##_sub_return(T i, t *v) { return __atomic_sub_fetch(&v->counter, i, __ATOMIC_SEQ_CST); } \
static inline T pfx##_inc_return(t *v) { return pfx##_add_return(1, v); } \
static inline T pfx##_dec_return(t *v) { return pfx##_sub_return(1, v); } \
static inline T pfx##_fetch_add(T i, t *v) { return __atomic_fetch_add(&v->counter, i, __ATOMIC_SEQ_CST); } \
static inline T pfx##_fetch_sub(T i, t *v) { return __atomic_fetch_sub(&v->counter, i, __ATOMIC_SEQ_CST); } \
static inline T pfx##_fetch_inc(t *v) { return pfx##_fetch_add(1, v); } \
static inline T pfx##_fetch_dec(t *v) { return pfx##_fetch_sub(1, v); } \
static inline T pfx##_fetch_or(T i, t *v) { return __atomic_fetch_or(&v->counter, i, __ATOMIC_SEQ_CST); } \
static inline T pfx##_fetch_and(T i, t *v) { return __atomic_fetch_and(&v->counter, i, __ATOMIC_SEQ_CST); } \
static inline T pfx##_fetch_andnot(T i, t *v) { return __atomic_fetch_and(&v->counter, ~i, __ATOMIC_SEQ_CST); } \
static inline T pfx##_fetch_xor(T i, t *v) { return __atomic_fetch_xor(&v->counter, i, __ATOMIC_SEQ_CST); } \
static inline void pfx##_or(T i, t *v) { __atomic_fetch_or(&v->counter, i, __ATOMIC_SEQ_CST); } \
static inline void pfx##_and(T i, t *v) { __atomic_fetch_and(&v->counter, i, __ATOMIC_SEQ_CST); } \
static inline void pfx##_andnot(T i, t *v) { __atomic_fetch_and(&v->counter, ~i, __ATOMIC_SEQ_CST); } \
static inline void pfx##_xor(T i, t *v) { __atomic_fetch_xor(&v->counter, i, __ATOMIC_SEQ_CST); } \
static inline bool pfx##_dec_and_test(t *v) { return pfx##_dec_return(v) == 0; } \
static inline bool pfx##_inc_and_test(t *v) { return pfx##_inc_return(v) == 0; } \
static inline bool pfx##_sub_and_test(T i, t *v) { return pfx##_sub_return(i, v) == 0; } \
static inline T pfx##_xchg(t *v, T n) { return __atomic_exchange_n(&v->counter, n, __ATOMIC_SEQ_CST); } \
static inline T pfx##_cmpxchg(t *v, T o, T n) { __atomic_compare_exchange_n(&v->counter, &o, n, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST); return o; } \
static inline bool pfx##_try_cmpxchg(t *v, T *o, T n) { return __atomic_compare_exchange_n(&v->counter, o, n, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST); } \
static inline T pfx##_fetch_add_unless(t *v, T a, T u) { T c = pfx##_read(v); do { if (c == u) break; } while (!pfx##_try_cmpxchg(v, &c, c + a)); return c; } \
static inline bool pfx##_add_unless(t *v, T a, T u) { return pfx##_fetch_add_unless(v, a, u) != u; } \
static inline bool pfx##_inc_not_zero(t *v) { return pfx##_add_unless(v, 1, 0); } \
static inline bool pfx##_dec_if_positive(t *v) { T c = pfx##_read(v); do { if (c <= 0) return 0; } while (!pfx##_try_cmpxchg(v, &c, c - 1)); return 1; } \
static inline bool pfx##_inc_unless_negative(t *v) { T c = pfx##_read(v); do { if (c < 0) return 0; } while (!pfx##_try_cmpxchg(v, &c, c + 1)); return 1; } \
static inline bool pfx##_add_negative(T i, t *v) { return pfx##_add_return(i, v) < 0; }

#endif
