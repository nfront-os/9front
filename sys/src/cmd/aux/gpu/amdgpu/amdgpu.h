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
int	amdgpu_ih_kick(void);	/* ctl "ih process" */
int	amdgpu_waves(void);	/* ctl "waves": the SQ's waves to the log */
void	amdgpu_freestr(char *s);
void	amdgpu_test_poweroff(void);	/* -X: orderly_poweroff, as at a critical temperature; does not return */

/*
 * gpu(4)'s contexts (linux/ctx.c): errors are -errno; a context is the
 * Linux side's, opaque here
 */
typedef unsigned long long	u64g;
void	*amdgpu_ctx_new(int *id);	/* or an error pointer: amdgpu_iserr */
void	amdgpu_ctx_free(void *c);
/* *gpu ~0: the driver chooses; seg at va: the object only (GTT: its pages; vram: its part of BAR 0) */
int	amdgpu_bo_new(void *c, u64g size, u64g *gpu, int vram, int *id, char *seg, int nseg, u64g *va, u64g *size1);
int	amdgpu_bo_bind(void *c, int id, u64g gpu);
int	amdgpu_bo_free(void *c, int id);
int	amdgpu_submit(void *c, int id, unsigned int offset, unsigned int ndw, int ip, u64g *k);	/* ip: 0 gfx, 1 compute, 2 sdma */
int	amdgpu_fence_wait(void *c, u64g k, int query);	/* 1 done, 0 not, <0 none */
void	amdgpu_fence_count(void *c, u64g *done, u64g *n, int *failed);
extern int amdgpu_fencetrace;	/* -t */
/* the client's objects mapped (linux/ctx.c) */
/* the context's requests as text (linux/req.c): the reply's length or -errno; isreq: a verb of them */
int	amdgpu_req(void *c, char **f, int n, char *reply, int nreply, int priv);
int	amdgpu_isreq(const char *verb);
int	amdgpu_map(void *c, unsigned int handle, char *seg, int nseg, u64g *va, u64g *size);
int	amdgpu_unmap(void *c, unsigned int handle);
/* a client's object between contexts by a token (gpu(4) export, import) */
int	amdgpu_export(void *c, unsigned int handle, char *token, int ntoken);
int	amdgpu_import(void *c, const char *token, unsigned int *handle, u64g *size);
int	amdgpu_gem_close(void *c, unsigned int handle);	/* ctl close: an object the client made or imported */
/* a syncobj between contexts by a token (gpu(4) syncexport, syncimport) */
int	amdgpu_syncexport(void *c, unsigned int handle, char *token, int ntoken);
int	amdgpu_syncimport(void *c, const char *token, unsigned int *handle);
#define amdgpu_iserr(p)	((unsigned long)(p) >= (unsigned long)-4095)

/* the display (linux/kms.c, linux/ctx.c) */
int	amdgpu_kms_start(void);
/* the outputs and the screen: kmsout.h (their strings: amdgpu_freestr) */
int	amdgpu_scanout(void *c, int id, unsigned int w, unsigned int h, unsigned int stride);	/* id 0: off again */
/* a client's object on an overlay plane at x y (linux/ctx.c: a GPU copy into VRAM); handle 0: off */
int	amdgpu_overlay(void *c, unsigned int handle, int x, int y, unsigned int w, unsigned int h, unsigned int stride);
int	amdgpu_scanout_gem(void *c, unsigned int handle, unsigned int w, unsigned int h, unsigned int stride);	/* a client's GEM object */
