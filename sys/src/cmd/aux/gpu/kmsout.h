/* SPDX-License-Identifier: MIT */
/*
 * aux/i915, aux/amdgpu: the outputs and the screen, the Linux side,
 * shared (kmsout.c, built into each driver's library); plain C types,
 * both sides include it.  The file server holds the primary node open
 * (a DRM master) and works through the KMS ioctls.  The screen of
 * draw(3) is one object, the bounding box of the outputs that are on;
 * each shows its part of it (SETCRTC at its x y), as an extended
 * desktop.  A client's picture of the screen's size goes to every
 * output (page flips), one of an output's size to that output.
 * Strings returned are kmalloc'd (the driver's freestr).
 */
#ifndef _KMSOUT_H
#define _KMSOUT_H

int	kmsout_names(char **names, int max);
int	kmsout_npipes(void);	/* outputs that can be on at once */
int	kmsout_edid(char *name, unsigned char *buf, int n);
char	*kmsout_modes(char *name);	/* "w x h hz" a line, the preferred first */
char	*kmsout_mode(char *name);	/* "w x h hz at x y", or "off" */
char	*kmsout_status(void);	/* "output conn connected|disconnected|unknown" a line */
char	*kmsout_plane(void);	/* "plane A 0 pitch 32 w h" */
/* the output's panel backlight: "brightness max"; set "n" or "n%" (nil or the error) */
int	kmsout_hasbacklight(char *name);
char	*kmsout_backlight(char *name);
const char	*kmsout_setbacklight(char *name, char *spec);

/*
 * the screen: every connected output on, in its preferred mode, side by
 * side (the first alone if that is bigger than maxbytes, w x h x 4: the
 * framebuffer devdraw draws in); its size, pitch, where the processor
 * writes it in the driver's BAR
 */
int	kmsout_screen(unsigned long long maxbytes, unsigned int *w, unsigned int *h, unsigned int *pitch, unsigned long long *baroff);
/*
 * an output's mode and place: "w x h [hz] [at x y]" or "off"; nil or
 * the error; the screen as it is after
 */
const char	*kmsout_set(char *name, char *spec, unsigned long long maxbytes, unsigned int *w, unsigned int *h, unsigned int *pitch, unsigned long long *baroff);
void	kmsout_free(char *s);	/* a string of kmsout's */
/* the mouse cursor (vga hwgc gpu): its image (cursor.h's clr and set, 32 bytes each, the offset), where it is */
int	kmsout_cursor_image(unsigned char *clr, unsigned char *set, int ox, int oy, int size);
void	kmsout_cursor_move(int x, int y, int on);
/* until a monitor may have come or gone (DRM's hotplug event after the one *seen): 0 or -EINTR */
int	kmsout_hotplug_wait(unsigned int *seen);
/* the layout after it: the newly connected on (right of the others), the gone off; nil or the error */
const char	*kmsout_relayout(unsigned long long maxbytes, int *changed, unsigned int *w, unsigned int *h, unsigned int *pitch, unsigned long long *baroff);

#ifndef __KERNEL__
/* the Plan 9 side's (kmsscreen.c): the screen through BAR barno; an output's mode set */
int	screenstart(char *gdir, int barno, char *drv);
char	*screenset(char *name, char *spec);
char	*screenstatus(void);	/* malloc'd, or nil (no screen) */
/* outputs/conn/mode written (bl 0), backlight written or read (spec nil, *s the text): on a proc with a big stack */
char	*outrequest(char *out, char *spec, int bl, char **s);
#endif

#ifdef __KERNEL__
struct file;
struct drm_connector;
struct backlight_device;

struct kmsout_ops {
	/*
	 * the screen object, w x h x8r8g8b8: a handle of the master file
	 * kms, its pitch, where the processor writes it
	 */
	int	(*screen_alloc)(struct file *kms, u32 w, u32 h, u32 *handle, u32 *pitch, unsigned long long *baroff);
	void	(*screen_free)(struct file *kms, u32 handle);
	/* the output's panel backlight, if it has one */
	struct backlight_device	*(*backlight)(struct drm_connector *c);
};

int	kmsout_init(struct file *kms, const struct kmsout_ops *ops);
int	kmsout_screenfb(u32 *fb, u32 *w, u32 *h, u32 *pitch);
int	kmsout_scanout(struct file *from, u32 handle, u32 w, u32 h, u32 stride);
int	kmsout_prime(struct file *from, u32 handle, struct file *to, u32 *out);
/* a client's window on an overlay plane: the object to copy its picture into, show it, off */
int	kmsout_overlay_back(struct file *from, u32 w, u32 h, u32 stride, struct file **kms, u32 *handle);
int	kmsout_overlay_show(struct file *from, int x, int y);
int	kmsout_overlay_direct(struct file *from, u32 handle, u32 w, u32 h, u32 stride, int x, int y);	/* the client's VRAM object itself */
void	kmsout_overlay_off(struct file *from);
#endif

#endif
