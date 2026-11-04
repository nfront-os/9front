// SPDX-License-Identifier: MIT
/*
 * aux/i915: gpu(4)'s contexts on Linux's i915, the Linux side.  A
 * context is an open DRM file of the render node (drm_open_helper), as a
 * client's descriptor is on Linux: its own GEM handles, address space
 * (i915's default context, a full PPGTT) and fences.  The requests of
 * gpu(4) become i915's ioctls, called through drm_ioctl as the kernel
 * does for a process:
 *
 *	bo size [gpuaddr]	GEM_CREATE (no gpuaddr: the driver chooses
 *				one; 0: not bound); the pages, pinned, become
 *				an alias segment (segment(3)) the client
 *				attaches
 *	bind id gpuaddr		where the object is softpinned from now on
 *	submit id offset eng	EXECBUFFER2 of every bound object, pinned
 *				(EXEC_OBJECT_PINNED, no relocations), the
 *				batch last, with an out fence: fence k
 *	free id			GEM_CLOSE, after the object's segment is revoked
 *				in every process (segment(3))
 */
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/sync_file.h>
#include <linux/dma-fence.h>
#include <linux/mutex.h>
#include <linux/pci.h>
#include <linux/vmalloc.h>
#include <drm/drm_drv.h>
#include <drm/drm_file.h>
#include <drm/drm_ioctl.h>
#include <drm/i915_drm.h>
#include "i915_drv.h"
#include "gem/i915_gem_object.h"
#include "i915_scatterlist.h"
#include "gt/intel_gt.h"
#include "gt/intel_gt_mcr.h"
#include "gt/intel_engine.h"
#include "gt/intel_rc6.h"
#include "gt/intel_rps.h"
#include "drivers/gpu/drm/drm_internal.h"
#include <kpi/p9.h>
#include "../i915.h"
#include "../../kmsout.h"

struct pci_dev	*kpi_pci_gpu(const char *gdir);
int	kms_plane(struct file *to, u32 *handle, u32 *w, u32 *h, u32 *stride, u32 *bpp);

enum { Nfence = 64 };

struct bo {
	bool	used;
	u32	handle;
	struct drm_i915_gem_object	*obj;
	u64	gpu;		/* softpinned there, 0: not bound */
	u64	size;
	char	seg[32];	/* "": none (the firmware's plane: stolen memory) */
	unsigned long	va;
};

struct gctx {
	int	id;
	struct mutex	lock;
	struct file	*filp;
	struct drm_file	*file;
	struct bo	*bo;
	int	nbo;
	u64	nfence;
	u64	next;		/* the next address the driver chooses */
	struct dma_fence	*fence[Nfence];	/* fence k at k % Nfence */
	bool	failed;
};

/* Linux 7.3: drm_open_helper wants the flag DRM's own GEM fops have */
static const struct file_operations ctx_fops = { .fop_flags = FOP_UNSIGNED_OFFSET };
static struct bo	*newbo(struct gctx *c);
static u64	chooseaddr(struct gctx *c, u64 size);
static bool	placeok(struct gctx *c, struct bo *self, u64 gpu, u64 size);
static atomic_t nctx;

static struct drm_device *
drmdev(void)
{
	struct pci_dev *p = kpi_pci_gpu(NULL);
	struct drm_i915_private *i915 = p ? pci_get_drvdata(p) : NULL;

	return i915 ? &i915->drm : NULL;
}

static int
ioctl(struct gctx *c, unsigned int cmd, void *arg)
{
	return drm_ioctl(c->filp, cmd, (unsigned long)arg);
}

void *
i915_ctx_new(int *id)
{
	struct drm_device *dev = drmdev();
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
	c->id = *id = atomic_inc_return(&nctx);
	return c;
}

static struct bo *
findbo(struct gctx *c, int id)
{
	if (id < 1 || id > c->nbo || !c->bo[id - 1].used)
		return NULL;
	return &c->bo[id - 1];
}

static void
bofree(struct gctx *c, struct bo *b)
{
	struct drm_gem_close cl = { .handle = b->handle };

	/*
	 * removing the alias segment revokes it (segment(3)): a client that
	 * still has it attached loses the pages before i915 gets them back
	 */
	if (b->seg[0]) {
		p9_segfree(b->seg, NULL);
		i915_gem_object_unpin_pages(b->obj);
		i915_gem_object_put(b->obj);
	}
	ioctl(c, DRM_IOCTL_GEM_CLOSE, &cl);
	memset(b, 0, sizeof(*b));
}

void
i915_ctx_free(void *v)
{
	struct gctx *c = v;
	struct drm_device *dev = c->file->minor->dev;
	int i;

	kmsout_scanout(c->filp, 0, 0, 0, 0);	/* its picture off the display */
	for (i = 0; i < Nfence; i++)
		if (c->fence[i]) {
			dma_fence_wait_timeout(c->fence[i], false, 5 * HZ);
			dma_fence_put(c->fence[i]);
		}
	for (i = 0; i < c->nbo; i++)
		if (c->bo[i].used)
			bofree(c, &c->bo[i]);
	kfree(c->bo);
	/* drm_close_helper */
	mutex_lock(&dev->filelist_mutex);
	list_del(&c->file->lhead);
	mutex_unlock(&dev->filelist_mutex);
	drm_file_free(c->file);
	c->filp->private_data = NULL;
	fput(c->filp);
	kfree(c);
}

/* a new object; its segment's name, address and size for the client */
int
i915_bo_new(void *v, u64 size, u64 *gpup, int *id, char *seg, int nseg, u64 *va, u64 *rsize)
{
	struct gctx *c = v;
	struct drm_i915_gem_create cr = { .size = size };
	struct sgt_iter it;
	struct page **pages, *p;
	struct bo *b;
	unsigned int n, i;
	u64 gpu = *gpup;
	int r;

	mutex_lock(&c->lock);
	b = newbo(c);
	if (!b) {
		r = -ENOMEM;
		goto out;
	}
	r = ioctl(c, DRM_IOCTL_I915_GEM_CREATE, &cr);
	if (r)
		goto out;
	b->handle = cr.handle;
	b->size = cr.size;
	b->obj = i915_gem_object_lookup(c->file, cr.handle);
	if (!b->obj) {
		r = -ENOENT;
		goto close;
	}
	/* its pages, pinned while the client may have them */
	r = i915_gem_object_pin_pages_unlocked(b->obj);
	if (r)
		goto put;
	n = b->size >> PAGE_SHIFT;
	pages = kvmalloc_array(n, sizeof(*pages), GFP_KERNEL);
	if (!pages) {
		r = -ENOMEM;
		goto unpin;
	}
	i = 0;
	for_each_sgt_page(p, it, b->obj->mm.pages)
		if (i < n)
			pages[i++] = p;
	snprintf(b->seg, sizeof(b->seg), "i915.%d.%d.%u", p9_getpid(), c->id, b->handle);
	b->va = (unsigned long)kpi_alias_pages(b->seg, pages, i, false);
	kvfree(pages);
	if (!b->va || i != n) {
		r = -ENOMEM;
		goto unpin;
	}
	b->used = true;
	if (gpu == ~0ULL)	/* the driver's choice: one after the other */
		gpu = chooseaddr(c, b->size);
	else if (gpu && !placeok(c, b, gpu, b->size)) {
		b->used = false;
		p9_segfree(b->seg, NULL);
		r = -EADDRINUSE;
		goto unpin;
	}
	b->gpu = gpu;
	*gpup = gpu;
	*id = b - c->bo + 1;
	strscpy(seg, b->seg, nseg);
	*va = b->va;
	*rsize = b->size;
	mutex_unlock(&c->lock);
	return 0;
unpin:
	i915_gem_object_unpin_pages(b->obj);
put:
	i915_gem_object_put(b->obj);
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
i915_bo_bind(void *v, int id, u64 gpu)
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
	else
		b->gpu = gpu;
	mutex_unlock(&c->lock);
	return r;
}

int
i915_bo_free(void *v, int id)
{
	struct gctx *c = v;
	struct bo *b;
	int r = 0;

	mutex_lock(&c->lock);
	b = findbo(c, id);
	if (!b)
		r = -ENOENT;
	else
		bofree(c, b);	/* i915 keeps it while the GPU uses it */
	mutex_unlock(&c->lock);
	return r;
}

/* i915's GETPARAM, as the context's file asks it */
int
i915_getparam(void *v, int param, int *value)
{
	struct gctx *c = v;
	struct drm_i915_getparam g = { .param = param, .value = value };

	return ioctl(c, DRM_IOCTL_I915_GETPARAM, &g);
}

/*
 * a place for an object of size at gpu in the context's address space:
 * inside its 48 bits and apart from the other bound objects (but self)
 */
static bool
placeok(struct gctx *c, struct bo *self, u64 gpu, u64 size)
{
	int i;

	if (gpu & (PAGE_SIZE - 1) || gpu + size > (1ULL << 48) || gpu + size < gpu)
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

static u64
chooseaddr(struct gctx *c, u64 size)
{
	u64 a;

	if (!c->next)
		c->next = 1ULL << 20;
	a = c->next;
	c->next = ALIGN(a + size, 1ULL << 16);
	return a;
}

int
i915_scanout(void *v, int id, unsigned int w, unsigned int h, unsigned int stride)
{
	struct gctx *c = v;
	struct bo *b;
	u32 handle;

	mutex_lock(&c->lock);
	if (id == 0)
		handle = 0;
	else if ((b = findbo(c, id)) != NULL)
		handle = b->handle;
	else {
		mutex_unlock(&c->lock);
		return -ENOENT;
	}
	mutex_unlock(&c->lock);
	/* returns when the display shows it: a swapchain's FIFO */
	return kmsout_scanout(c->filp, handle, w, h, stride);
}

/* the firmware's plane in the context: its object, address, geometry */
int
i915_map_plane(void *v, int *id, u64 *gpu, unsigned int *w, unsigned int *h, unsigned int *stride, unsigned int *bpp)
{
	struct gctx *c = v;
	struct bo *b;
	u32 handle;
	int r;

	r = kms_plane(c->filp, &handle, w, h, stride, bpp);
	if (r)
		return r;
	mutex_lock(&c->lock);
	b = newbo(c);
	if (!b) {
		struct drm_gem_close cl = { .handle = handle };

		ioctl(c, DRM_IOCTL_GEM_CLOSE, &cl);
		mutex_unlock(&c->lock);
		return -ENOMEM;
	}
	b->used = true;
	b->handle = handle;
	b->size = ALIGN((u64)*stride * *h, PAGE_SIZE);
	b->gpu = chooseaddr(c, b->size);
	*id = b - c->bo + 1;
	*gpu = b->gpu;
	mutex_unlock(&c->lock);
	return 0;
}

/* gen 8's virtual addresses are 48 bits, in canonical form for i915 */
static u64 canonical(u64 a) { return (u64)((s64)(a << 16) >> 16); }

int
i915_submit(void *v, int id, u32 offset, int engine, u64 *k)
{
	struct gctx *c = v;
	struct drm_i915_gem_exec_object2 *obj;
	struct drm_i915_gem_execbuffer2 eb = { };
	struct dma_fence *f;
	struct bo *batch;
	int i, n, r, fd;

	mutex_lock(&c->lock);
	batch = findbo(c, id);
	if (!batch || !batch->gpu) {
		r = batch ? -EINVAL : -ENOENT;
		goto out;
	}
	if (c->nfence >= Nfence && c->fence[(c->nfence + 1) % Nfence] &&
	    !dma_fence_is_signaled(c->fence[(c->nfence + 1) % Nfence])) {
		r = -EBUSY;	/* gpu(4): at most 64 outstanding */
		goto out;
	}
	obj = kvmalloc_array(c->nbo, sizeof(*obj), GFP_KERNEL | __GFP_ZERO);
	if (!obj) {
		r = -ENOMEM;
		goto out;
	}
	n = 0;
	for (i = 0; i < c->nbo; i++)
		if (c->bo[i].used && c->bo[i].gpu && &c->bo[i] != batch) {
			obj[n].handle = c->bo[i].handle;
			obj[n].offset = canonical(c->bo[i].gpu);
			obj[n].flags = EXEC_OBJECT_PINNED | EXEC_OBJECT_SUPPORTS_48B_ADDRESS | EXEC_OBJECT_WRITE;
			n++;
		}
	obj[n].handle = batch->handle;
	obj[n].offset = canonical(batch->gpu);
	obj[n].flags = EXEC_OBJECT_PINNED | EXEC_OBJECT_SUPPORTS_48B_ADDRESS;
	n++;
	eb.buffers_ptr = (u64)(uintptr_t)obj;
	eb.buffer_count = n;
	eb.batch_start_offset = offset;
	eb.flags = (engine == 1 ? I915_EXEC_BLT : I915_EXEC_RENDER) | I915_EXEC_NO_RELOC | I915_EXEC_FENCE_OUT;
	r = ioctl(c, DRM_IOCTL_I915_GEM_EXECBUFFER2_WR, &eb);
	kvfree(obj);
	if (r)
		goto out;
	fd = eb.rsvd2 >> 32;
	f = sync_file_get_fence(fd);
	close_fd(fd);
	if (!f) {
		r = -EIO;
		goto out;
	}
	*k = ++c->nfence;
	dma_fence_put(c->fence[*k % Nfence]);
	c->fence[*k % Nfence] = f;
	r = 0;
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
i915_fence_wait(void *v, u64 k, int query)
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
			pr_warn("i915: ctx %d fence %llu: not done in 5 s\n", c->id, k);
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
i915_fence_count(void *v, u64 *done, u64 *n, int *failed)
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

/*
 * the render engine's context workarounds (i915's ctx_wa_list, written
 * into every context): a line each, "rcs wa reg mask value", for a
 * client to read them in its own context (inteltest: the registers are
 * the context's, read through MMIO an idle engine shows another's);
 * kmalloc'd
 */
static char *
rcs_workarounds(struct intel_gt *gt)
{
	struct intel_engine_cs *e = gt->engine[RCS0];
	const struct i915_wa *wa;
	char *s, *t;
	unsigned int i;

	s = kstrdup("", GFP_KERNEL);
	if (!e || !s)
		return s;
	for (i = 0; i < e->ctx_wa_list.count; i++) {
		wa = &e->ctx_wa_list.list[i];
		t = kasprintf(GFP_KERNEL, "%srcs wa %x %x %x\n", s, i915_mmio_reg_offset(wa->reg),
			wa->read, wa->set & wa->read);
		kfree(s);
		if (!(s = t))
			break;
	}
	return s;
}

/* /dev/gpu/0/status, kmalloc'd (i915_freestr) */
char *
i915_status(void)
{
	struct drm_device *dev = drmdev();
	struct drm_i915_private *i915;
	struct pci_dev *p;
	const struct sseu_dev_info *s;
	struct intel_gt *gt;
	struct intel_rps *rps;
	intel_wakeref_t wf;
	char *wa, *r;
	u64 rc6;
	u32 act;

	if (!dev)
		return kstrdup("device none\n", GFP_KERNEL);
	i915 = to_i915(dev);
	p = to_pci_dev(dev->dev);
	gt = to_gt(i915);
	rps = &gt->rps;
	s = &gt->info.sseu;
	wa = rcs_workarounds(gt);
	rc6 = 0;
	act = 0;
	with_intel_runtime_pm(gt->uncore->rpm, wf) {
		rc6 = intel_rc6_residency_ns(&gt->rc6, INTEL_RC6_RES_RC6);
		act = intel_rps_read_actual_frequency(rps);
	}
	/* the lines as aux/intelgpu has them (inteltest reads them) */
	r = kasprintf(GFP_KERNEL,
		"device intel %04x/%04x, gen %d (%s, Linux 7.3's i915 on the LinuxKPI)\n"
		"pci %u.%u.%u rev %#x subsys %04x/%04x aperture %llu\n"	/* for every client (libdrm9): #G's ctl and #$/pci are the hostowner's */
		"%s"
		"sseu slices %#x subslices %#x eus %d\n"
		"power rc6 %s %llu ms, %u MHz (asked %d, min %d, rp1 %d, max %d)\n",
		p->vendor, p->device, GRAPHICS_VER(i915), intel_platform_name(INTEL_INFO(i915)->platform),
		p->bus->number, PCI_SLOT(p->devfn), PCI_FUNC(p->devfn), p->revision,
		p->subsystem_vendor, p->subsystem_device, (unsigned long long)pci_resource_len(p, 2),
		wa ? wa : "",
		s->slice_mask, s->subslice_mask.hsw[0], s->eu_total,
		gt->rc6.enabled ? "on" : "off", (unsigned long long)(rc6 / NSEC_PER_MSEC), act,
		intel_gpu_freq(rps, rps->cur_freq), intel_gpu_freq(rps, rps->min_freq_softlimit),
		intel_gpu_freq(rps, rps->rp1_freq), intel_gpu_freq(rps, rps->max_freq_softlimit));
	kfree(wa);
	return r;
}

void i915_freestr(char *s) { kfree(s); }
