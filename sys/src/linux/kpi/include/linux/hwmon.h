/* Public domain. */
/*
 * 9front: hardware monitors (temperatures, fans, power) as a device of
 * their own with the driver's attribute groups (kpi/linux/device.c).
 * OpenBSD's header is empty: OpenBSD changes the Linux code.
 */

#ifndef _LINUX_HWMON_H
#define _LINUX_HWMON_H

#include <linux/device.h>

struct device	*hwmon_device_register_with_groups(struct device *parent, const char *name,
		    void *drvdata, const struct attribute_group **groups);
void		hwmon_device_unregister(struct device *d);

#endif
