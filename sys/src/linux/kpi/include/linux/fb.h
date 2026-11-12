/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/fb.h (ours, not Linux's) */
#ifndef _KPI_LINUX_FB_H
#define _KPI_LINUX_FB_H

#include <linux/kernel.h>
#include <linux/device.h>

/* no fbdev (CONFIG_DRM_FBDEV_EMULATION off): the types the headers name */
struct fb_info;
struct fb_ops;
struct fb_var_screeninfo { u32 xres, yres, xres_virtual, yres_virtual, xoffset, yoffset, bits_per_pixel, grayscale; u32 pixclock; };
struct fb_fix_screeninfo { char id[16]; unsigned long smem_start; u32 smem_len; u32 type; u32 visual; u32 line_length; unsigned long mmio_start; u32 mmio_len; };
struct fb_info { int node; struct fb_var_screeninfo var; struct fb_fix_screeninfo fix; const struct fb_ops *fbops; struct device *device; void *par; char __iomem *screen_base; unsigned long screen_size; void *pseudo_palette; };
#define KHZ2PICOS(a)		(1000000000UL / (a))
#define PICOS2KHZ(a)		(1000000000UL / (a))
#define FBINFO_STATE_RUNNING	0
#define FBINFO_STATE_SUSPENDED	1
#define fb_set_suspend(i, s)	do { } while (0)
#define FB_ACTIVATE_NOW		0

#endif
