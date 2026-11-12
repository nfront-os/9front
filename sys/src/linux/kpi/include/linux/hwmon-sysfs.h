/* Public domain. */
/*
 * 9front: a hardware monitor's attributes with the sensor's index (the
 * driver's show and store tell its sensors apart by it).  OpenBSD's
 * header is empty: OpenBSD changes the Linux code that uses them.
 */

#ifndef _LINUX_HWMON_SYSFS_H
#define _LINUX_HWMON_SYSFS_H

#include <linux/device.h>

struct sensor_device_attribute {
	struct device_attribute	dev_attr;
	int	index;
};
#define to_sensor_dev_attr(a)	container_of(a, struct sensor_device_attribute, dev_attr)

#define SENSOR_ATTR(n, m, s, st, i)	{ .dev_attr = __ATTR(n, m, s, st), .index = (i) }
#define SENSOR_DEVICE_ATTR(n, m, s, st, i) \
	struct sensor_device_attribute sensor_dev_attr_##n = SENSOR_ATTR(n, m, s, st, i)

#endif
