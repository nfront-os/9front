/*	$OpenBSD: kref.h,v 1.6 2023/03/21 09:44:35 jsg Exp $	*/
/*
 * Copyright (c) 2015 Mark Kettenis
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _LINUX_KREF_H
#define _LINUX_KREF_H

#include <sys/types.h>
#include <sys/rwlock.h>
#include <linux/mutex.h>
#include <linux/refcount.h>
#include <linux/spinlock.h>

/* 9front: Linux's struct kref, around a refcount_t; kref_get_unless_zero atomic */
struct kref {
	refcount_t refcount;
};

#define KREF_INIT(n)	{ .refcount = REFCOUNT_INIT(n) }

static inline void
kref_init(struct kref *ref)
{
	refcount_set(&ref->refcount, 1);
}

static inline unsigned int
kref_read(const struct kref *ref)
{
	return refcount_read(&ref->refcount);
}

static inline void
kref_get(struct kref *ref)
{
	refcount_inc(&ref->refcount);
}

static inline int
kref_get_unless_zero(struct kref *ref)
{
	return refcount_inc_not_zero(&ref->refcount);
}

static inline int
kref_put(struct kref *ref, void (*release)(struct kref *ref))
{
	if (refcount_dec_and_test(&ref->refcount)) {
		release(ref);
		return 1;
	}
	return 0;
}

static inline int
kref_put_mutex(struct kref *kref, void (*release)(struct kref *kref),
    struct mutex *lock)
{
	if (!refcount_dec_not_one(&kref->refcount)) {
		mutex_lock(lock);
		if (likely(refcount_dec_and_test(&kref->refcount))) {
			release(kref);
			return 1;
		}
		mutex_unlock(lock);
		return 0;
	}
	return 0;
}

static inline int
kref_put_lock(struct kref *kref, void (*release)(struct kref *kref),
    struct spinlock *lock)
{
	if (!refcount_dec_not_one(&kref->refcount)) {
		mtx_enter(lock);
		if (likely(refcount_dec_and_test(&kref->refcount))) {
			release(kref);
			return 1;
		}
		mtx_leave(lock);
		return 0;
	}
	return 0;
}

#endif
