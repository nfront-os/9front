/* Public domain. */

#ifndef _LINUX_KERNFS_H
#define _LINUX_KERNFS_H

#include <linux/err.h>
#include <linux/idr.h>
#include <linux/mutex.h>

#define kernfs_find_and_get(kn, name)	((struct kernfs_node *)NULL)	/* 9front: no sysfs */
#define kernfs_put(kn)	do { } while (0)

#endif
