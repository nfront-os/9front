/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/gpio/machine.h (ours, not Linux's) */
#ifndef _KPI_LINUX_GPIO_MACHINE_H
#define _KPI_LINUX_GPIO_MACHINE_H

#include <linux/kernel.h>
#include <linux/list.h>

enum gpio_lookup_flags { GPIO_ACTIVE_HIGH = 0, GPIO_ACTIVE_LOW = 1 };
struct gpiod_lookup { const char *key; u16 chip_hwnum; const char *con_id; unsigned int idx; unsigned long flags; };
struct gpiod_lookup_table { struct list_head list; const char *dev_id; struct gpiod_lookup table[]; };
#define GPIO_LOOKUP_IDX(k, h, c, i, f)	((struct gpiod_lookup){ .key = k, .chip_hwnum = h, .con_id = c, .idx = i, .flags = f })	/* 9front: a compound literal, as Linux 6.10's */
#define GPIO_LOOKUP(k, h, c, f)		GPIO_LOOKUP_IDX(k, h, c, 0, f)
#define gpiod_add_lookup_table(t)	do { } while (0)
#define gpiod_remove_lookup_table(t)	do { } while (0)

#endif
