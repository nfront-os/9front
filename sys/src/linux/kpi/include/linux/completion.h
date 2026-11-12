/*	$OpenBSD: completion.h,v 1.11 2026/08/19 01:34:10 jsg Exp $	*/
/*
 * Copyright (c) 2015, 2018 Mark Kettenis
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

#ifndef _LINUX_COMPLETION_H
#define _LINUX_COMPLETION_H

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mutex.h>
#include <linux/wait.h>

struct completion {
	u_int done;
	struct spinlock lock;
};

#define DECLARE_COMPLETION_ONSTACK(name) \
	struct completion name = { 0, MUTEX_INITIALIZER(IPL_TTY) }

static inline void
init_completion(struct completion *x)
{
	x->done = 0;
	mtx_init(&x->lock, IPL_TTY);
}

static inline void
reinit_completion(struct completion *x)
{
	x->done = 0;
}

static inline u_long
wait_for_completion_timeout(struct completion *x, u_long timo)
{
	int ret;

	KASSERT(!cold);

	mtx_enter(&x->lock);
	while (x->done == 0) {
		ret = msleep_ticks(x, &x->lock, 0, "wfct", timo);	/* 9front: OpenBSD's msleep */
		if (ret) {
			mtx_leave(&x->lock);
			/* timeout */
			return 0;
		}
	}
	if (x->done != UINT_MAX)
		x->done--;
	mtx_leave(&x->lock);

	return 1;
}

static inline void
wait_for_completion(struct completion *x)
{
	KASSERT(!cold);

	mtx_enter(&x->lock);
	while (x->done == 0) {
		msleep_nsec(x, &x->lock, 0, "wfcom", INFSLP);
	}
	if (x->done != UINT_MAX)
		x->done--;
	mtx_leave(&x->lock);
}

static inline u_long
wait_for_completion_interruptible(struct completion *x)
{
	int ret;

	KASSERT(!cold);

	mtx_enter(&x->lock);
	while (x->done == 0) {
		ret = msleep_nsec(x, &x->lock, PCATCH, "wfci", INFSLP);
		if (ret) {
			mtx_leave(&x->lock);
			if (ret == EWOULDBLOCK)
				return 0;
			return -ERESTARTSYS;
		}
	}
	if (x->done != UINT_MAX)
		x->done--;
	mtx_leave(&x->lock);

	return 0;
}

static inline u_long
wait_for_completion_interruptible_timeout(struct completion *x, u_long timo)
{
	int ret;

	KASSERT(!cold);

	mtx_enter(&x->lock);
	while (x->done == 0) {
		ret = msleep_ticks(x, &x->lock, PCATCH, "wfcit", timo);
		if (ret) {
			mtx_leave(&x->lock);
			if (ret == EWOULDBLOCK)
				return 0;
			return -ERESTARTSYS;
		}
	}
	if (x->done != UINT_MAX)
		x->done--;
	mtx_leave(&x->lock);

	return 1;
}

static inline void
complete(struct completion *x)
{
	mtx_enter(&x->lock);
	if (x->done != UINT_MAX)
		x->done++;
	mtx_leave(&x->lock);
	wakeup_one(x);
}

static inline void
complete_all(struct completion *x)
{
	mtx_enter(&x->lock);
	x->done = UINT_MAX;
	mtx_leave(&x->lock);
	wakeup(x);
}

static inline bool
try_wait_for_completion(struct completion *x)
{
	mtx_enter(&x->lock);
	if (x->done == 0) {
		mtx_leave(&x->lock);
		return false;
	}
	if (x->done != UINT_MAX)
		x->done--;
	mtx_leave(&x->lock);
	return true;
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#define COMPLETION_INITIALIZER(w)	{ 0, MUTEX_INITIALIZER(IPL_TTY) }
#define DECLARE_COMPLETION(w)		struct completion w = COMPLETION_INITIALIZER(w)
static inline int wait_for_completion_killable(struct completion *x) { wait_for_completion(x); return 0; }
static inline bool completion_done(struct completion *x) { return READ_ONCE(x->done) != 0; }

#endif
