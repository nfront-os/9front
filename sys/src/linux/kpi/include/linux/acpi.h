/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/acpi.h (ours, not Linux's) */
#ifndef _KPI_LINUX_ACPI_H
#define _KPI_LINUX_ACPI_H

#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/uuid.h>
#include <linux/slab.h>

/*
 * ACPI through 9front's AML interpreter (libaml, kpi/plan9/p9acpi.c,
 * kpi/linux/acpi.c): a handle is a name of the namespace; methods are
 * evaluated, a PCI device's node found by _ADR.  No ACPI device objects
 * beyond the handle, no notifications (no SCI here: notifier.h).
 */
typedef void *acpi_handle;
typedef u32 acpi_status;
typedef u64 acpi_size;
struct device;
/* 9front: an ACPI device object is only its node */
struct acpi_device { acpi_handle handle; };
struct acpi_buffer { acpi_size length; void *pointer; };
#define ACPI_ALLOCATE_BUFFER		((acpi_size)-1)

/* an object a method returns or takes (ACPICA's layout) */
#define ACPI_TYPE_ANY			0
#define ACPI_TYPE_INTEGER		1
#define ACPI_TYPE_STRING		2
#define ACPI_TYPE_BUFFER		3
#define ACPI_TYPE_PACKAGE		4
#define ACPI_TYPE_LOCAL_REFERENCE	0x14
union acpi_object {
	u32	type;
	struct { u32 type; u64 value; } integer;
	struct { u32 type; u32 length; char *pointer; } string;
	struct { u32 type; u32 length; u8 *pointer; } buffer;
	struct { u32 type; u32 count; union acpi_object *elements; } package;
	struct { u32 type; u32 actual_type; acpi_handle handle; } reference;
};
struct acpi_object_list { u32 count; union acpi_object *pointer; };

/* the header of every ACPI table (ACPI spec 5.2.6) */
struct acpi_table_header {
	char	signature[4];
	u32	length;
	u8	revision;
	u8	checksum;
	char	oem_id[6];
	char	oem_table_id[8];
	u32	oem_revision;
	char	asl_compiler_id[4];
	u32	asl_compiler_revision;
} __packed;

/* ACPICA's actypes.h gives the Linux code these */
#ifndef TRUE
#define TRUE				(1 == 1)
#define FALSE				(1 == 0)
#endif
#define AE_OK				0
#define AE_ERROR			1
#define AE_NOT_FOUND			5
#define AE_NOT_EXIST			6
#define AE_TYPE				8
#define AE_BUFFER_OVERFLOW		0x000B
#define AE_BAD_PARAMETER		0x1001
#define AE_CTRL_TERMINATE		0x4002
#define return_ACPI_STATUS(s)		return (s)
#define ACPI_FREE(p)			kfree(p)
#define ACPI_FULL_PATHNAME		0
#define ACPI_ID_LEN			16
#define ACPI_VIDEO_CLASS		"video"
#define ACPI_AC_CLASS			"ac_adapter"
#define ACPI_VIDEO_NOTIFY_PROBE		0x81

/* the namespace (kpi/linux/acpi.c) */
acpi_status acpi_evaluate_object(acpi_handle h, const char *path,
	struct acpi_object_list *args, struct acpi_buffer *ret);
acpi_status acpi_evaluate_integer(acpi_handle h, const char *path,
	struct acpi_object_list *args, unsigned long long *v);
union acpi_object *acpi_evaluate_dsm_typed(acpi_handle h, const guid_t *guid,
	u64 rev, u64 func, union acpi_object *argv4, u32 type);
#define acpi_evaluate_dsm(h, g, r, f, a)	acpi_evaluate_dsm_typed(h, g, r, f, a, ACPI_TYPE_ANY)
bool acpi_check_dsm(acpi_handle h, const guid_t *guid, u64 rev, u64 funcs);
bool acpi_has_method(acpi_handle h, const char *name);
acpi_status acpi_get_name(acpi_handle h, u32 type, struct acpi_buffer *b);
const char *acpi_format_exception(acpi_status s);
acpi_status acpi_get_handle(acpi_handle parent, const char *name, acpi_handle *ret);
acpi_status acpi_get_devices(const char *hid, acpi_status (*fn)(acpi_handle h, u32 level, void *ctx, void **ret),
	void *ctx, void **ret);
bool acpi_dev_present(const char *hid, const char *uid, s64 hrv);
/* device objects by _HID (amdgpu's MI300 XCCs): none, no ACPI device objects here */
static inline struct acpi_device *acpi_dev_get_first_match_dev(const char *hid, const char *uid, s64 hrv) { return NULL; }
static inline void acpi_dev_put(struct acpi_device *a) { }
#define acpi_handle_debug(h, fmt, ...)	do { } while (0)
/* a device's node: a PCI device's, found at the first use; else none */
int kpi_acpi_enabled(void);
acpi_handle kpi_acpi_handle(struct device *d);
struct acpi_device *kpi_acpi_companion(struct device *d);
#define KPI_ACPI_PCI			((void *)1)	/* struct device.acpi: a PCI device, not yet looked up */
/* the tables are there (#P/acpitbls): kpi/linux/acpi.c */
acpi_status acpi_get_table(const char *sig, u32 instance, struct acpi_table_header **t);
void acpi_put_table(struct acpi_table_header *t);
#define ACPI_HANDLE(d)			kpi_acpi_handle(d)
#define ACPI_COMPANION(d)		kpi_acpi_companion(d)
#define ACPI_FAILURE(s)			((s) != 0)
#define ACPI_SUCCESS(s)			((s) == 0)
#define acpi_disabled			(!kpi_acpi_enabled())
#define has_acpi_companion(d)		(kpi_acpi_companion(d) != NULL)
#define acpi_lid_open()			1
#define acpi_target_system_state()	0
#define acpi_device_handle(a)		((a) ? (a)->handle : (acpi_handle)NULL)
/* ACPI's structures the drivers name (ACPI spec; ACPICA's layout) */
typedef char acpi_device_class[20];
typedef char acpi_bus_id[8];
struct acpi_bus_event {
	struct list_head	node;
	acpi_device_class	device_class;
	acpi_bus_id	bus_id;
	u32	type;
	u32	data;
};
struct acpi_resource_source { u8 index; u16 string_length; char *string_ptr; };
struct acpi_resource_i2c_serialbus {
	u8	revision_id, type, producer_consumer, slave_mode, connection_sharing, type_revision_id;
	u16	type_data_length, vendor_length;
	struct acpi_resource_source	resource_source;
	u8	*vendor_data;
	u8	access_mode;
	u16	slave_address;
	u32	connection_speed;
};
struct acpi_resource;
struct resource_entry;
/* no ACPI device objects: no resources */
static inline int acpi_dev_get_resources(struct acpi_device *a, struct list_head *l,
	int (*f)(struct acpi_resource *r, void *d), void *d) { return -ENODEV; }
static inline void acpi_dev_free_resource_list(struct list_head *l) { }
#define ACPI_STATE_S3			3
#define ACPI_STATE_S0			0

#endif
