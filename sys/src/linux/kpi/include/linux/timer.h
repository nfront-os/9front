/*	$OpenBSD: timer.h,v 1.11 2026/08/18 00:53:37 jsg Exp $	*/
/*
 * Copyright (c) 2013, 2014, 2015 Mark Kettenis
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

#ifndef _LINUX_TIMER_H
#define _LINUX_TIMER_H

#include <sys/types.h>
#include <sys/param.h>
#include <sys/timeout.h>
#include <sys/kernel.h>
#include <linux/ktime.h>
#include <linux/jiffies.h>
#include <linux/container_of.h>

/*
 * 9front: Linux's struct timer_list around a timeout(9) (OpenBSD changes
 * the Linux code to struct timeout); the timeout calls the timer's
 * function with the timer, as Linux does (timer_setup, from_timer).
 */
struct timer_list {
	struct timeout	to;
	void		(*function)(struct timer_list *);
	unsigned long	expires;
	unsigned int	flags;
};

#define TIMER_IRQSAFE		0x00200000
#define TIMER_DEFERRABLE	0x00080000

void	kpi_timer_fire(void *);	/* kpi/obsd/timeout.c: runs the timer's function */
int	kpi_timeout_status(char *, int);	/* the timeout proc's state */

static inline void
timer_setup(struct timer_list *t, void (*fn)(struct timer_list *), unsigned int flags)
{
	t->function = fn;
	t->flags = flags;
	t->expires = 0;
	timeout_set(&t->to, kpi_timer_fire, t);
}
#define timer_setup_on_stack(t, fn, f)	timer_setup(t, fn, f)
#define destroy_timer_on_stack(t)	((void)(t))
#define from_timer(var, t, field)	container_of(t, __typeof(*var), field)

#define del_timer_sync(x)	timeout_del_barrier(&(x)->to)
#define timer_delete_sync(x)	timeout_del_barrier(&(x)->to)
#define timer_shutdown_sync(x)	timeout_del_barrier(&(x)->to)
#define del_timer(x)		timeout_del(&(x)->to)
#define timer_delete(x)		timeout_del(&(x)->to)
#define timer_pending(x)	timeout_pending(&(x)->to)
/* 9front: from OpenBSD 6.18 */
#include <linux/debugobjects.h>

static inline int
mod_timer(struct timer_list *t, unsigned long j)
{
	t->expires = j;
	/* Linux's result: whether it was pending (timeout_add says whether it was not) */
	if ((long)(j - jiffies) <= 0)
		return !timeout_add(&t->to, 1);
	return !timeout_add(&t->to, j - jiffies);
}

static inline void
add_timer(struct timer_list *t)
{
	mod_timer(t, t->expires);
}

static inline unsigned long
round_jiffies_up(unsigned long j)
{
	return roundup(j, hz);
}

static inline unsigned long
round_jiffies_up_relative(unsigned long j)
{
	return roundup(j, hz);
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/jiffies.h>
#define DEFINE_TIMER(n, f)		struct timer_list n = { .function = f }

/* 9front: Linux 6.16's name of from_timer */
#define timer_container_of(var, t, field)	from_timer(var, t, field)

/* 9front: Linux 6.15's name of destroy_timer_on_stack */
#define timer_destroy_on_stack(t)	destroy_timer_on_stack(t)

#endif
