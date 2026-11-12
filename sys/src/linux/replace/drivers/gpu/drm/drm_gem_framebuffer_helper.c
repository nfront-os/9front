/* Public domain. */
/*
 * 9front: OpenBSD's, with the LinuxKPI's allocator (kzalloc, kfree) and
 * drm/drm_gem_framebuffer_helper.h; every plane's object goes with the
 * framebuffer.
 */

#include <drm/drm_gem.h>
#include <drm/drm_framebuffer.h>
#include <drm/drm_modeset_helper.h>
#include <drm/drm_gem_framebuffer_helper.h>
#include <drm/drm_fourcc.h>
#include <linux/slab.h>

void
drm_gem_fb_destroy(struct drm_framebuffer *fb)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(fb->obj); i++)
		if (fb->obj[i])
			drm_gem_object_put(fb->obj[i]);
	drm_framebuffer_cleanup(fb);
	kfree(fb);
}

int
drm_gem_fb_create_handle(struct drm_framebuffer *fb, struct drm_file *file,
    unsigned int *handle)
{
	return drm_gem_handle_create(file, fb->obj[0], handle);
}

const struct drm_framebuffer_funcs drm_gem_fb_funcs = {
	.create_handle = drm_gem_fb_create_handle,
	.destroy = drm_gem_fb_destroy,
};

struct drm_framebuffer *
drm_gem_fb_create(struct drm_device *dev, struct drm_file *file,
		  const struct drm_format_info *info, const struct drm_mode_fb_cmd2 *cmd)
{
	/* 9front: Linux 6.17 hands the format's info in (the core looked it up) */
	struct drm_framebuffer *fb;
	struct drm_gem_object *gem_obj;
	int error;

	if (!info)
		return ERR_PTR(-EINVAL);

	if (info->num_planes != 1)	/* 9front: one plane only, as OpenBSD's */
		return ERR_PTR(-EINVAL);

	gem_obj = drm_gem_object_lookup(file, cmd->handles[0]);
	if (gem_obj == NULL)
		return ERR_PTR(-ENOENT);

	fb = kzalloc(sizeof(*fb), GFP_KERNEL);
	if (fb == NULL) {
		drm_gem_object_put(gem_obj);
		return ERR_PTR(-ENOMEM);
	}

	drm_helper_mode_fill_fb_struct(dev, fb, info, cmd);
	fb->obj[0] = gem_obj;

	error = drm_framebuffer_init(dev, fb, &drm_gem_fb_funcs);
	if (error != 0)
		goto dealloc;

	return fb;

dealloc:
	kfree(fb);	/* 9front: drm_framebuffer_init failed, nothing to clean up */
	drm_gem_object_put(gem_obj);

	return ERR_PTR(error);
}

struct drm_gem_object *
drm_gem_fb_get_obj(struct drm_framebuffer *fb, unsigned int plane)
{
	if (plane < ARRAY_SIZE(fb->obj))
		return fb->obj[plane];
	return NULL;
}
