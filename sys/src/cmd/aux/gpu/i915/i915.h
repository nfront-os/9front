/*
 * SPDX-License-Identifier: MIT
 *
 * aux/i915: what the Plan 9 side calls on the Linux side (linux/i915.c);
 * plain C types only, both sides include it.
 */
int	i915_start(void);	/* i915's module_init: probes the device; 0 or -errno */
/* before i915_start: no display (i915.disable_display), DRM's debug categories, printk's level */
void	i915_options(int nodisplay, unsigned long drmdebug, int loglevel);
int	i915_kpitest(void);
int	i915_mock(char *filter);
int	i915_live_result(int);	/* -L: after i915_start; 0 or -errno */	/* -M: i915's mock selftests; 0 or -errno */	/* -T: the KPI on this machine; 0 or -errno */

/*
 * gpu(4)'s contexts (linux/ctx.c): errors are -errno; a context is the
 * Linux side's, opaque here
 */
typedef unsigned long long	u64g;
void	*i915_ctx_new(int *id);	/* or an error pointer: i915_iserr */
void	i915_ctx_free(void *c);
int	i915_bo_new(void *c, u64g size, u64g *gpu, int *id, char *seg, int nseg, u64g *va, u64g *size1);	/* *gpu ~0: the driver chooses */
int	i915_bo_bind(void *c, int id, u64g gpu);
int	i915_bo_free(void *c, int id);
int	i915_submit(void *c, int id, unsigned int offset, int engine, u64g *k);	/* engine: 0 rcs, 1 bcs */
int	i915_fence_wait(void *c, u64g k, int query);	/* 1 done, 0 not, <0 none */
void	i915_fence_count(void *c, u64g *done, u64g *n, int *failed);
char	*i915_status(void);
long	i915_error_read(char *buf, long n, long long off);	/* the GPU's error state */
void	i915_error_clear(void);	/* i915_freestr */
void	i915_freestr(char *s);
#define i915_iserr(p)	((unsigned long)(p) >= (unsigned long)-4095)

/* the display (linux/kms.c, linux/ctx.c) */
int	i915_kms_start(void);
int	i915_outputs(char **names, int max);	/* names: i915_freestr each */
int	i915_output_edid(char *name, unsigned char *buf, int n);
char	*i915_output_modes(char *name);	/* i915_freestr */
char	*i915_output_mode(char *name);
char	*i915_plane(void);
int	i915_scanout(void *c, int id, unsigned int w, unsigned int h, unsigned int stride);
int	i915_map_plane(void *c, int *id, u64g *gpu, unsigned int *w, unsigned int *h, unsigned int *stride, unsigned int *bpp);
int	i915_getparam(void *c, int param, int *value);	/* i915's GETPARAM */
