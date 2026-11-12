/* Public domain. */

#ifndef _LINUX_SPINLOCK_TYPES_H
#define _LINUX_SPINLOCK_TYPES_H

#include <sys/types.h>
#include <sys/mutex.h>
#include <linux/rwlock_types.h>

typedef struct spinlock spinlock_t;
typedef spinlock_t raw_spinlock_t;	/* 9front: before the includes below (ratelimit.h) */
/* 9front: struct raw_spinlock (drm_mode_config's panic_lock) is ours, as raw_spinlock_t */
#define raw_spinlock	spinlock
#define spin_lock_init(l)	mtx_init((l), IPL_NONE)
#define DEFINE_SPINLOCK(x)	struct spinlock x = MUTEX_INITIALIZER(IPL_TTY)


/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/types.h>
#include <linux/lockdep.h>
#define __SPIN_LOCK_UNLOCKED(n)		MUTEX_INITIALIZER(IPL_NONE)
#define SPIN_LOCK_UNLOCKED		__SPIN_LOCK_UNLOCKED(x)
#define DEFINE_RAW_SPINLOCK(x)		DEFINE_SPINLOCK(x)
#define __RW_LOCK_UNLOCKED(n)		MUTEX_INITIALIZER(IPL_NONE)

#endif
