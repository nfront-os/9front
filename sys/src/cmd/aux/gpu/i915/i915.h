/*
 * SPDX-License-Identifier: MIT
 *
 * aux/i915: what the Plan 9 side calls on the Linux side (linux/i915.c);
 * plain C types only, both sides include it.
 */
int	i915_start(void);	/* i915's module_init: probes the device; 0 or -errno */
/* before i915_start: no display (i915.disable_display), DRM's debug categories, printk's level */
void	i915_options(int nodisplay, unsigned long drmdebug, int loglevel);
int	i915_selftest(void);	/* -T: the KPI on this machine; 0 or -errno */

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
char	*i915_status(void);	/* i915_freestr */
void	i915_freestr(char *s);
#define i915_iserr(p)	((unsigned long)(p) >= (unsigned long)-4095)
