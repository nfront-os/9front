/*
 * Copyright 2026 The 9front gpu port
 * SPDX-License-Identifier: MIT
 *
 * venus renderer on 9front: the virtio-gpu file system of gpu(4).
 *
 *   /dev/gpu/0/clone             a context (its ctl file)
 *   ctx/N/ctl                    init, blob, map, unref
 *   ctx/N/submit                 "offset length ring" in the command segment
 *   ctx/N/fence                  wait for or query the k-th submission
 *
 * Command streams are copied into the context's command segment (a
 * fixed #g segment the driver made) and submitted from there.  Host
 * blobs are mapped through the device's host visible bar, attached
 * once.  Syncs are timelines kept here: a submission's syncs get their
 * values once its fence is done.
 */

#include <fcntl.h>
#include <unistd.h>

#include "util/os_time.h"
#include "util/sparse_array.h"
#include "util/u_process.h"

#include "virtio-gpu/venus_hw.h"

#include "vn_renderer_internal.h"

/* APE's lib9 (lib9.h would bring Plan 9's macros into Mesa) */
extern void *segattach(int, char *, void *, unsigned long);
/* plan9/lib/dmabuf.c: gpu(4)'s export tokens as dma-buf fds */
extern int plan9_dmabuf_fd(const char *token);
extern int plan9_dmabuf_token(int fd, char *buf, int n);
extern int segdetach(void *);

#define PLAN9_GPU_DIR "/dev/gpu/0"
#define PLAN9_PCI_VENDOR_ID 0x1af4
#define PLAN9_PCI_DEVICE_ID 0x1050
#define PLAN9_CAPSET_VENUS 4
#define PLAN9_BLOB_HOST3D 2
#define PLAN9_BLOB_FLAG_MAPPABLE 1
#define PLAN9_BLOB_FLAG_SHAREABLE 2
#define PLAN9_MAX_RINGS 64

struct plan9_shmem {
   struct vn_renderer_shmem base;
};

struct plan9_bo {
   struct vn_renderer_bo base;
   uint32_t blob_flags;
};

struct plan9_pending {
   uint64_t fence;
   uint64_t value;
};

struct plan9_sync {
   struct vn_renderer_sync base;

   uint64_t value;
   /* values to take once their submissions are done */
   struct plan9_pending *pending;
   uint32_t pending_count;
   uint32_t pending_max;
};

struct plan9 {
   struct vn_renderer base;

   struct vn_instance *instance;

   mtx_t mutex;
   int ctl_fd;
   int submit_fd;
   int fence_fd;
   int ctx_id;

   /* command segment */
   uint8_t *cmd;
   size_t cmd_size;
   size_t cmd_pos;
   uint64_t cmd_fence; /* last submission using the segment since pos 0 */

   uint64_t fence;     /* last submission */
   uint64_t done;      /* done, with all before it */

   /*
    * the host visible bar, through the segments the driver names for
    * the blobs: one for each way the host maps them (cached,
    * write-combining, uncached; gpu(4) map)
    */
   struct {
      char name[32];
      uint8_t *addr;
   } bar_seg[3];
   uint64_t bar_size;

   struct virgl_renderer_capset_venus capset;

   struct util_sparse_array shmem_array;
   struct util_sparse_array bo_array;
   struct util_sparse_array sync_array;
   uint32_t next_sync_id;

   struct vn_renderer_shmem_cache shmem_cache;
};

/* a ctl request, its reply in buf; called with the mutex held */
static bool
plan9_ctl(struct plan9 *p9, char *buf, size_t size, const char *fmt, ...)
{
   char req[256];
   va_list ap;

   va_start(ap, fmt);
   vsnprintf(req, sizeof(req), fmt, ap);
   va_end(ap);
   if (write(p9->ctl_fd, req, strlen(req)) < 0) {
      vn_log(p9->instance, "plan9: %s: %s", req, strerror(errno));
      return false;
   }
   if (!buf)
      return true;
   const ssize_t n = pread(p9->ctl_fd, buf, size - 1, 0);
   if (n <= 0)
      return false;
   buf[n] = '\0';
   return true;
}

static int
plan9_fields(char *s, char **f, int max)
{
   int n = 0;
   char *save;

   for (char *t = strtok_r(s, " \t\n", &save); t && n < max;
        t = strtok_r(NULL, " \t\n", &save))
      f[n++] = t;
   return n;
}

/*
 * the state of fence k: 1 done, 0 busy, -1 failed.  wait blocks until
 * it is done.  called with the mutex held.
 */
static int
plan9_fence(struct plan9 *p9, uint64_t k, bool wait)
{
   char buf[64];

   if (k == 0 || k <= p9->done)
      return 1;
   snprintf(buf, sizeof(buf), "%s %" PRIu64, wait ? "wait" : "query", k);
   if (write(p9->fence_fd, buf, strlen(buf)) < 0)
      return -1;
   const ssize_t n = pread(p9->fence_fd, buf, sizeof(buf) - 1, 0);
   if (n <= 0)
      return -1;
   buf[n] = '\0';
   if (strstr(buf, "failed"))
      return -1;
   return strstr(buf, "done") != NULL;
}

/* block until fence k is done, without the mutex */
static int
plan9_fence_wait(struct plan9 *p9, uint64_t k)
{
   char buf[64], path[64];

   snprintf(path, sizeof(path), PLAN9_GPU_DIR "/ctx/%d/fence", p9->ctx_id);
   const int fd = open(path, O_RDWR);
   if (fd < 0)
      return -1;
   snprintf(buf, sizeof(buf), "wait %" PRIu64, k);
   ssize_t n = write(fd, buf, strlen(buf));
   if (n >= 0)
      n = pread(fd, buf, sizeof(buf) - 1, 0);
   close(fd);
   if (n <= 0)
      return -1;
   buf[n] = '\0';
   return strstr(buf, "failed") ? -1 : 1;
}

/* move the values of done submissions into the sync */
static void
plan9_sync_update(struct plan9 *p9, struct plan9_sync *sync)
{
   uint32_t i = 0;

   while (i < sync->pending_count) {
      struct plan9_pending *pend = &sync->pending[i];
      if (plan9_fence(p9, pend->fence, false) != 0) {
         if (pend->value > sync->value)
            sync->value = pend->value;
         *pend = sync->pending[--sync->pending_count];
      } else {
         i++;
      }
   }
}

static VkResult
plan9_sync_write(struct vn_renderer *renderer,
                 struct vn_renderer_sync *_sync,
                 uint64_t val)
{
   struct plan9 *p9 = (struct plan9 *)renderer;
   struct plan9_sync *sync = (struct plan9_sync *)_sync;

   mtx_lock(&p9->mutex);
   sync->value = val;
   mtx_unlock(&p9->mutex);

   return VK_SUCCESS;
}

static VkResult
plan9_sync_read(struct vn_renderer *renderer,
                struct vn_renderer_sync *_sync,
                uint64_t *val)
{
   struct plan9 *p9 = (struct plan9 *)renderer;
   struct plan9_sync *sync = (struct plan9_sync *)_sync;

   mtx_lock(&p9->mutex);
   plan9_sync_update(p9, sync);
   *val = sync->value;
   mtx_unlock(&p9->mutex);

   return VK_SUCCESS;
}

static VkResult
plan9_sync_reset(struct vn_renderer *renderer,
                 struct vn_renderer_sync *_sync)
{
   struct plan9 *p9 = (struct plan9 *)renderer;
   struct plan9_sync *sync = (struct plan9_sync *)_sync;

   mtx_lock(&p9->mutex);
   sync->value = 0;
   sync->pending_count = 0;
   mtx_unlock(&p9->mutex);

   return VK_SUCCESS;
}

static void
plan9_sync_destroy(struct vn_renderer *renderer,
                   struct vn_renderer_sync *_sync)
{
   struct plan9_sync *sync = (struct plan9_sync *)_sync;

   free(sync->pending);
   sync->pending = NULL;
   sync->pending_count = 0;
   sync->pending_max = 0;
}

static VkResult
plan9_sync_create(struct vn_renderer *renderer,
                  uint64_t initial_val,
                  struct vn_renderer_sync **out_sync)
{
   struct plan9 *p9 = (struct plan9 *)renderer;

   mtx_lock(&p9->mutex);
   const uint32_t sync_id = ++p9->next_sync_id;
   mtx_unlock(&p9->mutex);

   struct plan9_sync *sync = util_sparse_array_get(&p9->sync_array, sync_id);
   *sync = (struct plan9_sync){
      .base.sync_id = sync_id,
      .value = initial_val,
   };
   *out_sync = &sync->base;

   return VK_SUCCESS;
}

static bool
plan9_sync_add_pending(struct plan9_sync *sync, uint64_t fence, uint64_t val)
{
   if (sync->pending_count == sync->pending_max) {
      const uint32_t max = sync->pending_max ? sync->pending_max * 2 : 4;
      struct plan9_pending *pend =
         realloc(sync->pending, max * sizeof(*pend));
      if (!pend)
         return false;
      sync->pending = pend;
      sync->pending_max = max;
   }
   sync->pending[sync->pending_count++] =
      (struct plan9_pending){ .fence = fence, .value = val };
   return true;
}

/* submit a batch; called with the mutex held */
static VkResult
plan9_submit_locked(struct plan9 *p9,
                    const struct vn_renderer_submit_batch *batch)
{
   char buf[64];
   size_t size = align64(batch->cs_size, 4);

   if (size > p9->cmd_size) {
      vn_log(p9->instance, "plan9: command stream of %zu bytes too big",
             batch->cs_size);
      return VK_ERROR_OUT_OF_DEVICE_MEMORY;
   }
   /* wrap around: the device must be done with the start of the segment */
   if (p9->cmd_pos + size > p9->cmd_size) {
      if (plan9_fence(p9, p9->fence, true) < 0)
         return VK_ERROR_DEVICE_LOST;
      p9->cmd_pos = 0;
   }
   const size_t off = p9->cmd_pos;
   if (batch->cs_size)
      memcpy(p9->cmd + off, batch->cs_data, batch->cs_size);
   if (size > batch->cs_size)
      memset(p9->cmd + off + batch->cs_size, 0, size - batch->cs_size);
   p9->cmd_pos += size;

   snprintf(buf, sizeof(buf), "%zu %zu %u", off, size, batch->ring_idx);
   if (write(p9->submit_fd, buf, strlen(buf)) < 0) {
      vn_log(p9->instance, "plan9: submit: %s", strerror(errno));
      return VK_ERROR_DEVICE_LOST;
   }
   const uint64_t fence = ++p9->fence;

   for (uint32_t i = 0; i < batch->sync_count; i++) {
      struct plan9_sync *sync = (struct plan9_sync *)batch->syncs[i];
      if (!plan9_sync_add_pending(sync, fence, batch->sync_values[i]))
         return VK_ERROR_OUT_OF_HOST_MEMORY;
   }
   return VK_SUCCESS;
}

static VkResult
plan9_submit(struct vn_renderer *renderer,
             const struct vn_renderer_submit_batch *batch)
{
   struct plan9 *p9 = (struct plan9 *)renderer;

   mtx_lock(&p9->mutex);
   const VkResult result = plan9_submit_locked(p9, batch);
   mtx_unlock(&p9->mutex);

   return result;
}

/* called with the mutex held */
static bool
plan9_wait_ready(struct plan9 *p9,
                 const struct vn_renderer_wait *wait,
                 uint64_t *block_on)
{
   uint32_t ready = 0;

   *block_on = 0;
   for (uint32_t i = 0; i < wait->sync_count; i++) {
      struct plan9_sync *sync = (struct plan9_sync *)wait->syncs[i];
      plan9_sync_update(p9, sync);
      if (sync->value >= wait->sync_values[i]) {
         ready++;
         continue;
      }
      /* a fence that brings the sync there, to block on */
      for (uint32_t j = 0; j < sync->pending_count; j++) {
         if (sync->pending[j].value >= wait->sync_values[i] &&
             (!*block_on || sync->pending[j].fence < *block_on))
            *block_on = sync->pending[j].fence;
      }
   }
   return wait->wait_any ? ready > 0 : ready == wait->sync_count;
}

static VkResult
plan9_wait(struct vn_renderer *renderer, const struct vn_renderer_wait *wait)
{
   struct plan9 *p9 = (struct plan9 *)renderer;
   const int64_t end = os_time_get_absolute_timeout(wait->timeout);
   uint64_t block_on;

   for (;;) {
      mtx_lock(&p9->mutex);
      const bool ready = plan9_wait_ready(p9, wait, &block_on);
      mtx_unlock(&p9->mutex);
      /*
       * without a timeout, block in the driver on a fence that helps,
       * on a fence file of our own: the mutex stays free for the
       * submissions the fence may depend on
       */
      if (!ready && block_on && wait->timeout == OS_TIMEOUT_INFINITE &&
          wait->sync_count == 1) {
         if (plan9_fence_wait(p9, block_on) < 0)
            return VK_ERROR_DEVICE_LOST;
         continue;
      }
      if (ready)
         return VK_SUCCESS;
      if (wait->timeout != OS_TIMEOUT_INFINITE && os_time_get_nano() >= end)
         return VK_TIMEOUT;
      os_time_sleep(100);
   }
}

static void
plan9_bo_invalidate(struct vn_renderer *renderer,
                    struct vn_renderer_bo *bo,
                    VkDeviceSize offset,
                    VkDeviceSize size)
{
   /* nop: the host visible bar is coherent */
}

static void
plan9_bo_flush(struct vn_renderer *renderer,
               struct vn_renderer_bo *bo,
               VkDeviceSize offset,
               VkDeviceSize size)
{
   /* nop */
}

/* the address of resource res_id in the host visible bar; with the mutex */
static void *
plan9_map_locked(struct plan9 *p9, uint32_t res_id, size_t size)
{
   char buf[256], *f[8];

   if (!plan9_ctl(p9, buf, sizeof(buf), "map %u", res_id))
      return NULL;
   /* map res seg barsize offset size info */
   if (plan9_fields(buf, f, 8) != 7 || strcmp(f[0], "map"))
      return NULL;
   const uint64_t off = strtoull(f[4], NULL, 0);
   unsigned i;
   for (i = 0; i < ARRAY_SIZE(p9->bar_seg) && p9->bar_seg[i].addr; i++) {
      if (strcmp(p9->bar_seg[i].name, f[2]) == 0)
         break;
   }
   if (i == ARRAY_SIZE(p9->bar_seg))
      return NULL;
   if (!p9->bar_seg[i].addr) {
      p9->bar_size = strtoull(f[3], NULL, 0);
      void *bar = segattach(0, f[2], NULL, p9->bar_size);
      if (bar == (void *)-1) {
         vn_log(p9->instance, "plan9: segattach %s failed", f[2]);
         return NULL;
      }
      snprintf(p9->bar_seg[i].name, sizeof(p9->bar_seg[i].name), "%s", f[2]);
      p9->bar_seg[i].addr = bar;
   }
   if (off + size > p9->bar_size)
      return NULL;
   return p9->bar_seg[i].addr + off;
}

static void *
plan9_bo_map(struct vn_renderer *renderer,
             struct vn_renderer_bo *_bo,
             void *placed_addr)
{
   struct plan9 *p9 = (struct plan9 *)renderer;
   struct plan9_bo *bo = (struct plan9_bo *)_bo;

   /* no placed maps: the bar is where it is */
   if (placed_addr)
      return NULL;
   if (!bo->base.mmap_ptr && (bo->blob_flags & PLAN9_BLOB_FLAG_MAPPABLE)) {
      mtx_lock(&p9->mutex);
      bo->base.mmap_ptr =
         plan9_map_locked(p9, bo->base.res_id, bo->base.mmap_size);
      mtx_unlock(&p9->mutex);
   }
   return bo->base.mmap_ptr;
}

/*
 * a shareable blob as a dma-buf fd: aux/virtiogpu names it by a token
 * ("export res" -> "export token"), which another context, of any
 * process, imports (plan9_bo_create_from_dma_buf)
 */
static int
plan9_bo_export_dma_buf(struct vn_renderer *renderer,
                        struct vn_renderer_bo *_bo)
{
   struct plan9 *p9 = (struct plan9 *)renderer;
   struct plan9_bo *bo = (struct plan9_bo *)_bo;
   char buf[128], *f[4];
   int fd = -1;

   if (!(bo->blob_flags & PLAN9_BLOB_FLAG_SHAREABLE))
      return -1;
   mtx_lock(&p9->mutex);
   if (plan9_ctl(p9, buf, sizeof(buf), "export %u", _bo->res_id) &&
       plan9_fields(buf, f, 4) >= 2 && !strcmp(f[0], "export"))
      fd = plan9_dmabuf_fd(f[1]);
   mtx_unlock(&p9->mutex);
   return fd;
}

/*
 * a dma-buf fd's buffer (an export token of aux/virtiogpu) into this
 * context: "import token" -> "res id size"; as virtgpu's: a resource
 * this renderer has already is the same bo, with one more reference
 */
static VkResult
plan9_bo_create_from_dma_buf(struct vn_renderer *renderer,
                             VkDeviceSize size,
                             int fd,
                             VkMemoryPropertyFlags flags,
                             struct vn_renderer_bo **out_bo)
{
   struct plan9 *p9 = (struct plan9 *)renderer;
   char token[64], buf[256], *f[8];
   uint32_t res_id;
   size_t real_size;

   if (plan9_dmabuf_token(fd, token, sizeof(token)) < 0)
      return VK_ERROR_INVALID_EXTERNAL_HANDLE;
   mtx_lock(&p9->mutex);
   if (!plan9_ctl(p9, buf, sizeof(buf), "import %s", token) ||
       plan9_fields(buf, f, 8) < 3 || strcmp(f[0], "res")) {
      mtx_unlock(&p9->mutex);
      return VK_ERROR_INVALID_EXTERNAL_HANDLE;
   }
   res_id = strtoul(f[1], NULL, 0);
   real_size = strtoull(f[2], NULL, 0);

   uint32_t blob_flags = PLAN9_BLOB_FLAG_SHAREABLE;
   size_t mmap_size = 0;
   if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
      /* smaller than asked: not mappable, the app's map fails later */
      if (real_size >= size) {
         blob_flags |= PLAN9_BLOB_FLAG_MAPPABLE;
         mmap_size = real_size;
      }
   }

   struct plan9_bo *bo = util_sparse_array_get(&p9->bo_array, res_id);
   if (bo->base.res_id == res_id) {
      if (bo->base.mmap_size < mmap_size || (blob_flags & ~bo->blob_flags)) {
         mtx_unlock(&p9->mutex);
         return VK_ERROR_INVALID_EXTERNAL_HANDLE;
      }
      vn_refcount_inc(&bo->base.refcount);
   } else {
      *bo = (struct plan9_bo){
         .base = {
            .refcount = VN_REFCOUNT_INIT(1),
            .res_id = res_id,
            .mmap_size = mmap_size,
         },
         .blob_flags = blob_flags,
      };
   }
   mtx_unlock(&p9->mutex);
   *out_bo = &bo->base;
   return VK_SUCCESS;
}

static bool
plan9_bo_destroy(struct vn_renderer *renderer, struct vn_renderer_bo *_bo)
{
   struct plan9 *p9 = (struct plan9 *)renderer;

   mtx_lock(&p9->mutex);
   /* imported again meanwhile (create_from_dma_buf): it stays */
   if (vn_refcount_is_valid(&_bo->refcount)) {
      mtx_unlock(&p9->mutex);
      return false;
   }
   plan9_ctl(p9, NULL, 0, "unref %u", _bo->res_id);
   /* gone: an import of the resource later is a new bo */
   _bo->res_id = 0;
   mtx_unlock(&p9->mutex);

   return true;
}

/* create a host3d blob; with the mutex. returns the resource id or 0 */
static uint32_t
plan9_blob_locked(struct plan9 *p9,
                  uint32_t flags,
                  uint64_t blob_id,
                  size_t size,
                  size_t *out_size)
{
   char buf[256], *f[8];

   if (!plan9_ctl(p9, buf, sizeof(buf), "blob %d %u %" PRIu64 " %zu",
                  PLAN9_BLOB_HOST3D, flags, blob_id, size))
      return 0;
   /* res id size */
   if (plan9_fields(buf, f, 8) < 3 || strcmp(f[0], "res"))
      return 0;
   *out_size = strtoull(f[2], NULL, 0);
   return strtoul(f[1], NULL, 0);
}

static uint32_t
plan9_bo_blob_flags(VkMemoryPropertyFlags flags,
                    VkExternalMemoryHandleTypeFlags external_handles)
{
   uint32_t blob_flags = 0;
   if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
      blob_flags |= PLAN9_BLOB_FLAG_MAPPABLE;
   if (external_handles)
      blob_flags |= PLAN9_BLOB_FLAG_SHAREABLE;
   return blob_flags;
}

static VkResult
plan9_bo_create_from_device_memory(
   struct vn_renderer *renderer,
   struct vn_renderer_submit_batch *batch,
   VkDeviceSize size,
   vn_object_id mem_id,
   VkMemoryPropertyFlags flags,
   VkExternalMemoryHandleTypeFlags external_handles,
   struct vn_renderer_bo **out_bo)
{
   struct plan9 *p9 = (struct plan9 *)renderer;
   const uint32_t blob_flags = plan9_bo_blob_flags(flags, external_handles);
   size_t real_size = 0;

   mtx_lock(&p9->mutex);
   if (batch) {
      const VkResult result = plan9_submit_locked(p9, batch);
      if (result != VK_SUCCESS) {
         mtx_unlock(&p9->mutex);
         return result;
      }
   }
   const uint32_t res_id =
      plan9_blob_locked(p9, blob_flags, mem_id, size, &real_size);
   mtx_unlock(&p9->mutex);
   if (!res_id)
      return VK_ERROR_OUT_OF_DEVICE_MEMORY;

   struct plan9_bo *bo = util_sparse_array_get(&p9->bo_array, res_id);
   *bo = (struct plan9_bo){
      .base = {
         .refcount = VN_REFCOUNT_INIT(1),
         .res_id = res_id,
         .mmap_size = size,
      },
      .blob_flags = blob_flags,
   };
   *out_bo = &bo->base;

   return VK_SUCCESS;
}

static void
plan9_shmem_destroy_now(struct vn_renderer *renderer,
                        struct vn_renderer_shmem *shmem)
{
   struct plan9 *p9 = (struct plan9 *)renderer;

   mtx_lock(&p9->mutex);
   plan9_ctl(p9, NULL, 0, "unref %u", shmem->res_id);
   mtx_unlock(&p9->mutex);
}

static void
plan9_shmem_destroy(struct vn_renderer *renderer,
                    struct vn_renderer_shmem *shmem)
{
   struct plan9 *p9 = (struct plan9 *)renderer;

   if (vn_renderer_shmem_cache_add(&p9->shmem_cache, shmem))
      return;
   plan9_shmem_destroy_now(&p9->base, shmem);
}

static struct vn_renderer_shmem *
plan9_shmem_create(struct vn_renderer *renderer, size_t size)
{
   struct plan9 *p9 = (struct plan9 *)renderer;
   size_t real_size = 0;

   struct vn_renderer_shmem *cached_shmem =
      vn_renderer_shmem_cache_get(&p9->shmem_cache, size);
   if (cached_shmem) {
      cached_shmem->refcount = VN_REFCOUNT_INIT(1);
      return cached_shmem;
   }

   mtx_lock(&p9->mutex);
   /* blob id 0: plain shared memory (capset supports_blob_id_0) */
   const uint32_t res_id =
      plan9_blob_locked(p9, PLAN9_BLOB_FLAG_MAPPABLE, 0, size, &real_size);
   void *ptr = res_id ? plan9_map_locked(p9, res_id, size) : NULL;
   if (res_id && !ptr)
      plan9_ctl(p9, NULL, 0, "unref %u", res_id);
   mtx_unlock(&p9->mutex);
   if (!ptr)
      return NULL;

   struct plan9_shmem *shmem = util_sparse_array_get(&p9->shmem_array, res_id);
   *shmem = (struct plan9_shmem){
      .base = {
         .refcount = VN_REFCOUNT_INIT(1),
         .res_id = res_id,
         .mmap_size = size,
         .mmap_ptr = ptr,
      },
   };

   return &shmem->base;
}

static void
plan9_init_renderer_info(struct plan9 *p9)
{
   struct vn_renderer_info *info = &p9->base.info;

   info->pci.vendor_id = PLAN9_PCI_VENDOR_ID;
   info->pci.device_id = PLAN9_PCI_DEVICE_ID;

   info->has_dma_buf_import = true;	/* export tokens (plan9/lib/dmabuf.c) */
   info->has_external_sync = false;
   info->has_timeline_sync = true;
   info->has_implicit_fencing = false;

   const struct virgl_renderer_capset_venus *capset = &p9->capset;
   info->wire_format_version = capset->wire_format_version;
   info->vk_xml_version = capset->vk_xml_version;
   info->vk_ext_command_serialization_spec_version =
      capset->vk_ext_command_serialization_spec_version;
   info->vk_mesa_venus_protocol_spec_version =
      capset->vk_mesa_venus_protocol_spec_version;

   STATIC_ASSERT(sizeof(info->vk_extension_mask) >=
                 sizeof(capset->vk_extension_mask1));
   memcpy(info->vk_extension_mask, capset->vk_extension_mask1,
          sizeof(capset->vk_extension_mask1));

   /* ring 0 is the context's CPU timeline */
   info->max_timeline_count = PLAN9_MAX_RINGS - 1;
}

VkResult
vn_renderer_plan9_scanout(struct vn_renderer *renderer,
                          uint32_t res_id,
                          uint32_t width,
                          uint32_t height,
                          uint32_t stride)
{
   struct plan9 *p9 = (struct plan9 *)renderer;
   bool ok;

   mtx_lock(&p9->mutex);
   if (res_id)
      ok = plan9_ctl(p9, NULL, 0, "scanout %u %u %u %u", res_id, width,
                     height, stride);
   else
      ok = plan9_ctl(p9, NULL, 0, "scanout 0");
   mtx_unlock(&p9->mutex);
   return ok ? VK_SUCCESS : VK_ERROR_FEATURE_NOT_PRESENT;
}

static void
plan9_destroy(struct vn_renderer *renderer,
              const VkAllocationCallbacks *alloc)
{
   struct plan9 *p9 = (struct plan9 *)renderer;

   if (p9->shmem_cache.initialized)
      vn_renderer_shmem_cache_fini(&p9->shmem_cache);
   for (unsigned i = 0; i < ARRAY_SIZE(p9->bar_seg); i++) {
      if (p9->bar_seg[i].addr)
         segdetach(p9->bar_seg[i].addr);
   }
   if (p9->cmd)
      segdetach(p9->cmd);
   /* closing the files destroys the context */
   if (p9->fence_fd >= 0)
      close(p9->fence_fd);
   if (p9->submit_fd >= 0)
      close(p9->submit_fd);
   if (p9->ctl_fd >= 0)
      close(p9->ctl_fd);

   mtx_destroy(&p9->mutex);
   util_sparse_array_finish(&p9->shmem_array);
   util_sparse_array_finish(&p9->bo_array);
   util_sparse_array_finish(&p9->sync_array);

   vk_free(alloc, p9);
}

static VkResult
plan9_init_capset(struct plan9 *p9)
{
   char path[64];

   snprintf(path, sizeof(path), PLAN9_GPU_DIR "/caps/%d", PLAN9_CAPSET_VENUS);
   const int fd = open(path, O_RDONLY);
   if (fd < 0) {
      vn_log(p9->instance, "plan9: no venus capset (%s)", path);
      return VK_ERROR_INITIALIZATION_FAILED;
   }
   memset(&p9->capset, 0, sizeof(p9->capset));
   const ssize_t n = read(fd, &p9->capset, sizeof(p9->capset));
   close(fd);
   if (n < (ssize_t)offsetof(struct virgl_renderer_capset_venus,
                             vk_extension_mask1)) {
      vn_log(p9->instance, "plan9: short venus capset");
      return VK_ERROR_INITIALIZATION_FAILED;
   }
   if (!p9->capset.supports_blob_id_0) {
      vn_log(p9->instance, "plan9: renderer cannot do blob id 0");
      return VK_ERROR_INITIALIZATION_FAILED;
   }
   return VK_SUCCESS;
}

static VkResult
plan9_init_context(struct plan9 *p9)
{
   char buf[256], path[64], *f[8];

   p9->ctl_fd = open(PLAN9_GPU_DIR "/clone", O_RDWR);
   if (p9->ctl_fd < 0) {
      vn_log(p9->instance, "plan9: no " PLAN9_GPU_DIR "/clone");
      return VK_ERROR_INITIALIZATION_FAILED;
   }
   /* ctx n seg name va size */
   const ssize_t n = pread(p9->ctl_fd, buf, sizeof(buf) - 1, 0);
   if (n <= 0)
      return VK_ERROR_INITIALIZATION_FAILED;
   buf[n] = '\0';
   if (plan9_fields(buf, f, 8) != 6 || strcmp(f[0], "ctx"))
      return VK_ERROR_INITIALIZATION_FAILED;
   p9->ctx_id = atoi(f[1]);
   p9->cmd_size = strtoull(f[5], NULL, 0);
   void *cmd = segattach(0, f[3], NULL, 0);
   if (cmd == (void *)-1) {
      vn_log(p9->instance, "plan9: segattach %s failed", f[3]);
      return VK_ERROR_INITIALIZATION_FAILED;
   }
   p9->cmd = cmd;

   snprintf(path, sizeof(path), PLAN9_GPU_DIR "/ctx/%d/submit", p9->ctx_id);
   p9->submit_fd = open(path, O_WRONLY);
   snprintf(path, sizeof(path), PLAN9_GPU_DIR "/ctx/%d/fence", p9->ctx_id);
   p9->fence_fd = open(path, O_RDWR);
   if (p9->submit_fd < 0 || p9->fence_fd < 0)
      return VK_ERROR_INITIALIZATION_FAILED;

   const char *name = util_get_process_name();
   if (!plan9_ctl(p9, NULL, 0, "init %d %s", PLAN9_CAPSET_VENUS,
                  name ? name : "venus"))
      return VK_ERROR_INITIALIZATION_FAILED;
   return VK_SUCCESS;
}

static VkResult
plan9_init(struct plan9 *p9)
{
   util_sparse_array_init(&p9->shmem_array, sizeof(struct plan9_shmem), 1024);
   util_sparse_array_init(&p9->bo_array, sizeof(struct plan9_bo), 1024);
   util_sparse_array_init(&p9->sync_array, sizeof(struct plan9_sync), 1024);
   mtx_init(&p9->mutex, mtx_plain);

   VkResult result = plan9_init_capset(p9);
   if (result == VK_SUCCESS)
      result = plan9_init_context(p9);
   if (result != VK_SUCCESS)
      return result;

   vn_renderer_shmem_cache_init(&p9->shmem_cache, &p9->base,
                                plan9_shmem_destroy_now);

   plan9_init_renderer_info(p9);

   p9->base.ops.destroy = plan9_destroy;
   p9->base.ops.submit = plan9_submit;
   p9->base.ops.wait = plan9_wait;

   p9->base.shmem_ops.create = plan9_shmem_create;
   p9->base.shmem_ops.destroy = plan9_shmem_destroy;

   p9->base.bo_ops.create_from_device_memory =
      plan9_bo_create_from_device_memory;
   p9->base.bo_ops.create_from_dma_buf = plan9_bo_create_from_dma_buf;
   p9->base.bo_ops.destroy = plan9_bo_destroy;
   p9->base.bo_ops.export_dma_buf = plan9_bo_export_dma_buf;
   p9->base.bo_ops.export_sync_file = vn_renderer_bo_export_sync_file_internal;
   p9->base.bo_ops.map = plan9_bo_map;
   p9->base.bo_ops.flush = plan9_bo_flush;
   p9->base.bo_ops.invalidate = plan9_bo_invalidate;

   p9->base.sync_ops.create = plan9_sync_create;
   p9->base.sync_ops.create_from_syncobj = NULL;
   p9->base.sync_ops.destroy = plan9_sync_destroy;
   p9->base.sync_ops.export_syncobj = NULL;
   p9->base.sync_ops.reset = plan9_sync_reset;
   p9->base.sync_ops.read = plan9_sync_read;
   p9->base.sync_ops.write = plan9_sync_write;

   return VK_SUCCESS;
}

VkResult
vn_renderer_create_plan9(struct vn_instance *instance,
                         const VkAllocationCallbacks *alloc,
                         struct vn_renderer **renderer)
{
   struct plan9 *p9 = vk_zalloc(alloc, sizeof(*p9), VN_DEFAULT_ALIGN,
                                VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
   if (!p9)
      return VK_ERROR_OUT_OF_HOST_MEMORY;

   p9->instance = instance;
   p9->ctl_fd = -1;
   p9->submit_fd = -1;
   p9->fence_fd = -1;

   VkResult result = plan9_init(p9);
   if (result != VK_SUCCESS) {
      plan9_destroy(&p9->base, alloc);
      return result;
   }

   *renderer = &p9->base;

   if (VN_DEBUG(INIT))
      vn_log(p9->instance, "plan9 backend initialized");

   return VK_SUCCESS;
}
