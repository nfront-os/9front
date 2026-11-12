/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/kobject.h (ours, not Linux's) */
#ifndef _KPI_LINUX_KOBJECT_H
#define _KPI_LINUX_KOBJECT_H

#include <linux/idr.h>	/* as Linux's kobject.h brings it (sysfs.h, kernfs.h) */
#include <linux/kernel.h>
#include <linux/kref.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/sysfs.h>
#include <linux/workqueue.h>

struct kobj_type;
struct kset;
struct kernfs_node;
struct kobject { const char *name; struct kobject *parent; struct kref kref; const struct kobj_type *ktype; void *kpi;
	struct list_head entry; struct kset *kset; struct kernfs_node *sd; };	/* sd: its sysfs node, none here */	/* entry: in its kset's list */
struct kobj_type { void (*release)(struct kobject *k); const void *sysfs_ops; const struct attribute_group **default_groups; };
struct kobj_attribute { struct attribute attr; ssize_t (*show)(struct kobject *k, struct kobj_attribute *a, char *buf);
	ssize_t (*store)(struct kobject *k, struct kobj_attribute *a, const char *buf, size_t n); };
enum kobject_action { KOBJ_ADD, KOBJ_REMOVE, KOBJ_CHANGE, KOBJ_MOVE, KOBJ_ONLINE, KOBJ_OFFLINE, KOBJ_BIND, KOBJ_UNBIND };
struct kobj_uevent_env { char *argv[3]; char *envp[64]; int envp_idx; char buf[2048]; int buflen; };
#define add_uevent_var(e, fmt, ...)	0
extern const struct sysfs_ops kobj_sysfs_ops;
struct sysfs_ops { ssize_t (*show)(struct kobject *k, struct attribute *a, char *buf); ssize_t (*store)(struct kobject *k, struct attribute *a, const char *buf, size_t n); };
#define kobject_uevent(k, a)			0
#define kobject_uevent_env(k, a, e)		0
/* kobjects: named, counted, released by their type (kpi/linux/device.c); no sysfs serves them yet */
struct kobject	*kobject_get(struct kobject *k);
void	kobject_put(struct kobject *k);
void	kobject_init(struct kobject *k, const struct kobj_type *t);
int	kobject_add(struct kobject *k, struct kobject *parent, const char *fmt, ...) __printf(3, 4);
int	kobject_init_and_add(struct kobject *k, const struct kobj_type *t, struct kobject *parent, const char *fmt, ...) __printf(4, 5);
#define kobject_del(k)				do { } while (0)
int	kobject_set_name(struct kobject *k, const char *fmt, ...) __printf(2, 3);
/*
 * a set of kobjects: itself a kobject, the parent of its members that
 * have none; they are in its list from kobject_add to their release
 */
struct kset_uevent_ops;
struct kset {
	struct list_head	list;
	spinlock_t	list_lock;
	struct kobject	kobj;
	const struct kset_uevent_ops	*uevent_ops;
};
#define to_kset(k)	container_of(k, struct kset, kobj)
int	kset_register(struct kset *k);
void	kset_unregister(struct kset *k);
/* a directory of sysfs: named, under its parent; nothing serves it yet (kpi/linux/device.c) */
struct kobject	*kobject_create_and_add(const char *name, struct kobject *parent);
#define kobject_name(k)				((k)->name)
#define kernel_kobj				((struct kobject *)NULL)

#endif
