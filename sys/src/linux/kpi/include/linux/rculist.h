/* Public domain. */

#ifndef _LINUX_RCULIST_H
#define _LINUX_RCULIST_H

#include <linux/list.h>
#include <linux/rcupdate.h>

#define list_add_rcu(a, b)		list_add(a, b)
#define list_add_tail_rcu(a, b)		list_add_tail(a, b)
#define list_del_rcu(a)			list_del(a)
#define list_for_each_entry_rcu		list_for_each_entry
#define list_for_each_entry_lockless(a, b, c)	list_for_each_entry(a, b, c)

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#define list_replace_rcu(o, n)		list_replace(o, n)
#define list_for_each_entry_continue_rcu(p, h, m)	list_for_each_entry_continue(p, h, m)
#define list_first_or_null_rcu(p, t, m)	list_first_entry_or_null(p, t, m)
#define list_next_or_null_rcu(h, p, t, m)	({ struct list_head *__n = READ_ONCE((p)->next); __n != (h) ? list_entry(__n, t, m) : NULL; })
#define list_entry_rcu(p, t, m)		list_entry(p, t, m)
#define hlist_add_head_rcu(n, h)	hlist_add_head(n, h)
#define hlist_del_rcu(n)		__hlist_del(n)
#define hlist_del_init_rcu(n)		hlist_del_init(n)
#define hlist_for_each_entry_rcu(p, h, m, ...)	hlist_for_each_entry(p, h, m)

#endif
