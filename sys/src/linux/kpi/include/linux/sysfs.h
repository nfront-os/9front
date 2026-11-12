/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/sysfs.h (ours, not Linux's) */
#ifndef _KPI_LINUX_SYSFS_H
#define _KPI_LINUX_SYSFS_H

#include <linux/kernel.h>
#include <asm/page.h>

/* no sysfs: the file server's files instead */
struct attribute { const char *name; umode_t mode; };
struct kobject;
struct file;
struct vm_area_struct;
struct bin_attribute;
struct attribute_group { const char *name; umode_t (*is_visible)(struct kobject *k, struct attribute *a, int n);
	umode_t (*is_bin_visible)(struct kobject *k, const struct bin_attribute *a, int n);
	size_t (*bin_size)(struct kobject *k, const struct bin_attribute *a, int n);
	union { struct attribute **attrs; const struct attribute *const *attrs_const; };
	const struct bin_attribute *const *bin_attrs; };	/* 9front: Linux 6.16's const bin attributes */
struct bin_attribute { struct attribute attr; size_t size; void *private;
	int (*mmap)(struct file *f, struct kobject *k, const struct bin_attribute *a, struct vm_area_struct *v);
	ssize_t (*read)(struct file *f, struct kobject *k, const struct bin_attribute *a, char *buf, loff_t off, size_t n);
	ssize_t (*write)(struct file *f, struct kobject *k, const struct bin_attribute *a, char *buf, loff_t off, size_t n); };	/* 9front: const (Linux 6.16) */
#define __ATTR(n, m, s, st)	{ .attr = { .name = __stringify(n), .mode = (m) }, .show = (s), .store = (st) }
#define __ATTR_RO(n)		{ .attr = { .name = __stringify(n), .mode = 0444 }, .show = n##_show }
#define __ATTR_WO(n)		{ .attr = { .name = __stringify(n), .mode = 0200 }, .store = n##_store }
#define __ATTR_RW(n)		{ .attr = { .name = __stringify(n), .mode = 0644 }, .show = n##_show, .store = n##_store }
#define __ATTR_NULL		{ .attr = { .name = NULL } }
#define __BIN_ATTR(n, m, r, w, s)	{ .attr = { .name = __stringify(n), .mode = (m) }, .read = (r), .write = (w), .size = (s) }
#define BIN_ATTR(n, m, r, w, s)	struct bin_attribute bin_attr_##n = __BIN_ATTR(n, m, r, w, s)
#define ATTRIBUTE_GROUPS(n)	static const struct attribute_group n##_group = { .attrs = n##_attrs }; \
	static const struct attribute_group *n##_groups[] = { &n##_group, NULL }
#define sysfs_create_group(k, g)		0
#define sysfs_remove_group(k, g)		do { } while (0)
#define sysfs_create_groups(k, g)		0
#define sysfs_remove_groups(k, g)		do { } while (0)
#define sysfs_add_file_to_group(k, a, g)	0
#define sysfs_remove_file_from_group(k, a, g)	do { } while (0)
#define sysfs_create_files(k, a)		0
#define sysfs_remove_files(k, a)		do { } while (0)
#define sysfs_create_file(k, a)			0
#define sysfs_remove_file(k, a)			do { } while (0)
#define sysfs_create_bin_file(k, a)		0
#define sysfs_remove_bin_file(k, a)		do { } while (0)
#define sysfs_create_link(k, t, n)		0
#define sysfs_remove_link(k, n)			do { } while (0)
#define sysfs_merge_group(k, g)			0
#define sysfs_unmerge_group(k, g)		do { } while (0)
#define sysfs_update_group(k, g)		0
#define sysfs_notify(k, d, a)			do { } while (0)
#define sysfs_emit(buf, fmt, ...)		scnprintf(buf, PAGE_SIZE, fmt, ##__VA_ARGS__)
#define sysfs_emit_at(buf, at, fmt, ...)	scnprintf((buf) + (at), PAGE_SIZE - (at), fmt, ##__VA_ARGS__)
#define sysfs_attr_init(a)			do { } while (0)
#define sysfs_bin_attr_init(a)			do { } while (0)

#define __ATTR_RW_MODE(_name, _mode)	__ATTR(_name, _mode, _name##_show, _name##_store)	/* 9front: Linux 6.4 */

#endif
