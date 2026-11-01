// SPDX-License-Identifier: MIT
/*
 * aux/amdgpu: gpu(4)'s contexts on Linux's amdgpu, the Linux side.  A
 * context is an open DRM file of the render node (drm_open_helper), as a
 * client's descriptor is on Linux: amdgpu gives it a GPU address space of
 * its own (amdgpu_driver_open_kms: the file's VM), and the context holds
 * one amdgpu context (AMDGPU_CTX) on it.  The requests of gpu(4) become
 * amdgpu's ioctls, called through drm_ioctl as the kernel does for a
 * process (as libdrm_amdgpu calls them):
 *
 *	bo size [gpuaddr] [vram]
 *				GEM_CREATE in GTT (system memory the processor
 *				reaches), always valid in the file's VM; its
 *				pages, pinned, become an alias segment
 *				(segment(3)) the client attaches; GEM_VA maps
 *				it at gpuaddr (none given: the driver chooses;
 *				0: not mapped).  vram: in the card's memory,
 *				contiguous and pinned where the processor sees
 *				it through BAR 0 (the first 256 MB without a
 *				resized BAR); the client attaches BAR 0's
 *				write-combining segment (gpu(3)) and finds the
 *				object at its offset there.  Not always valid in
 *				the VM, so that it can go to the display (amdgpu
 *				exports no such object): each CS lists it
 *	bind id gpuaddr		GEM_VA: unmapped, mapped there instead
 *	submit id offset ndw ip	CS of one IB, ndw dwords at offset in the
 *				object, on ip (0 gfx, 1 compute, 2 sdma),
 *				with a syncobj for its fence: fence k
 *	free id			GEM_VA unmap, GEM_CLOSE, after the object's
 *				segment is revoked in every process
 *	scanout id w h stride	the object (VRAM) on the display (kms.c)
 */
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/dma-fence.h>
#include <linux/mutex.h>
#include <linux/pci.h>
#include <linux/vmalloc.h>
#include <drm/drm_drv.h>
#include <drm/drm_file.h>
#include <drm/drm_ioctl.h>
#include <drm/drm_syncobj.h>
#include <drm/amdgpu_drm.h>
#include <drm/ttm/ttm_tt.h>
#include "amdgpu.h"
#include "amdgpu_object.h"
#include "amdgpu_gem.h"
#include "amdgpu_vm.h"
#include "amdgpu_ttm.h"
#include "drivers/gpu/drm/drm_internal.h"
#include <kpi/p9.h>
#include "../amdgpu.h"

struct pci_dev	*kpi_pci_gpu(const char *gdir);
int	kms_scanout(struct file *from, u32 handle, u32 w, u32 h, u32 stride);

enum { Nfence = 64 };

/* where the driver puts objects for which the client names no address */
#define VASTART		(1ULL << 32)
#define VAEND		(1ULL << 47)	/* below the hole of the 48-bit space (AMDGPU_GMC_HOLE_START) */
#define VAFLAGS		(AMDGPU_VM_PAGE_READABLE | AMDGPU_VM_PAGE_WRITEABLE | AMDGPU_VM_PAGE_EXECUTABLE)

struct bo {
	bool	used;
	u32	handle;
	struct amdgpu_bo	*abo;	/* pinned: a reference of ours */
	u64	gpu;		/* mapped there, 0: not mapped */
	u64	size;
	bool	vram;		/* in VRAM: at baroff in BAR 0 */
	u64	baroff;
	char	seg[32];
	unsigned long	va;
};

struct gctx {
	int	id;
	struct mutex	lock;
	struct file	*filp;
	struct drm_file	*file;
	u32	ctxid;		/* the amdgpu context */
	u32	syncobj;	/* the out fence of each submission */
	struct bo	*bo;
	int	nbo;
	u64	nfence;
	u64	next;		/* the next address the driver chooses */
	struct dma_fence	*fence[Nfence];	/* fence k at k % Nfence */
	bool	failed;
};

static const struct file_operations ctx_fops = { };
static struct bo	*newbo(struct gctx *c);
static atomic_t nctx;

static struct drm_device *
drmdev(void)
{
	struct pci_dev *p = kpi_pci_gpu(NULL);

	return p ? pci_get_drvdata(p) : NULL;
}

static int
ioctl(struct gctx *c, unsigned int cmd, void *arg)
{
	return drm_ioctl(c->filp, cmd, (unsigned long)arg);
}

static void
closefile(struct gctx *c)
{
	struct drm_device *dev = c->file->minor->dev;

	/* drm_close_helper */
	mutex_lock(&dev->filelist_mutex);
	list_del(&c->file->lhead);
	mutex_unlock(&dev->filelist_mutex);
	drm_file_free(c->file);
	c->filp->private_data = NULL;
	fput(c->filp);
}

void *
amdgpu_ctx_new(int *id)
{
	struct drm_device *dev = drmdev();
	union drm_amdgpu_ctx ctx = { .in.op = AMDGPU_CTX_OP_ALLOC_CTX };
	struct drm_syncobj_create sc = { };
	struct gctx *c;
	int r;

	if (!dev || !dev->render)
		return ERR_PTR(-ENODEV);
	c = kzalloc(sizeof(*c), GFP_KERNEL);
	if (!c)
		return ERR_PTR(-ENOMEM);
	mutex_init(&c->lock);
	c->filp = kpi_alloc_file(NULL, &ctx_fops, NULL, O_RDWR);
	if (IS_ERR(c->filp)) {
		r = PTR_ERR(c->filp);
		kfree(c);
		return ERR_PTR(r);
	}
	r = drm_open_helper(c->filp, dev->render);
	if (r) {
		fput(c->filp);
		kfree(c);
		return ERR_PTR(r);
	}
	c->file = c->filp->private_data;
	r = ioctl(c, DRM_IOCTL_AMDGPU_CTX, &ctx);
	if (r)
		goto bad;
	c->ctxid = ctx.out.alloc.ctx_id;
	r = ioctl(c, DRM_IOCTL_SYNCOBJ_CREATE, &sc);
	if (r)
		goto bad;
	c->syncobj = sc.handle;
	c->id = *id = atomic_inc_return(&nctx);
	return c;
bad:
	closefile(c);	/* the file's release frees its amdgpu context and VM */
	kfree(c);
	return ERR_PTR(r);
}

static struct bo *
findbo(struct gctx *c, int id)
{
	if (id < 1 || id > c->nbo || !c->bo[id - 1].used)
		return NULL;
	return &c->bo[id - 1];
}

static int
vamap(struct gctx *c, struct bo *b, u64 gpu, u32 op)
{
	struct drm_amdgpu_gem_va va = {
		.handle = b->handle,
		.operation = op,
		.flags = op == AMDGPU_VA_OP_MAP ? VAFLAGS : 0,
		.va_address = gpu,
		.offset_in_bo = 0,
		.map_size = b->size,
	};

	return ioctl(c, DRM_IOCTL_AMDGPU_GEM_VA, &va);
}

static void
unpin(struct amdgpu_bo *abo)
{
	if (amdgpu_bo_reserve(abo, true) == 0) {
		amdgpu_bo_unpin(abo);
		amdgpu_bo_unreserve(abo);
	}
	drm_gem_object_put(&abo->tbo.base);	/* drm_gem_object_lookup's: a GEM reference, not TTM's */
}

static void
bofree(struct gctx *c, struct bo *b)
{
	struct drm_gem_close cl = { .handle = b->handle };

	/*
	 * removing the alias segment revokes it (segment(3)): a client that
	 * still has it attached loses the pages before amdgpu gets them back
	 */
	if (b->seg[0] && !b->vram)
		p9_segfree(b->seg, NULL);	/* BAR 0's segment stays: the client may still see the memory */
	if (b->gpu)
		vamap(c, b, b->gpu, AMDGPU_VA_OP_UNMAP);
	if (b->abo)
		unpin(b->abo);
	ioctl(c, DRM_IOCTL_GEM_CLOSE, &cl);
	memset(b, 0, sizeof(*b));
}

void
amdgpu_ctx_free(void *v)
{
	struct gctx *c = v;
	int i;

	kms_scanout(c->filp, 0, 0, 0, 0);	/* its picture off the display */
	for (i = 0; i < Nfence; i++)
		if (c->fence[i]) {
			dma_fence_wait_timeout(c->fence[i], false, 5 * HZ);
			dma_fence_put(c->fence[i]);
		}
	for (i = 0; i < c->nbo; i++)
		if (c->bo[i].used)
			bofree(c, &c->bo[i]);
	kfree(c->bo);
	closefile(c);	/* amdgpu_driver_postclose_kms: the contexts, the VM, the syncobjs */
	kfree(c);
}

/*
 * a place for an object of size at gpu in the context's address space:
 * page aligned, past amdgpu's reserved start, below the hole, apart from
 * the other mapped objects (but self)
 */
static bool
placeok(struct gctx *c, struct bo *self, u64 gpu, u64 size)
{
	int i;

	if (gpu & (AMDGPU_GPU_PAGE_SIZE - 1) || gpu < AMDGPU_VA_RESERVED_SIZE ||
	    gpu + size > VAEND || gpu + size < gpu)
		return false;
	for (i = 0; i < c->nbo; i++) {
		struct bo *b = &c->bo[i];

		if (!b->used || b == self || !b->gpu)
			continue;
		if (gpu < b->gpu + b->size && b->gpu < gpu + size)
			return false;
	}
	return true;
}

static u64
chooseaddr(struct gctx *c, u64 size)
{
	u64 a;

	if (!c->next)
		c->next = VASTART;
	a = c->next;
	c->next = ALIGN(a + size, 1ULL << 16);
	return a;
}

/* a new object; its segment's name, address and size for the client */
int
amdgpu_bo_new(void *v, u64 size, u64 *gpup, int vram, int *id, char *seg, int nseg, u64 *va, u64 *rsize, u64 *baroff, u64 *barsize)
{
	struct gctx *c = v;
	union drm_amdgpu_gem_create cr = {
		.in.bo_size = PAGE_ALIGN(size),
		.in.alignment = PAGE_SIZE,
		.in.domains = vram ? AMDGPU_GEM_DOMAIN_VRAM : AMDGPU_GEM_DOMAIN_GTT,
		.in.domain_flags = AMDGPU_GEM_CREATE_CPU_ACCESS_REQUIRED |
		    (vram ? 0 : AMDGPU_GEM_CREATE_VM_ALWAYS_VALID),
	};
	struct amdgpu_device *adev;
	struct drm_gem_object *gobj;
	struct ttm_tt *tt;
	struct bo *b;
	u64 gpu = *gpup;
	unsigned int n;
	int r;

	if (size == 0)
		return -EINVAL;
	mutex_lock(&c->lock);
	b = newbo(c);
	if (!b) {
		r = -ENOMEM;
		goto out;
	}
	r = ioctl(c, DRM_IOCTL_AMDGPU_GEM_CREATE, &cr);
	if (r) {
		pr_warn("amdgpu: ctx %d: bo of %llu bytes: GEM_CREATE: error %d\n", c->id, size, r);
		goto out;
	}
	b->handle = cr.out.handle;
	b->size = PAGE_ALIGN(size);	/* not cr.in: the union holds the reply now */
	gobj = drm_gem_object_lookup(c->file, b->handle);
	if (!gobj) {
		r = -ENOENT;
		goto close;
	}
	b->abo = gem_to_amdgpu_bo(gobj);	/* the lookup's reference is ours now */
	/* its memory, pinned while the client may have it */
	r = amdgpu_bo_reserve(b->abo, false);
	if (r)
		goto put;
	/*
	 * in VRAM in one piece, so that it is one range of BAR 0: the flag
	 * is the kernel's (GEM_CREATE does not take it), as amdgpu sets it
	 * itself for what the display scans out
	 */
	if (vram)
		b->abo->flags |= AMDGPU_GEM_CREATE_VRAM_CONTIGUOUS;
	r = amdgpu_bo_pin(b->abo, vram ? AMDGPU_GEM_DOMAIN_VRAM : AMDGPU_GEM_DOMAIN_GTT);
	amdgpu_bo_unreserve(b->abo);
	if (r) {
		pr_warn("amdgpu: ctx %d: bo %u: pin in %s: error %d\n", c->id, b->handle, vram ? "VRAM" : "GTT", r);
		goto put;
	}
	*baroff = *barsize = 0;
	if (vram) {
		/* where in VRAM, so where in BAR 0: CPU_ACCESS_REQUIRED keeps it in the visible part */
		adev = amdgpu_ttm_adev(b->abo->tbo.bdev);
		b->vram = true;
		b->baroff = (u64)b->abo->tbo.resource->start << PAGE_SHIFT;
		if (b->baroff + b->size > adev->gmc.visible_vram_size ||
		    p9_barseg(0, 1, b->seg, sizeof(b->seg)) < 0) {
			pr_warn("amdgpu: ctx %d: bo %u: not in BAR 0 (%#llx)\n", c->id, b->handle, b->baroff);
			r = -ENOSPC;
			goto unpin;
		}
		*baroff = b->baroff;
		*barsize = adev->gmc.visible_vram_size;
		b->va = 0;
		goto mapped;
	}
	tt = b->abo->tbo.ttm;
	n = b->size >> PAGE_SHIFT;
	if (!tt || !tt->pages || tt->num_pages < n) {
		pr_warn("amdgpu: ctx %d: bo %u: no pages in GTT\n", c->id, b->handle);
		r = -EIO;
		goto unpin;
	}
	snprintf(b->seg, sizeof(b->seg), "amdgpu.%d.%d.%u", p9_getpid(), c->id, b->handle);
	b->va = (unsigned long)kpi_alias_pages(b->seg, tt->pages, n, false);
	if (!b->va) {
		pr_warn("amdgpu: ctx %d: bo %u: no alias segment %s\n", c->id, b->handle, b->seg);
		b->seg[0] = 0;
		r = -ENOMEM;
		goto unpin;
	}
mapped:
	b->used = true;
	if (gpu == ~0ULL)	/* the driver's choice: one after the other */
		gpu = chooseaddr(c, b->size);
	else if (gpu && !placeok(c, b, gpu, b->size)) {
		r = -EADDRINUSE;
		goto unseg;
	}
	if (gpu) {
		r = vamap(c, b, gpu, AMDGPU_VA_OP_MAP);
		if (r)
			goto unseg;
	}
	b->gpu = gpu;
	*gpup = gpu;
	*id = b - c->bo + 1;
	strscpy(seg, b->seg, nseg);
	*va = b->va;
	*rsize = b->size;
	mutex_unlock(&c->lock);
	return 0;
unseg:
	b->used = false;
	if (!b->vram)
		p9_segfree(b->seg, NULL);
unpin:
	unpin(b->abo);
	b->abo = NULL;
	goto close;
put:
	drm_gem_object_put(&b->abo->tbo.base);
	b->abo = NULL;
close:
	{
		struct drm_gem_close cl = { .handle = b->handle };

		ioctl(c, DRM_IOCTL_GEM_CLOSE, &cl);
	}
	memset(b, 0, sizeof(*b));
out:
	mutex_unlock(&c->lock);
	return r;
}

int
amdgpu_bo_bind(void *v, int id, u64 gpu)
{
	struct gctx *c = v;
	struct bo *b;
	int r = 0;

	mutex_lock(&c->lock);
	b = findbo(c, id);
	if (!b)
		r = -ENOENT;
	else if (gpu && !placeok(c, b, gpu, b->size))
		r = -EADDRINUSE;
	else {
		/* amdgpu keeps the old mapping until the work before is done */
		if (b->gpu)
			r = vamap(c, b, b->gpu, AMDGPU_VA_OP_UNMAP);
		b->gpu = 0;
		if (r == 0 && gpu && (r = vamap(c, b, gpu, AMDGPU_VA_OP_MAP)) == 0)
			b->gpu = gpu;
	}
	mutex_unlock(&c->lock);
	return r;
}

int
amdgpu_bo_free(void *v, int id)
{
	struct gctx *c = v;
	struct bo *b;
	int r = 0;

	mutex_lock(&c->lock);
	b = findbo(c, id);
	if (!b)
		r = -ENOENT;
	else
		bofree(c, b);	/* amdgpu keeps it while the GPU uses it */
	mutex_unlock(&c->lock);
	return r;
}

int
amdgpu_scanout(void *v, int id, unsigned int w, unsigned int h, unsigned int stride)
{
	struct gctx *c = v;
	struct bo *b;
	u32 handle;

	mutex_lock(&c->lock);
	if (id == 0)
		handle = 0;
	else if ((b = findbo(c, id)) != NULL && b->vram)
		handle = b->handle;
	else {
		mutex_unlock(&c->lock);
		return b ? -EINVAL : -ENOENT;	/* the display scans out VRAM only */
	}
	mutex_unlock(&c->lock);
	/* returns when the display shows it: a swapchain's FIFO */
	return kms_scanout(c->filp, handle, w, h, stride);
}

/* a free entry of the object table */
static struct bo *
newbo(struct gctx *c)
{
	struct bo *nb;
	int i;

	for (i = 0; i < c->nbo; i++)
		if (!c->bo[i].used)
			return &c->bo[i];
	nb = krealloc_array(c->bo, c->nbo + 64, sizeof(*nb), GFP_KERNEL);
	if (!nb)
		return NULL;
	memset(nb + c->nbo, 0, 64 * sizeof(*nb));
	c->bo = nb;
	c->nbo += 64;
	return &c->bo[i];
}

/* one IB: ndw dwords at offset in object id, on ip (AMDGPU_HW_IP_*); fence k */
int
amdgpu_submit(void *v, int id, u32 offset, u32 ndw, int ip, u64 *k)
{
	struct gctx *c = v;
	struct drm_amdgpu_cs_chunk_ib ib = { };
	struct drm_amdgpu_cs_chunk_sem out = { };
	struct drm_amdgpu_bo_list_in list = { };
	struct drm_amdgpu_bo_list_entry *ent = NULL;
	struct drm_amdgpu_cs_chunk chunk[3];
	u64 chunkp[3];
	union drm_amdgpu_cs cs = { };
	struct dma_fence *f;
	struct bo *b;
	int i, n, r;

	if (ip != AMDGPU_HW_IP_GFX && ip != AMDGPU_HW_IP_COMPUTE && ip != AMDGPU_HW_IP_DMA)
		return -EINVAL;
	mutex_lock(&c->lock);
	b = findbo(c, id);
	if (!b || !b->gpu) {
		r = b ? -EINVAL : -ENOENT;
		goto out;
	}
	if (ndw == 0 || offset & 3 || (u64)offset + (u64)ndw * 4 > b->size) {
		r = -EINVAL;
		goto out;
	}
	if (c->nfence >= Nfence && c->fence[(c->nfence + 1) % Nfence] &&
	    !dma_fence_is_signaled(c->fence[(c->nfence + 1) % Nfence])) {
		r = -EBUSY;	/* gpu(4): at most 64 outstanding */
		goto out;
	}
	ib.va_start = b->gpu + offset;
	ib.ib_bytes = ndw * 4;
	ib.ip_type = ip;
	out.handle = c->syncobj;
	chunk[0].chunk_id = AMDGPU_CHUNK_ID_IB;
	chunk[0].length_dw = sizeof(ib) / 4;
	chunk[0].chunk_data = (u64)(uintptr_t)&ib;
	chunk[1].chunk_id = AMDGPU_CHUNK_ID_SYNCOBJ_OUT;
	chunk[1].length_dw = sizeof(out) / 4;
	chunk[1].chunk_data = (u64)(uintptr_t)&out;
	chunkp[0] = (u64)(uintptr_t)&chunk[0];
	chunkp[1] = (u64)(uintptr_t)&chunk[1];
	cs.in.ctx_id = c->ctxid;
	cs.in.num_chunks = 2;
	cs.in.chunks = (u64)(uintptr_t)chunkp;
	/* the objects not always valid in the VM (VRAM ones), as Mesa lists them */
	for (i = n = 0; i < c->nbo; i++)
		if (c->bo[i].used && c->bo[i].vram && c->bo[i].gpu)
			n++;
	if (n) {
		ent = kcalloc(n, sizeof(*ent), GFP_KERNEL);
		if (!ent) {
			r = -ENOMEM;
			goto out;
		}
		for (i = n = 0; i < c->nbo; i++)
			if (c->bo[i].used && c->bo[i].vram && c->bo[i].gpu)
				ent[n++].bo_handle = c->bo[i].handle;
		list.bo_number = n;
		list.bo_info_size = sizeof(*ent);
		list.bo_info_ptr = (u64)(uintptr_t)ent;
		chunk[2].chunk_id = AMDGPU_CHUNK_ID_BO_HANDLES;
		chunk[2].length_dw = sizeof(list) / 4;
		chunk[2].chunk_data = (u64)(uintptr_t)&list;
		chunkp[2] = (u64)(uintptr_t)&chunk[2];
		cs.in.num_chunks = 3;
	}
	r = ioctl(c, DRM_IOCTL_AMDGPU_CS, &cs);
	kfree(ent);
	if (r)
		goto out;
	/* the syncobj holds the submission's fence now */
	r = drm_syncobj_find_fence(c->file, c->syncobj, 0, 0, &f);
	if (r)
		goto out;
	*k = ++c->nfence;
	dma_fence_put(c->fence[*k % Nfence]);
	c->fence[*k % Nfence] = f;
out:
	mutex_unlock(&c->lock);
	return r;
}

static struct dma_fence *
getfence(struct gctx *c, u64 k)
{
	struct dma_fence *f = NULL;

	mutex_lock(&c->lock);
	if (k >= 1 && k <= c->nfence && k + Nfence > c->nfence)
		f = dma_fence_get(c->fence[k % Nfence]);
	mutex_unlock(&c->lock);
	return f;
}

/* fence k: 1 done, 0 not (query, or not in 5 seconds), <0 no such fence */
int
amdgpu_fence_wait(void *v, u64 k, int query)
{
	struct gctx *c = v;
	struct dma_fence *f;
	long t;
	int done;

	if (k == 0 || k > c->nfence)
		return -ENOENT;
	f = getfence(c, k);
	if (!f)
		return 1;	/* old: long done */
	if (!query) {
		t = dma_fence_wait_timeout(f, true, 5 * HZ);
		if (t == 0) {
			c->failed = true;	/* gpu(4): given up after 5 s */
			pr_warn("amdgpu: ctx %d fence %llu: not done in 5 s\n", c->id, k);
		}
	}
	done = dma_fence_is_signaled(f);
	if (done && f->error < 0 && f->error != -ECANCELED)
		c->failed = true;
	dma_fence_put(f);
	return done;
}

/* fences done in order, those submitted, failed */
void
amdgpu_fence_count(void *v, u64 *done, u64 *n, int *failed)
{
	struct gctx *c = v;
	u64 k;

	mutex_lock(&c->lock);
	for (k = c->nfence > Nfence ? c->nfence - Nfence : 0; k < c->nfence; k++)
		if (c->fence[(k + 1) % Nfence] && !dma_fence_is_signaled(c->fence[(k + 1) % Nfence]))
			break;
	*done = k;
	*n = c->nfence;
	*failed = c->failed;
	mutex_unlock(&c->lock);
}
