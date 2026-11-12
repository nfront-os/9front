/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/platform_device.h (ours, not Linux's) */
#ifndef _KPI_LINUX_PLATFORM_DEVICE_H
#define _KPI_LINUX_PLATFORM_DEVICE_H

#include <linux/device.h>
#include <linux/err.h>

struct platform_device { const char *name; int id; struct device dev; u32 num_resources; struct resource *resource; };
#define to_platform_device(d)		container_of(d, struct platform_device, dev)
#define platform_get_drvdata(p)		dev_get_drvdata(&(p)->dev)
#define platform_set_drvdata(p, d)	dev_set_drvdata(&(p)->dev, d)
#define platform_device_unregister(p)	do { } while (0)
#define platform_device_register_simple(n, i, r, nr)	((struct platform_device *)ERR_PTR(-ENODEV))
struct platform_device_info { struct device *parent; struct fwnode_handle *fwnode; const char *name; int id; const struct resource *res; unsigned int num_res; const void *data; size_t size_data; u64 dma_mask; };
#define platform_device_register_full(i)	((struct platform_device *)ERR_PTR(-ENODEV))
#define PLATFORM_DEVID_NONE		(-1)

#endif
