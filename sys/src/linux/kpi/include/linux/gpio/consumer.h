/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/gpio/consumer.h (ours, not Linux's) */
#ifndef _KPI_LINUX_GPIO_CONSUMER_H
#define _KPI_LINUX_GPIO_CONSUMER_H

#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/acpi.h>

struct gpio_desc;
enum gpiod_flags { GPIOD_ASIS = 0, GPIOD_IN = 1, GPIOD_OUT_LOW = 3, GPIOD_OUT_HIGH = 7 };
#define gpiod_get(d, n, f)	((struct gpio_desc *)ERR_PTR(-ENODEV))
#define gpiod_get_index(d, n, i, f)	((struct gpio_desc *)ERR_PTR(-ENODEV))
#define devm_gpiod_get_index(d, n, i, f)	((struct gpio_desc *)ERR_PTR(-ENODEV))
#define devm_gpiod_get(d, n, f)	((struct gpio_desc *)ERR_PTR(-ENODEV))
#define gpiod_put(g)	do { } while (0)
#define gpiod_set_value_cansleep(g, v)	do { } while (0)
#define gpiod_set_value(g, v)	do { } while (0)

#endif
