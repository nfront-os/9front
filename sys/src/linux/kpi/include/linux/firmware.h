/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/firmware.h (ours, not Linux's) */
#ifndef _KPI_LINUX_FIRMWARE_H
#define _KPI_LINUX_FIRMWARE_H

#include <linux/kernel.h>
#include <linux/device.h>

/* firmware: files in /lib/firmware (kpi/firmware.c) */
struct firmware { size_t size; const u8 *data; void *priv; };
int	request_firmware(const struct firmware **fw, const char *name, struct device *d);
int	firmware_request_nowarn(const struct firmware **fw, const char *name, struct device *d);
int	request_firmware_direct(const struct firmware **fw, const char *name, struct device *d);
int	request_firmware_into_buf(const struct firmware **fw, const char *name, struct device *d, void *buf, size_t size);
void	release_firmware(const struct firmware *fw);
int	request_firmware_nowait(struct module *m, bool uevent, const char *name, struct device *d, gfp_t gfp, void *ctx, void (*cont)(const struct firmware *fw, void *ctx));

#endif
