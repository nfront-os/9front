/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/mod_devicetable.h (ours, not Linux's) */
#ifndef _KPI_LINUX_MOD_DEVICETABLE_H
#define _KPI_LINUX_MOD_DEVICETABLE_H

#include <linux/kernel.h>

struct pci_device_id { u32 vendor, device, subvendor, subdevice, class, class_mask; unsigned long driver_data; u32 override_only; };
struct acpi_device_id { u8 id[16]; unsigned long driver_data; u32 cls, cls_msk; };
struct of_device_id { char name[32]; char type[32]; char compatible[128]; const void *data; };
struct dmi_system_id;
#define PCI_ANY_ID		(~0)

typedef unsigned long kernel_ulong_t;	/* 9front: the tables' driver_data type */

#endif
