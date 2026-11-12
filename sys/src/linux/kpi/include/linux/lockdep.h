/* Public domain. */

#ifndef _LINUX_LOCKDEP_H
#define _LINUX_LOCKDEP_H

#include <linux/smp.h>

struct lock_class_key {
};

struct pin_cookie {
};

#define might_lock(lock)
#define might_lock_nested(lock, subc)
#define lockdep_assert(c)		do {} while(0)
#define lockdep_assert_held(lock)	do { (void)(lock); } while(0)
#define lockdep_assert_held_once(lock)	do { (void)(lock); } while(0)
#define lockdep_assert_held_write(lock)	do { (void)(lock); } while(0)
#define lockdep_assert_once(lock)	do { (void)(lock); } while(0)
#define lockdep_assert_not_held(lock)	do { (void)(lock); } while(0)
#define lockdep_assert_none_held_once()	do {} while(0)
#define lock_acquire(lock, a, b, c, d, e, f)
#define lock_release(lock, a)
#define lock_acquire_shared_recursive(lock, a, b, c, d)
#define lockdep_set_subclass(a, b)
#define lockdep_unpin_lock(a, b)
#define lockdep_set_class(a, b)
#define lockdep_init_map(a, b, c, d)
#define lockdep_set_class_and_name(a, b, c)
#define lockdep_is_held(lock)		0

#define lock_map_acquire(a)
#define lock_map_release(b)

#define mutex_acquire(a, b, c, d)
#define mutex_release(a, b)

#define SINGLE_DEPTH_NESTING		0

#define lockdep_pin_lock(lock)		\
({					\
	struct pin_cookie pc = {};	\
	pc;				\
})

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
struct lockdep_map { int unused; };
#define lockdep_assert_held_write(l)	do { (void)(l); } while (0)
#define lockdep_assert_held_read(l)	do { (void)(l); } while (0)
#define lockdep_assert_irqs_disabled()	do { } while (0)
#define lockdep_assert_irqs_enabled()	do { } while (0)
#define lockdep_assert_in_irq()		do { } while (0)
#define lock_map_acquire(m)		do { } while (0)
#define lock_map_release(m)		do { } while (0)
#define lock_map_acquire_read(m)	do { } while (0)
#define might_lock_read(l)		do { } while (0)
#define lockdep_repin_lock(l, c)	do { } while (0)
#define lockdep_off()			do { } while (0)
#define lockdep_on()			do { } while (0)
#define STATIC_LOCKDEP_MAP_INIT(n, k)	{ 0 }
#define rwsem_acquire(m, s, t, i)	do { } while (0)
#define rwsem_release(m, i)		do { } while (0)
#define lock_is_held(m)			1

#endif
