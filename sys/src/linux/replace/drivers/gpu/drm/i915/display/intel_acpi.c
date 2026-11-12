// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: intel_acpi, our own (Linux's is GPL).  The
 * graphics device's _DSM, revision 1: under UUID
 * 3e5b41c6-eb1d-4260-9d15-c71fbadae414, function 0 makes the BIOS hand
 * out its data, which the OpRegion's later requests read (called as it
 * is, its answer not used); UUID 7ed873d3-c2d0-4e4f-a854-0f1317b01c2c
 * is the MUX of switchable graphics, not handled here.  The namespace
 * through the machine's AML interpreter (kpi/linux/acpi.c).  No ACPI
 * video devices.  The
 * display ids of the connectors (the ACPI spec's _DOD scheme) are what
 * the OpRegion's lists of attached displays use.
 */
#include <linux/acpi.h>
#include <linux/pci.h>
#include <drm/drm_connector.h>
#include <drm/drm_print.h>
#include "intel_display_core.h"
#include "intel_acpi.h"
#include "intel_display_types.h"

/* a display's id (ACPI spec, appendix B, _DOD): index 3:0, type 11:8 */
#define DISPLAY_INDEX_SHIFT	0
#define DISPLAY_TYPE_SHIFT	8
#define DISPLAY_TYPE_MASK	(0xf << DISPLAY_TYPE_SHIFT)
#define DISPLAY_TYPE_OTHER	(0 << DISPLAY_TYPE_SHIFT)
#define DISPLAY_TYPE_VGA	(1 << DISPLAY_TYPE_SHIFT)
#define DISPLAY_TYPE_TV		(2 << DISPLAY_TYPE_SHIFT)
#define DISPLAY_TYPE_EXTERNAL	(3 << DISPLAY_TYPE_SHIFT)
#define DISPLAY_TYPE_INTERNAL	(4 << DISPLAY_TYPE_SHIFT)

#define INTEL_DSM_REVISION	1
#define INTEL_DSM_BIOS_DATA	0	/* function: the BIOS data functions supported */

static const guid_t intel_dsm_biosdata_guid =
	GUID_INIT(0x3e5b41c6, 0xeb1d, 0x4260, 0x9d, 0x15, 0xc7, 0x1f, 0xba, 0xda, 0xe4, 0x14);

/* switchable graphics (a MUX between two GPUs): none handled here */
void intel_register_dsm_handler(void) { }
void intel_unregister_dsm_handler(void) { }

void
intel_dsm_get_bios_data_funcs_supported(struct intel_display *display)
{
	struct pci_dev *pdev = to_pci_dev(display->drm->dev);
	union acpi_object *o;
	acpi_handle h;

	h = ACPI_HANDLE(&pdev->dev);
	if (!h) {
		drm_dbg_kms(display->drm, "ACPI: no node for the graphics device: no _DSM\n");
		return;
	}
	/* calling it is what counts; the answer is not used */
	o = acpi_evaluate_dsm(h, &intel_dsm_biosdata_guid, INTEL_DSM_REVISION, INTEL_DSM_BIOS_DATA, NULL);
	drm_info(display->drm, "ACPI: Intel _DSM (BIOS data): %s\n", o ? "called" : "none");
	ACPI_FREE(o);
}
void intel_acpi_assign_connector_fwnodes(struct intel_display *display) { }
void intel_acpi_video_register(struct intel_display *display) { }

static u32
display_type(struct intel_connector *c)
{
	switch (c->base.connector_type) {
	case DRM_MODE_CONNECTOR_VGA:
	case DRM_MODE_CONNECTOR_DVIA:
		return DISPLAY_TYPE_VGA;
	case DRM_MODE_CONNECTOR_Composite:
	case DRM_MODE_CONNECTOR_SVIDEO:
	case DRM_MODE_CONNECTOR_Component:
	case DRM_MODE_CONNECTOR_9PinDIN:
	case DRM_MODE_CONNECTOR_TV:
		return DISPLAY_TYPE_TV;
	case DRM_MODE_CONNECTOR_DVII:
	case DRM_MODE_CONNECTOR_DVID:
	case DRM_MODE_CONNECTOR_DisplayPort:
	case DRM_MODE_CONNECTOR_HDMIA:
	case DRM_MODE_CONNECTOR_HDMIB:
		return DISPLAY_TYPE_EXTERNAL;
	case DRM_MODE_CONNECTOR_LVDS:
	case DRM_MODE_CONNECTOR_eDP:
	case DRM_MODE_CONNECTOR_DSI:
		return DISPLAY_TYPE_INTERNAL;
	default:
		return DISPLAY_TYPE_OTHER;
	}
}

/* each connector's id: its type, numbered within the type */
void
intel_acpi_device_id_update(struct intel_display *display)
{
	struct drm_connector_list_iter it;
	struct intel_connector *c;
	u8 next[16] = { 0 };
	u32 type;

	drm_connector_list_iter_begin(display->drm, &it);
	for_each_intel_connector_iter(c, &it) {
		type = display_type(c);
		c->acpi_device_id = type | next[type >> DISPLAY_TYPE_SHIFT]++ << DISPLAY_INDEX_SHIFT;
	}
	drm_connector_list_iter_end(&it);
}
