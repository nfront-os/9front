/* Public domain. */

#ifndef _LINUX_RWLOCK_TYPES_H
#define _LINUX_RWLOCK_TYPES_H

#include <sys/mutex.h>

/* 9front: Linux's rwlock_t, a spinning lock as spinlock_t (OpenBSD changes the Linux code to spinlock_t) */
typedef struct spinlock rwlock_t;
#define rwlock_init(l)		mtx_init((l), IPL_NONE)
#define DEFINE_RWLOCK(x)	rwlock_t x = MUTEX_INITIALIZER(IPL_NONE)

#endif
