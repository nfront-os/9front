/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/of.h (ours, not Linux's) */
#ifndef _KPI_LINUX_OF_H
#define _KPI_LINUX_OF_H

#include <linux/fwnode.h>	/* 9front: as Linux's of.h */
#include <linux/device.h>



/* 9front: __free(device_node) (linux/cleanup.h) */
#include <linux/cleanup.h>
DEFINE_FREE(device_node, struct device_node *, if (_T) of_node_put(_T))

#endif
