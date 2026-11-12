/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: drm/drm_mipi_dsi.h (ours: Linux's is GPL or has no notice) */
#ifndef _KPI_DRM_DRM_MIPI_DSI_H
#define _KPI_DRM_DRM_MIPI_DSI_H

#include <linux/device.h>
#include <linux/types.h>
#include <video/mipi_display.h>
#include <linux/errno.h>

/*
 * MIPI DSI (ours): i915 drives DSI panels itself, as a DSI host; the
 * peripheral API it uses, without a DSI bus of devices behind it
 */
struct mipi_dsi_host;
struct mipi_dsi_device;
struct drm_dsc_picture_parameter_set;
#define MIPI_DSI_MSG_REQ_ACK	(1 << 0)
#define MIPI_DSI_MSG_USE_LPM	(1 << 1)
struct mipi_dsi_msg { u8 channel; u8 type; u16 flags; size_t tx_len; const void *tx_buf; size_t rx_len; void *rx_buf; };
struct mipi_dsi_packet { size_t size; u8 header[4]; size_t payload_length; const u8 *payload; };
struct mipi_dsi_host_ops {
	int	(*attach)(struct mipi_dsi_host *h, struct mipi_dsi_device *d);
	int	(*detach)(struct mipi_dsi_host *h, struct mipi_dsi_device *d);
	ssize_t	(*transfer)(struct mipi_dsi_host *h, const struct mipi_dsi_msg *m);
};
struct mipi_dsi_host { struct device *dev; const struct mipi_dsi_host_ops *ops; struct list_head list; };
#define MIPI_DSI_MODE_VIDEO		(1U << 0)
#define MIPI_DSI_MODE_VIDEO_BURST	(1U << 1)
#define MIPI_DSI_MODE_VIDEO_SYNC_PULSE	(1U << 2)
#define MIPI_DSI_CLOCK_NON_CONTINUOUS	(1U << 10)
#define MIPI_DSI_MODE_VIDEO_AUTO_VERT	(1U << 3)
#define MIPI_DSI_MODE_VIDEO_HSE		(1U << 4)
#define MIPI_DSI_MODE_VIDEO_NO_HFP	(1U << 5)
#define MIPI_DSI_MODE_VIDEO_NO_HBP	(1U << 6)
#define MIPI_DSI_MODE_VIDEO_NO_HSA	(1U << 7)
#define MIPI_DSI_MODE_VSYNC_FLUSH	(1U << 8)
#define MIPI_DSI_MODE_NO_EOT_PACKET	(1U << 9)
#define MIPI_DSI_MODE_LPM		(1U << 11)
#define MIPI_DSI_HS_PKT_END_ALIGNED	(1U << 12)
#define MIPI_DSI_MODULE_PREFIX		"mipi-dsi:"
enum mipi_dsi_pixel_format { MIPI_DSI_FMT_RGB888, MIPI_DSI_FMT_RGB666, MIPI_DSI_FMT_RGB666_PACKED, MIPI_DSI_FMT_RGB565 };
#define DSI_DEV_NAME_SIZE		20
struct mipi_dsi_device { struct mipi_dsi_host *host; struct device dev; bool attached; char name[DSI_DEV_NAME_SIZE];
	unsigned int channel; unsigned int lanes; enum mipi_dsi_pixel_format format; unsigned long mode_flags;
	unsigned long hs_rate; unsigned long lp_rate; struct drm_dsc_config *dsc; };
struct mipi_dsi_device_info { char type[DSI_DEV_NAME_SIZE]; u32 channel; struct device_node *node; };
static inline int mipi_dsi_pixel_format_to_bpp(enum mipi_dsi_pixel_format f)
{ switch (f) { case MIPI_DSI_FMT_RGB888: case MIPI_DSI_FMT_RGB666: return 24; case MIPI_DSI_FMT_RGB666_PACKED: return 18; case MIPI_DSI_FMT_RGB565: return 16; } return -EINVAL; }
#define to_mipi_dsi_device(d)		container_of(d, struct mipi_dsi_device, dev)
bool	mipi_dsi_packet_format_is_short(u8 type);
bool	mipi_dsi_packet_format_is_long(u8 type);
int	mipi_dsi_create_packet(struct mipi_dsi_packet *p, const struct mipi_dsi_msg *m);
int	mipi_dsi_host_register(struct mipi_dsi_host *h);
void	mipi_dsi_host_unregister(struct mipi_dsi_host *h);
struct mipi_dsi_device	*mipi_dsi_device_register_full(struct mipi_dsi_host *h, const struct mipi_dsi_device_info *info);
void	mipi_dsi_device_unregister(struct mipi_dsi_device *d);
int	mipi_dsi_attach(struct mipi_dsi_device *d);
int	mipi_dsi_detach(struct mipi_dsi_device *d);
int	mipi_dsi_compression_mode(struct mipi_dsi_device *d, bool enable);	/* 9front: int since Linux 6.9 */
int	mipi_dsi_picture_parameter_set(struct mipi_dsi_device *d, const struct drm_dsc_picture_parameter_set *pps);
int	mipi_dsi_set_maximum_return_packet_size(struct mipi_dsi_device *d, u16 value);
ssize_t	mipi_dsi_generic_write(struct mipi_dsi_device *d, const void *payload, size_t size);
ssize_t	mipi_dsi_generic_read(struct mipi_dsi_device *d, const void *params, size_t n, void *data, size_t size);
ssize_t	mipi_dsi_dcs_write_buffer(struct mipi_dsi_device *d, const void *data, size_t len);
ssize_t	mipi_dsi_dcs_write(struct mipi_dsi_device *d, u8 cmd, const void *data, size_t len);
ssize_t	mipi_dsi_dcs_read(struct mipi_dsi_device *d, u8 cmd, void *data, size_t len);
enum mipi_dsi_dcs_tear_mode { MIPI_DSI_DCS_TEAR_MODE_VBLANK, MIPI_DSI_DCS_TEAR_MODE_VHBLANK };
#define MIPI_DSI_DCS_POWER_MODE_DISPLAY	(1 << 2)
#define MIPI_DSI_DCS_POWER_MODE_NORMAL	(1 << 3)
#define MIPI_DSI_DCS_POWER_MODE_SLEEP	(1 << 4)
#define MIPI_DSI_DCS_POWER_MODE_PARTIAL	(1 << 5)
#define MIPI_DSI_DCS_POWER_MODE_IDLE	(1 << 6)
int	mipi_dsi_dcs_nop(struct mipi_dsi_device *d);
int	mipi_dsi_dcs_soft_reset(struct mipi_dsi_device *d);
int	mipi_dsi_dcs_get_power_mode(struct mipi_dsi_device *d, u8 *mode);
int	mipi_dsi_dcs_get_pixel_format(struct mipi_dsi_device *d, u8 *format);
int	mipi_dsi_dcs_enter_sleep_mode(struct mipi_dsi_device *d);
int	mipi_dsi_dcs_exit_sleep_mode(struct mipi_dsi_device *d);
int	mipi_dsi_dcs_set_display_off(struct mipi_dsi_device *d);
int	mipi_dsi_dcs_set_display_on(struct mipi_dsi_device *d);
int	mipi_dsi_dcs_set_column_address(struct mipi_dsi_device *d, u16 start, u16 end);
int	mipi_dsi_dcs_set_page_address(struct mipi_dsi_device *d, u16 start, u16 end);
int	mipi_dsi_dcs_set_tear_off(struct mipi_dsi_device *d);
int	mipi_dsi_dcs_set_tear_on(struct mipi_dsi_device *d, enum mipi_dsi_dcs_tear_mode mode);
int	mipi_dsi_dcs_set_pixel_format(struct mipi_dsi_device *d, u8 format);
int	mipi_dsi_dcs_set_tear_scanline(struct mipi_dsi_device *d, u16 scanline);
int	mipi_dsi_shutdown_peripheral(struct mipi_dsi_device *d);
int	mipi_dsi_turn_on_peripheral(struct mipi_dsi_device *d);
int	devm_mipi_dsi_attach(struct device *dev, struct mipi_dsi_device *d);
struct mipi_dsi_host	*of_find_mipi_dsi_host_by_node(struct device_node *node);
struct mipi_dsi_device	*of_find_mipi_dsi_device_by_node(struct device_node *np);
struct mipi_dsi_device	*devm_mipi_dsi_device_register_full(struct device *dev, struct mipi_dsi_host *h, const struct mipi_dsi_device_info *info);
struct mipi_dsi_driver { struct device_driver driver; int (*probe)(struct mipi_dsi_device *d); void (*remove)(struct mipi_dsi_device *d); void (*shutdown)(struct mipi_dsi_device *d); };
int	mipi_dsi_driver_register_full(struct mipi_dsi_driver *drv, struct module *owner);
void	mipi_dsi_driver_unregister(struct mipi_dsi_driver *drv);
static inline struct mipi_dsi_driver *to_mipi_dsi_driver(struct device_driver *d) { return container_of(d, struct mipi_dsi_driver, driver); }
static inline void *mipi_dsi_get_drvdata(const struct mipi_dsi_device *d) { return dev_get_drvdata(&d->dev); }
static inline void mipi_dsi_set_drvdata(struct mipi_dsi_device *d, void *data) { dev_set_drvdata(&d->dev, data); }
int	mipi_dsi_dcs_set_display_brightness(struct mipi_dsi_device *d, u16 brightness);
int	mipi_dsi_dcs_get_display_brightness(struct mipi_dsi_device *d, u16 *brightness);
int	mipi_dsi_dcs_set_display_brightness_large(struct mipi_dsi_device *d, u16 brightness);
int	mipi_dsi_dcs_get_display_brightness_large(struct mipi_dsi_device *d, u16 *brightness);

/*
 * 9front: Linux 6.10 on: compression by algorithm, and the _multi calls,
 * which keep the first error in a context and do nothing after it (the
 * prototypes from drm_mipi_dsi.c, MIT)
 */
enum mipi_dsi_compression_algo {
	MIPI_DSI_COMPRESSION_DSC = 0,
	MIPI_DSI_COMPRESSION_VENDOR = 3,
};

struct mipi_dsi_multi_context {
	struct mipi_dsi_device	*dsi;
	int	accum_err;
};

int mipi_dsi_compression_mode_ext(struct mipi_dsi_device *dsi, bool enable, enum mipi_dsi_compression_algo algo, unsigned int pps_selector);
void mipi_dsi_generic_write_multi(struct mipi_dsi_multi_context *ctx, const void *payload, size_t size);
void mipi_dsi_dual_generic_write_multi(struct mipi_dsi_multi_context *ctx, struct mipi_dsi_device *dsi1, struct mipi_dsi_device *dsi2, const void *payload, size_t size);
int mipi_dsi_dcs_write_buffer_chatty(struct mipi_dsi_device *dsi, const void *data, size_t len);
void mipi_dsi_dcs_write_buffer_multi(struct mipi_dsi_multi_context *ctx, const void *data, size_t len);
void mipi_dsi_dual_dcs_write_buffer_multi(struct mipi_dsi_multi_context *ctx, struct mipi_dsi_device *dsi1, struct mipi_dsi_device *dsi2, const void *data, size_t len);
void mipi_dsi_dcs_read_multi(struct mipi_dsi_multi_context *ctx, u8 cmd, void *data, size_t len);
void mipi_dsi_picture_parameter_set_multi(struct mipi_dsi_multi_context *ctx, const struct drm_dsc_picture_parameter_set *pps);
void mipi_dsi_compression_mode_ext_multi(struct mipi_dsi_multi_context *ctx, bool enable, enum mipi_dsi_compression_algo algo, unsigned int pps_selector);
void mipi_dsi_compression_mode_multi(struct mipi_dsi_multi_context *ctx, bool enable);
void mipi_dsi_dcs_nop_multi(struct mipi_dsi_multi_context *ctx);
void mipi_dsi_dcs_enter_sleep_mode_multi(struct mipi_dsi_multi_context *ctx);
void mipi_dsi_dcs_exit_sleep_mode_multi(struct mipi_dsi_multi_context *ctx);
void mipi_dsi_dcs_set_display_off_multi(struct mipi_dsi_multi_context *ctx);
void mipi_dsi_dcs_set_display_on_multi(struct mipi_dsi_multi_context *ctx);
void mipi_dsi_dcs_set_tear_on_multi(struct mipi_dsi_multi_context *ctx, enum mipi_dsi_dcs_tear_mode mode);
void mipi_dsi_turn_on_peripheral_multi(struct mipi_dsi_multi_context *ctx);
void mipi_dsi_dcs_set_tear_off_multi(struct mipi_dsi_multi_context *ctx);
void mipi_dsi_dcs_soft_reset_multi(struct mipi_dsi_multi_context *ctx);
void mipi_dsi_dcs_set_display_brightness_multi(struct mipi_dsi_multi_context *ctx, u16 brightness);
void mipi_dsi_dcs_set_pixel_format_multi(struct mipi_dsi_multi_context *ctx, u8 format);
void mipi_dsi_dcs_set_column_address_multi(struct mipi_dsi_multi_context *ctx, u16 start, u16 end);
void mipi_dsi_dcs_set_page_address_multi(struct mipi_dsi_multi_context *ctx, u16 start, u16 end);
void mipi_dsi_dcs_set_tear_scanline_multi(struct mipi_dsi_multi_context *ctx, u16 scanline);
void mipi_dsi_shutdown_peripheral_multi(struct mipi_dsi_multi_context *ctx);

#endif
