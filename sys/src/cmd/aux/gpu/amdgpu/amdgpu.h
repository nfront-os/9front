/*
 * SPDX-License-Identifier: MIT
 *
 * aux/amdgpu: what the Plan 9 side calls on the Linux side
 * (linux/amdgpu.c); plain C types only, both sides include it.
 */
/* before amdgpu_start: DRM's debug categories, printk's level */
void	amdgpu_options(unsigned long drmdebug, int loglevel);
int	amdgpu_start(void);	/* the scheduler's and amdgpu's module_init: probes the device; 0 or -errno */
char	*amdgpu_status(void);	/* amdgpu_freestr */
void	amdgpu_freestr(char *s);

/*
 * gpu(4)'s contexts (linux/ctx.c): errors are -errno; a context is the
 * Linux side's, opaque here
 */
typedef unsigned long long	u64g;
void	*amdgpu_ctx_new(int *id);	/* or an error pointer: amdgpu_iserr */
void	amdgpu_ctx_free(void *c);
int	amdgpu_bo_new(void *c, u64g size, u64g *gpu, int *id, char *seg, int nseg, u64g *va, u64g *size1);	/* *gpu ~0: the driver chooses */
int	amdgpu_bo_bind(void *c, int id, u64g gpu);
int	amdgpu_bo_free(void *c, int id);
int	amdgpu_submit(void *c, int id, unsigned int offset, unsigned int ndw, int ip, u64g *k);	/* ip: 0 gfx, 1 compute, 2 sdma */
int	amdgpu_fence_wait(void *c, u64g k, int query);	/* 1 done, 0 not, <0 none */
void	amdgpu_fence_count(void *c, u64g *done, u64g *n, int *failed);
#define amdgpu_iserr(p)	((unsigned long)(p) >= (unsigned long)-4095)
