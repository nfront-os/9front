/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/pinctrl/consumer.h (ours, not Linux's) */
#ifndef _KPI_LINUX_PINCTRL_CONSUMER_H
#define _KPI_LINUX_PINCTRL_CONSUMER_H

#include <linux/kernel.h>
#include <linux/err.h>

struct pinctrl;
#define pinctrl_get_select(d, s)	((struct pinctrl *)ERR_PTR(-ENODEV))
#define devm_pinctrl_get_select(d, s)	((struct pinctrl *)ERR_PTR(-ENODEV))
#define pinctrl_put(p)	do { } while (0)

#endif
