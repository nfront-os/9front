/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: drm/drm_writeback.h (ours: Linux's is GPL) */
#ifndef _KPI_DRM_DRM_WRITEBACK_H
#define _KPI_DRM_DRM_WRITEBACK_H

#include <drm/drm_connector.h>
#include <drm/drm_encoder.h>
#include <linux/workqueue.h>

/* writeback connectors: not offered (no driver here has them) */
struct drm_writeback_connector { struct drm_connector base; struct drm_encoder encoder; struct drm_property_blob *pixel_formats_blob_ptr;
	spinlock_t job_lock; struct list_head job_queue; unsigned int fence_context; spinlock_t fence_lock; unsigned long fence_seqno; char timeline_name[32]; };
struct drm_writeback_job { struct drm_writeback_connector *connector; bool prepared; struct work_struct cleanup_work; struct list_head list_entry;
	struct drm_framebuffer *fb; struct dma_fence *out_fence; void *priv; };
static inline struct drm_writeback_connector *drm_connector_to_writeback(struct drm_connector *c) { return container_of(c, struct drm_writeback_connector, base); }
int	drm_writeback_set_fb(struct drm_connector_state *s, struct drm_framebuffer *fb);
int	drm_writeback_prepare_job(struct drm_writeback_job *job);
void	drm_writeback_queue_job(struct drm_writeback_connector *c, struct drm_connector_state *s);
void	drm_writeback_cleanup_job(struct drm_writeback_job *job);
void	drm_writeback_signal_completion(struct drm_writeback_connector *c, int status);
struct dma_fence	*drm_writeback_get_out_fence(struct drm_writeback_connector *c);

int	drm_writeback_connector_init(struct drm_device *dev, struct drm_writeback_connector *wb,
	const struct drm_connector_funcs *con_funcs, const struct drm_encoder_helper_funcs *enc_helper_funcs,
	const u32 *formats, int n_formats, u32 possible_crtcs);

#endif
