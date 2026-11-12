/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/auxiliary_bus.h (ours, not Linux's) */
#ifndef _KPI_LINUX_AUXILIARY_BUS_H
#define _KPI_LINUX_AUXILIARY_BUS_H

#include <linux/device.h>

struct auxiliary_device { struct device dev; const char *name; u32 id; };
struct auxiliary_device_id { char name[32]; unsigned long driver_data; };
struct auxiliary_driver { int (*probe)(struct auxiliary_device *a, const struct auxiliary_device_id *id); void (*remove)(struct auxiliary_device *a);
	const char *name; struct device_driver driver; const struct auxiliary_device_id *id_table; };
#define auxiliary_device_init(a)		(-ENODEV)
#define __auxiliary_device_add(a, n)		(-ENODEV)
#define auxiliary_device_add(a)			(-ENODEV)
#define auxiliary_device_uninit(a)		do { } while (0)
#define auxiliary_device_delete(a)		do { } while (0)
#define to_auxiliary_dev(d)			container_of(d, struct auxiliary_device, dev)

#endif
