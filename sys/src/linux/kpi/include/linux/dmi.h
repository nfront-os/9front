/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/dmi.h (ours, not Linux's) */
#ifndef _KPI_LINUX_DMI_H
#define _KPI_LINUX_DMI_H

#include <linux/kernel.h>

enum dmi_field { DMI_NONE, DMI_BIOS_VENDOR, DMI_BIOS_VERSION, DMI_BIOS_DATE, DMI_SYS_VENDOR, DMI_PRODUCT_NAME, DMI_PRODUCT_VERSION,
	DMI_PRODUCT_SERIAL, DMI_PRODUCT_UUID, DMI_PRODUCT_SKU, DMI_PRODUCT_FAMILY, DMI_BOARD_VENDOR, DMI_BOARD_NAME, DMI_BOARD_VERSION,
	DMI_BOARD_SERIAL, DMI_BOARD_ASSET_TAG, DMI_CHASSIS_VENDOR, DMI_CHASSIS_TYPE, DMI_STRING_MAX };
struct dmi_strmatch { unsigned char slot:7; unsigned char exact_match:1; char substr[79]; };
struct dmi_system_id { int (*callback)(const struct dmi_system_id *d); const char *ident; struct dmi_strmatch matches[4]; void *driver_data; };
#define DMI_MATCH(a, b)		{ .slot = a, .substr = b }
#define DMI_EXACT_MATCH(a, b)	{ .slot = a, .substr = b, .exact_match = 1 }
/* the machine's DMI strings (kpi/dmi.c, from #P/dmi or empty) */
int	dmi_check_system(const struct dmi_system_id *list);
const struct dmi_system_id	*dmi_first_match(const struct dmi_system_id *list);
const char	*dmi_get_system_info(int field);
#define dmi_match(f, s)		0

/* an SMBIOS structure's header (the SMBIOS specification); no tables to walk yet */
struct dmi_header { u8 type; u8 length; u16 handle; } __packed;
#define DMI_ENTRY_MEM_DEVICE	17
static inline int dmi_walk(void (*decode)(const struct dmi_header *, void *), void *private) { return -ENXIO; }

#endif
