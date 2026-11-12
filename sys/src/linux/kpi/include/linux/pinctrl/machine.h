/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/pinctrl/machine.h (ours, not Linux's) */
#ifndef _KPI_LINUX_PINCTRL_MACHINE_H
#define _KPI_LINUX_PINCTRL_MACHINE_H

#include <linux/kernel.h>

struct pinctrl_map { const char *dev_name; const char *name; int type; const char *ctrl_dev_name; };
#define PIN_MAP_MUX_GROUP(d, s, p, g, f)	{ 0 }
#define pinctrl_register_mappings(m, n)	0
#define pinctrl_unregister_mappings(m)	do { } while (0)

#endif
