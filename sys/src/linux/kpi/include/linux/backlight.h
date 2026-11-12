/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/backlight.h (ours, not Linux's) */
#ifndef _KPI_LINUX_BACKLIGHT_H
#define _KPI_LINUX_BACKLIGHT_H

#include <linux/device.h>
#include <linux/mutex.h>

enum backlight_type { BACKLIGHT_RAW = 1, BACKLIGHT_PLATFORM, BACKLIGHT_FIRMWARE, BACKLIGHT_TYPE_MAX };
enum backlight_scale { BACKLIGHT_SCALE_UNKNOWN = 0, BACKLIGHT_SCALE_LINEAR, BACKLIGHT_SCALE_NON_LINEAR };
#define BL_CORE_SUSPENDED	(1 << 0)
#define BL_CORE_SUSPENDRESUME	(1 << 0)	/* backlight_ops.options: dimmed while suspended */
#define BL_CORE_FBBLANK		(1 << 1)
#define FB_BLANK_UNBLANK	0
#define FB_BLANK_POWERDOWN	4
struct backlight_device;
struct backlight_properties { int brightness; int max_brightness; int power; int fb_blank; enum backlight_type type; unsigned int state; enum backlight_scale scale; };
struct backlight_ops { unsigned int options; int (*update_status)(struct backlight_device *b); int (*get_brightness)(struct backlight_device *b); };
struct backlight_device { struct backlight_properties props; struct mutex update_lock; struct mutex ops_lock; const struct backlight_ops *ops; struct device dev; void *priv; };
/* the panel's backlight: registered by name (kpi/linux/device.c); the file server may offer it */
struct backlight_device	*backlight_device_register(const char *name, struct device *parent, void *data, const struct backlight_ops *ops, const struct backlight_properties *props);
void	backlight_device_unregister(struct backlight_device *b);
struct backlight_device	*backlight_device_get_by_name(const char *name);
int	backlight_device_set_brightness(struct backlight_device *b, unsigned long brightness);
/* unregistered with the device (devres); dev's */
struct backlight_device	*devm_backlight_device_register(struct device *dev, const char *name, struct device *parent, void *data, const struct backlight_ops *ops, const struct backlight_properties *props);
/* the backlight a device tree names for dev: there is no device tree here */
static inline struct backlight_device *devm_of_find_backlight(struct device *dev) { return NULL; }
static inline void *bl_get_data(struct backlight_device *b) { return b->priv; }
static inline int backlight_update_status(struct backlight_device *b) { return b->ops && b->ops->update_status ? b->ops->update_status(b) : 0; }
static inline int backlight_get_brightness(const struct backlight_device *b) { return b->props.power == FB_BLANK_UNBLANK ? b->props.brightness : 0; }
static inline bool backlight_is_blank(const struct backlight_device *b) { return b->props.power != FB_BLANK_UNBLANK || b->props.brightness == 0; }
static inline int backlight_enable(struct backlight_device *b) { if (!b) return 0; b->props.power = FB_BLANK_UNBLANK; b->props.state &= ~BL_CORE_FBBLANK; return backlight_update_status(b); }
static inline int backlight_disable(struct backlight_device *b) { if (!b) return 0; b->props.power = FB_BLANK_POWERDOWN; b->props.state |= BL_CORE_FBBLANK; return backlight_update_status(b); }
#define backlight_device_set_brightness_compat(b, v)	backlight_device_set_brightness(b, v)
#define put_device_backlight(b)		do { } while (0)

/* 9front: Linux 6.14's names of the power states */
#define BACKLIGHT_POWER_ON		FB_BLANK_UNBLANK
#define BACKLIGHT_POWER_REDUCED		1
#define BACKLIGHT_POWER_OFF		FB_BLANK_POWERDOWN

#endif
