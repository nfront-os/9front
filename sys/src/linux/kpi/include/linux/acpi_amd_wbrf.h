/* Public domain. */
/*
 * 9front: AMD's WiFi band RFI mitigation (WBRF), which the platform's
 * ACPI offers on some laptops: the bands to keep the memory clock out
 * of.  9front has no such ACPI interface: no consumer is supported, so
 * amdgpu never asks.  (OpenBSD's stub, with the structures 7.3 needs.)
 */

#ifndef _ACPI_AMD_WBRF_H
#define _ACPI_AMD_WBRF_H

#include <linux/types.h>
#include <linux/errno.h>

#define MAX_NUM_OF_WBRF_RANGES	11
enum wbrf_notifier_actions { WBRF_CHANGED };

struct freq_band_range {
	u64	start;
	u64	end;
};

struct wbrf_ranges_in_out {
	u64	num_of_ranges;
	struct freq_band_range	band_list[MAX_NUM_OF_WBRF_RANGES];
};

struct device;
struct notifier_block;

#define acpi_amd_wbrf_supported_consumer(dev)	false
#define amd_wbrf_retrieve_freq_band(dev, out)	(-ENODEV)
#define amd_wbrf_register_notifier(nb)		(-ENODEV)
#define amd_wbrf_unregister_notifier(nb)	(-ENODEV)

#endif
