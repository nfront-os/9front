/* Public domain. */

#ifndef _ACPI_VIDEO_H
#define _ACPI_VIDEO_H

#include <linux/types.h>

static inline void
acpi_video_register(void)
{
}

static inline void
acpi_video_unregister(void)
{
}

static inline void
acpi_video_register_backlight(void)
{
}

static inline bool
acpi_video_backlight_use_native(void)
{
	return true;
}

static inline void
acpi_video_report_nolcd(void)
{
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/acpi.h>
enum acpi_backlight_type {
	acpi_backlight_undef = -1,
	acpi_backlight_none = 0,
	acpi_backlight_video,
	acpi_backlight_vendor,
	acpi_backlight_native,
	acpi_backlight_nvidia_wmi_ec,
	acpi_backlight_apple_gmux,
};
static inline enum acpi_backlight_type acpi_video_get_backlight_type(void) { return acpi_backlight_vendor; }

#define ACPI_VIDEO_CLASS	"video"

#define acpi_video_get_edid(adev, type, device, edid)	(-ENODEV)	/* 9front: no ACPI video */

#endif
