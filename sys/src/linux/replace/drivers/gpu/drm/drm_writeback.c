// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: drm_writeback.c (ours: Linux's is GPL).
 *
 * Writeback connectors (the display's output written to memory): no
 * driver here makes one, so no state ever has a job.
 */
#include <linux/errno.h>
#include <drm/drm_atomic.h>
#include <drm/drm_writeback.h>

/* none made here: the caller goes without (amdgpu_dm logs it and carries on) */
int drm_writeback_connector_init(struct drm_device *dev, struct drm_writeback_connector *wb,
	const struct drm_connector_funcs *con_funcs, const struct drm_encoder_helper_funcs *enc_helper_funcs,
	const u32 *formats, int n_formats, u32 possible_crtcs)
{
	return -ENODEV;
}

int drm_writeback_set_fb(struct drm_connector_state *conn_state, struct drm_framebuffer *fb)
{
	return -EOPNOTSUPP;
}

int drm_writeback_prepare_job(struct drm_writeback_job *job)
{
	return 0;
}

void drm_writeback_queue_job(struct drm_writeback_connector *wb_connector, struct drm_connector_state *conn_state)
{
}

void drm_writeback_cleanup_job(struct drm_writeback_job *job)
{
	kfree(job);
}

void drm_writeback_signal_completion(struct drm_writeback_connector *wb_connector, int status)
{
}

struct dma_fence *drm_writeback_get_out_fence(struct drm_writeback_connector *wb_connector)
{
	return NULL;
}
