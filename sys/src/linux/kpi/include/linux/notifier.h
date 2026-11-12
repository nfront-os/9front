/* Public domain. */

#ifndef _LINUX_NOTIFIER_H
#define _LINUX_NOTIFIER_H

#include <sys/queue.h>

struct notifier_block {
	int (*notifier_call)(struct notifier_block *, unsigned long, void *);
	SLIST_ENTRY(notifier_block) link;
};

struct blocking_notifier_head {
};

#define ATOMIC_INIT_NOTIFIER_HEAD(x)

#define NOTIFY_DONE	0
#define NOTIFY_OK	1
#define NOTIFY_BAD	2

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/rwsem.h>
#include <linux/spinlock.h>
struct atomic_notifier_head { spinlock_t lock; struct notifier_block *head; };
#define ATOMIC_NOTIFIER_HEAD(n)	struct atomic_notifier_head n = { .head = NULL }
int	atomic_notifier_chain_register(struct atomic_notifier_head *h, struct notifier_block *n);
int	atomic_notifier_chain_unregister(struct atomic_notifier_head *h, struct notifier_block *n);
int	atomic_notifier_call_chain(struct atomic_notifier_head *h, unsigned long v, void *d);
#define NOTIFY_STOP		0x8001
#define BLOCKING_NOTIFIER_HEAD(n)	struct blocking_notifier_head n = { .head = NULL }
#define BLOCKING_INIT_NOTIFIER_HEAD(n)	((n)->head = NULL)
#define blocking_notifier_chain_register(h, n)	0
#define blocking_notifier_chain_unregister(h, n)	0
#define blocking_notifier_call_chain(h, v, d)	NOTIFY_DONE

#define register_acpi_notifier(n)	0
#define unregister_acpi_notifier(n)	0

#endif
