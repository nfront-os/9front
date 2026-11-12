/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/fwnode.h (ours, not Linux's) */
#ifndef _KPI_LINUX_FWNODE_H
#define _KPI_LINUX_FWNODE_H

#include <linux/err.h>
#include <linux/device.h>



#define fwnode_device_is_available(fw)	false	/* 9front: no firmware nodes */
#define fwnode_find_reference(fw, name, i)	((struct fwnode_handle *)ERR_PTR(-ENOENT))

static inline void fwnode_handle_put(struct fwnode_handle *h) { }

#endif
