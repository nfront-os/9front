/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/mei_aux.h (ours, not Linux's) */
#ifndef _KPI_LINUX_MEI_AUX_H
#define _KPI_LINUX_MEI_AUX_H

#include <linux/auxiliary_bus.h>
#include <linux/ioport.h>

/* no Intel ME/GSC auxiliary devices: no HDCP or PXP through it */
struct mei_aux_device { struct auxiliary_device aux_dev; int irq; struct resource bar; struct resource ext_op_mem; bool slow_firmware; };
#define auxiliary_dev_to_mei_aux_dev(a)	container_of(a, struct mei_aux_device, aux_dev)

#endif
