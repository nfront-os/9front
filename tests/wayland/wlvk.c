/*
 * SPDX-License-Identifier: MIT
 *
 * wlvk: a Wayland client of wlsys with GPU buffers (../9front-ui PLAN §12,
 * stage 6.6), run by wlsys in wsys -x -i (wlvk.rc) on venus.  Two buffers of
 * exportable, host visible device memory, filled on the GPU (vkCmdFillBuffer:
 * orange, then cyan), their dma-buf fds (vkGetMemoryFdKHR: Mesa's
 * plan9/lib/dmabuf.c) handed to wlsys by zwp_linux_dmabuf_v1 (one by create,
 * one by create_immed); the toplevel shows one, then the other, read back
 * from wsys' screen; the first released.  Params wlsys must refuse (an
 * offset) fail.  Then Mesa's Wayland WSI: a second toplevel with a
 * VkSurfaceKHR of its wl_surface (VK_KHR_wayland_surface), a swapchain,
 * three frames cleared to three colours, each read back by the GPU first
 * (the image is right) and then from wsys' screen (it got there).
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <wayland-client.h>
#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan.h>
#include <xdg-shell-client-protocol.h>
#include <linux-dmabuf-unstable-v1-client-protocol.h>

enum {
	W = 160,
	H = 120,
	Stride = W*4,
	Xrgb = 0x34325258,	/* DRM_FORMAT_XRGB8888 */
	Orange = 0xff8000,
	Cyan = 0x00c0c0,
};

static int nfail;

static void
check(const char *what, int ok, const char *fmt, ...)
{
	va_list a;

	if(ok)
		printf("ok %s\n", what);
	else{
		printf("FAIL %s", what);
		if(fmt != NULL){
			printf(": ");
			va_start(a, fmt);
			vprintf(fmt, a);
			va_end(a);
		}
		printf("\n");
		nfail++;
	}
	fflush(stdout);
}

#define FATAL(...) do { check("setup", 0, __VA_ARGS__); printf("wlvk: failed\n"); exit(1); } while(0)
#define VK(call, what) do { VkResult r_ = (call); if(r_ != VK_SUCCESS) FATAL("%s: VkResult %d", what, r_); } while(0)

/* ---- Vulkan: exportable buffers, filled on the GPU ---- */

static VkInstance inst;
static VkPhysicalDevice pd;
static VkDevice dev;
static VkQueue q;
static uint32_t qfam;
static VkCommandPool cpool;

static void
vksetup(void)
{
	uint32_t n, qf;
	VkQueueFamilyProperties qfp[16];
	float prio = 1.0f;
	const char *dext[] = { VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME, VK_EXT_EXTERNAL_MEMORY_DMA_BUF_EXTENSION_NAME,
		VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	const char *iext[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME };
	VkApplicationInfo app = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "wlvk",
		.apiVersion = VK_API_VERSION_1_1,
	};
	VkInstanceCreateInfo ici = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app,
		.enabledExtensionCount = 2,
		.ppEnabledExtensionNames = iext,
	};
	VK(vkCreateInstance(&ici, NULL, &inst), "vkCreateInstance (VK_KHR_wayland_surface)");
	n = 1;
	vkEnumeratePhysicalDevices(inst, &n, &pd);
	if(n == 0)
		FATAL("no physical device");
	n = 16;
	vkGetPhysicalDeviceQueueFamilyProperties(pd, &n, qfp);
	for(qf = 0; qf < n; qf++)
		if(qfp[qf].queueFlags & (VK_QUEUE_TRANSFER_BIT|VK_QUEUE_GRAPHICS_BIT))
			break;
	if(qf == n)
		FATAL("no queue family");
	VkDeviceQueueCreateInfo qci = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = qf,
		.queueCount = 1,
		.pQueuePriorities = &prio,
	};
	VkDeviceCreateInfo dci = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &qci,
		.enabledExtensionCount = 3,
		.ppEnabledExtensionNames = dext,
	};
	VK(vkCreateDevice(pd, &dci, NULL, &dev), "vkCreateDevice (external memory fd, dma-buf, swapchain)");
	vkGetDeviceQueue(dev, qf, 0, &q);
	qfam = qf;
	VkCommandPoolCreateInfo pci = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = qf,
	};
	VK(vkCreateCommandPool(dev, &pci, NULL, &cpool), "vkCreateCommandPool");
}

static uint32_t
memtype(uint32_t bits, VkMemoryPropertyFlags want)
{
	VkPhysicalDeviceMemoryProperties mp;
	uint32_t i;

	vkGetPhysicalDeviceMemoryProperties(pd, &mp);
	for(i = 0; i < mp.memoryTypeCount; i++)
		if((bits & 1u<<i) && (mp.memoryTypes[i].propertyFlags & want) == want)
			return i;
	FATAL("no memory type for bits %#x flags %#x", bits, want);
	return 0;
}

/* a buffer of W×H xrgb pixels of colour rgb, filled by the GPU; its dma-buf fd */
static int
gpubuffer(uint32_t rgb)
{
	VkBuffer b;
	VkDeviceMemory m;
	VkMemoryRequirements mr;
	VkCommandBuffer cb;
	VkFence f;
	int fd;

	VkExternalMemoryBufferCreateInfo ext = {
		.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO,
		.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
	};
	VkBufferCreateInfo bci = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = &ext,
		.size = Stride*H,
		.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};
	VK(vkCreateBuffer(dev, &bci, NULL, &b), "vkCreateBuffer");
	vkGetBufferMemoryRequirements(dev, b, &mr);
	VkExportMemoryAllocateInfo exp = {
		.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO,
		.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
	};
	/* host visible: wsys reads the pixels where the processor sees them */
	VkMemoryAllocateInfo mai = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = &exp,
		.allocationSize = mr.size,
		.memoryTypeIndex = memtype(mr.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
	};
	VK(vkAllocateMemory(dev, &mai, NULL, &m), "vkAllocateMemory (exportable, host visible)");
	VK(vkBindBufferMemory(dev, b, m, 0), "vkBindBufferMemory");

	VkCommandBufferAllocateInfo cai = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = cpool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};
	VkCommandBufferBeginInfo bi = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};
	VK(vkAllocateCommandBuffers(dev, &cai, &cb), "vkAllocateCommandBuffers");
	VK(vkBeginCommandBuffer(cb, &bi), "vkBeginCommandBuffer");
	vkCmdFillBuffer(cb, b, 0, Stride*H, 0xff000000 | rgb);
	VK(vkEndCommandBuffer(cb), "vkEndCommandBuffer");
	VkFenceCreateInfo fci = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	VkSubmitInfo si = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &cb,
	};
	VK(vkCreateFence(dev, &fci, NULL, &f), "vkCreateFence");
	VK(vkQueueSubmit(q, 1, &si, f), "vkQueueSubmit");
	/* done before wlsys hears of it: no fences in the protocol here */
	VK(vkWaitForFences(dev, 1, &f, VK_TRUE, 10000000000ULL), "vkWaitForFences");
	vkDestroyFence(dev, f, NULL);

	VkMemoryGetFdInfoKHR gfi = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR,
		.memory = m,
		.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
	};
	PFN_vkGetMemoryFdKHR getfd = (PFN_vkGetMemoryFdKHR)vkGetDeviceProcAddr(dev, "vkGetMemoryFdKHR");
	if(getfd == NULL)
		FATAL("no vkGetMemoryFdKHR");
	VK(getfd(dev, &gfi, &fd), "vkGetMemoryFdKHR");
	return fd;
}

/* ---- Wayland ---- */

static struct wl_display *d;
static struct wl_compositor *comp;
static struct xdg_wm_base *wm;
static struct zwp_linux_dmabuf_v1 *dmabuf;
static int linear;	/* xrgb8888 linear offered */
static struct wl_surface *surf;
static struct xdg_surface *xsurf;
static struct xdg_toplevel *top;
static int configured;
static uint32_t confserial;
static int released[2];
static struct wl_buffer *created;
static int gotcreated, failed;

/* dispatch until *flag or about 5 s went by */
static int
waitfor(int *flag)
{
	int i;

	for(i = 0; i < 100 && !*flag; i++){
		wl_display_flush(d);
		if(wl_display_roundtrip(d) < 0)
			return 0;
		if(!*flag)
			usleep(50*1000);
	}
	return *flag;
}

static void
xdgping(void *data, struct xdg_wm_base *b, uint32_t serial)
{
	xdg_wm_base_pong(b, serial);
}
static const struct xdg_wm_base_listener wmlistener = { xdgping };

static void
xsconfigure(void *data, struct xdg_surface *x, uint32_t serial)
{
	confserial = serial;
	configured = 1;
}
static const struct xdg_surface_listener xslistener = { xsconfigure };

static void topconfigure(void *data, struct xdg_toplevel *t, int32_t w, int32_t h, struct wl_array *states) {}
static void topclose(void *data, struct xdg_toplevel *t) {}
static void topbounds(void *data, struct xdg_toplevel *t, int32_t w, int32_t h) {}
static void topcaps(void *data, struct xdg_toplevel *t, struct wl_array *caps) {}
static const struct xdg_toplevel_listener toplistener = { topconfigure, topclose, topbounds, topcaps };

static void
bufrelease(void *data, struct wl_buffer *b)
{
	released[(intptr_t)data] = 1;
}
static const struct wl_buffer_listener buflistener = { bufrelease };

static void
dmaformat(void *data, struct zwp_linux_dmabuf_v1 *z, uint32_t format)
{
}

static void
dmamodifier(void *data, struct zwp_linux_dmabuf_v1 *z, uint32_t format, uint32_t hi, uint32_t lo)
{
	if(format == Xrgb && hi == 0 && lo == 0)
		linear = 1;
}
static const struct zwp_linux_dmabuf_v1_listener dmalistener = { dmaformat, dmamodifier };

static void
pcreated(void *data, struct zwp_linux_buffer_params_v1 *p, struct wl_buffer *b)
{
	created = b;
	gotcreated = 1;
}

static void
pfailed(void *data, struct zwp_linux_buffer_params_v1 *p)
{
	failed = 1;
}
static const struct zwp_linux_buffer_params_v1_listener paramslistener = { pcreated, pfailed };

static void
global(void *data, struct wl_registry *r, uint32_t name, const char *iface, uint32_t version)
{
	if(strcmp(iface, wl_compositor_interface.name) == 0)
		comp = wl_registry_bind(r, name, &wl_compositor_interface, 4);
	else if(strcmp(iface, xdg_wm_base_interface.name) == 0){
		wm = wl_registry_bind(r, name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(wm, &wmlistener, NULL);
	}else if(strcmp(iface, zwp_linux_dmabuf_v1_interface.name) == 0 && version >= 3){
		dmabuf = wl_registry_bind(r, name, &zwp_linux_dmabuf_v1_interface, 3);
		zwp_linux_dmabuf_v1_add_listener(dmabuf, &dmalistener, NULL);
	}
}

static void globalremove(void *data, struct wl_registry *r, uint32_t name) {}
static const struct wl_registry_listener reglistener = { global, globalremove };

static struct zwp_linux_buffer_params_v1*
params(int fd, uint32_t off)
{
	struct zwp_linux_buffer_params_v1 *p;

	p = zwp_linux_dmabuf_v1_create_params(dmabuf);
	zwp_linux_buffer_params_v1_add(p, fd, 0, off, Stride, 0, 0);
	return p;
}

/* ---- wsys, from the outside: the window of ours, its screen ---- */

static int wx0, wy0;

static int
readfile(char *path, char *buf, int n)
{
	int fd, m;

	if((fd = open(path, O_RDONLY)) < 0)
		return -1;
	m = read(fd, buf, n-1);
	close(fd);
	if(m < 0)
		return -1;
	buf[m] = 0;
	return m;
}

/* the window of wsys labelled label, and where it is */
static int
findwin(char *label)
{
	DIR *dir;
	struct dirent *e;
	char path[128], buf[256];
	int id, winid, x1, y1;

	if((dir = opendir("/mnt/wsys")) == NULL)
		return -1;
	winid = -1;
	while((e = readdir(dir)) != NULL){
		if((id = atoi(e->d_name)) <= 0)
			continue;
		snprintf(path, sizeof path, "/mnt/wsys/%d/label", id);
		if(readfile(path, buf, sizeof buf) >= 0 && strcmp(buf, label) == 0)
			winid = id;
	}
	closedir(dir);
	if(winid < 0)
		return -1;
	snprintf(path, sizeof path, "/mnt/wsys/%d/ctl", winid);
	if(readfile(path, buf, sizeof buf) < 0 ||
	   sscanf(buf, "%d %d %d %d %d", &id, &wx0, &wy0, &x1, &y1) != 5)
		return -1;
	return 0;
}

/* the screen's pixel at x, y (screen coordinates) as 0xRRGGBB; -1 if not read */
static long
peek(int x, int y)
{
	static unsigned char *img;
	char hdr[61], chan[16];
	int fd, sx0, sy0, sx1, sy1, n, total, m;
	unsigned char *p;

	if((fd = open("/mnt/wsys/screen", O_RDONLY)) < 0)
		return -1;
	if(read(fd, hdr, 60) != 60){
		close(fd);
		return -1;
	}
	hdr[60] = 0;
	if(sscanf(hdr, "%15s %d %d %d %d", chan, &sx0, &sy0, &sx1, &sy1) != 5 ||
	   (strcmp(chan, "x8r8g8b8") != 0 && strcmp(chan, "a8r8g8b8") != 0)){
		fprintf(stderr, "wlvk: screen %s\n", hdr);
		close(fd);
		return -1;
	}
	total = (sx1 - sx0) * (sy1 - sy0) * 4;
	free(img);
	if((img = malloc(total)) == NULL){
		close(fd);
		return -1;
	}
	for(n = 0; n < total; n += m)
		if((m = read(fd, img + n, total - n)) <= 0)
			break;
	close(fd);
	if(n < total || x < sx0 || y < sy0 || x >= sx1 || y >= sy1)
		return -1;
	p = img + ((long)(y - sy0) * (sx1 - sx0) + (x - sx0)) * 4;
	return (long)p[2]<<16 | p[1]<<8 | p[0];
}

/* the window at x, y is rgb, within a second */
static void
want(int x, int y, long rgb, char *what)
{
	char buf[128];
	long v;
	int i;

	v = -1;
	for(i = 0; i < 50; i++){
		if((v = peek(wx0 + x, wy0 + y)) == rgb)
			break;
		usleep(20*1000);
	}
	snprintf(buf, sizeof buf, "the window at %d,%d: %s", x, y, what);
	check(buf, v == rgb, "%06lx, want %06lx", v, rgb);
}

/* ---- Mesa's Wayland WSI ---- */

/*
 * the swapchain image read back by the GPU into a buffer of ours (host
 * visible): whether the frame is right before the WSI hands it on
 */
static VkBuffer rbbuf;
static VkDeviceMemory rbmem;
static uint32_t *rbmap;

static void
readbackinit(void)
{
	VkMemoryRequirements mr;
	VkBufferCreateInfo bci = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = W*H*4,
		.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};
	VK(vkCreateBuffer(dev, &bci, NULL, &rbbuf), "vkCreateBuffer (read back)");
	vkGetBufferMemoryRequirements(dev, rbbuf, &mr);
	VkMemoryAllocateInfo mai = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = mr.size,
		.memoryTypeIndex = memtype(mr.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
	};
	VK(vkAllocateMemory(dev, &mai, NULL, &rbmem), "vkAllocateMemory (read back)");
	VK(vkBindBufferMemory(dev, rbbuf, rbmem, 0), "vkBindBufferMemory (read back)");
	VK(vkMapMemory(dev, rbmem, 0, VK_WHOLE_SIZE, 0, (void **)&rbmap), "vkMapMemory (read back)");
}

static void
clearframe(VkSwapchainKHR sc, VkImage *imgs, float r, float g, float b, char *what)
{
	VkSemaphore acq, done;
	VkFence f;
	VkCommandBuffer cb;
	uint32_t i;
	VkSemaphoreCreateInfo sci = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkFenceCreateInfo fci = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	VkResult res;
	char buf[96];

	VK(vkCreateSemaphore(dev, &sci, NULL, &acq), "vkCreateSemaphore");
	VK(vkCreateSemaphore(dev, &sci, NULL, &done), "vkCreateSemaphore");
	VK(vkCreateFence(dev, &fci, NULL, &f), "vkCreateFence");
	res = vkAcquireNextImageKHR(dev, sc, 5000000000ULL, acq, VK_NULL_HANDLE, &i);
	snprintf(buf, sizeof buf, "vkAcquireNextImageKHR (%s)", what);
	check(buf, res == VK_SUCCESS || res == VK_SUBOPTIMAL_KHR, "VkResult %d", res);
	if(res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)
		return;

	VkCommandBufferAllocateInfo cai = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = cpool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};
	VkCommandBufferBeginInfo bi = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};
	VkImageSubresourceRange range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	VkImageMemoryBarrier tobuf = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = imgs[i],
		.subresourceRange = range,
	};
	VkImageMemoryBarrier tosrc = tobuf;
	tosrc.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	tosrc.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	tosrc.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	tosrc.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	VkImageMemoryBarrier topresent = tobuf;
	topresent.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	topresent.dstAccessMask = 0;
	topresent.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	topresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	VkBufferImageCopy rb = {
		.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
		.imageExtent = { W, H, 1 },
	};
	VkBufferMemoryBarrier tohost = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
		.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_HOST_READ_BIT,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.buffer = rbbuf,
		.size = VK_WHOLE_SIZE,
	};
	VkClearColorValue c = { .float32 = { r, g, b, 1.0f } };

	VK(vkAllocateCommandBuffers(dev, &cai, &cb), "vkAllocateCommandBuffers");
	VK(vkBeginCommandBuffer(cb, &bi), "vkBeginCommandBuffer");
	vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		0, 0, NULL, 0, NULL, 1, &tobuf);
	vkCmdClearColorImage(cb, imgs[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &c, 1, &range);
	vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		0, 0, NULL, 0, NULL, 1, &tosrc);
	vkCmdCopyImageToBuffer(cb, imgs[i], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, rbbuf, 1, &rb);
	vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT,
		0, 0, NULL, 1, &tohost, 0, NULL);
	vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0, 0, NULL, 0, NULL, 1, &topresent);
	VK(vkEndCommandBuffer(cb), "vkEndCommandBuffer");
	VkPipelineStageFlags stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	VkSubmitInfo si = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &acq,
		.pWaitDstStageMask = &stage,
		.commandBufferCount = 1,
		.pCommandBuffers = &cb,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &done,
	};
	VK(vkQueueSubmit(q, 1, &si, f), "vkQueueSubmit");
	VkPresentInfoKHR pi = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &done,
		.swapchainCount = 1,
		.pSwapchains = &sc,
		.pImageIndices = &i,
	};
	res = vkQueuePresentKHR(q, &pi);
	snprintf(buf, sizeof buf, "vkQueuePresentKHR (%s)", what);
	check(buf, res == VK_SUCCESS || res == VK_SUBOPTIMAL_KHR, "VkResult %d", res);
	VK(vkWaitForFences(dev, 1, &f, VK_TRUE, 10000000000ULL), "vkWaitForFences");
	{
		uint32_t want = 0xff000000u | (uint32_t)(r*255+0.5)<<16 | (uint32_t)(g*255+0.5)<<8 | (uint32_t)(b*255+0.5);
		uint32_t v = rbmap[(H/2)*W + W/2];

		snprintf(buf, sizeof buf, "wsi: the swapchain image read back by the GPU (%s)", what);
		check(buf, v == want, "%08x, want %08x", v, want);
	}
	vkDestroyFence(dev, f, NULL);
	vkFreeCommandBuffers(dev, cpool, 1, &cb);
	/* the semaphores may still be in use by the present: kept */
}

static void
wsi(void)
{
	struct wl_surface *ws;
	struct xdg_surface *wx;
	struct xdg_toplevel *wt;
	VkSurfaceKHR vs;
	VkSwapchainKHR sc;
	VkSurfaceCapabilitiesKHR caps;
	VkSurfaceFormatKHR fmts[16];
	VkImage imgs[8];
	VkBool32 ok;
	uint32_t n, i;
	VkResult res;

	check("vkGetPhysicalDeviceWaylandPresentationSupportKHR",
		vkGetPhysicalDeviceWaylandPresentationSupportKHR(pd, qfam, d), NULL);
	ws = wl_compositor_create_surface(comp);
	wx = xdg_wm_base_get_xdg_surface(wm, ws);
	configured = 0;
	xdg_surface_add_listener(wx, &xslistener, NULL);
	wt = xdg_surface_get_toplevel(wx);
	xdg_toplevel_add_listener(wt, &toplistener, NULL);
	xdg_toplevel_set_title(wt, "wlvk-wsi");
	wl_surface_commit(ws);
	check("wsi: configure", waitfor(&configured), NULL);
	xdg_surface_ack_configure(wx, confserial);

	VkWaylandSurfaceCreateInfoKHR wci = {
		.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
		.display = d,
		.surface = ws,
	};
	VK(vkCreateWaylandSurfaceKHR(inst, &wci, NULL, &vs), "vkCreateWaylandSurfaceKHR");
	ok = VK_FALSE;
	vkGetPhysicalDeviceSurfaceSupportKHR(pd, qfam, vs, &ok);
	check("wsi: the queue presents to the surface", ok, NULL);
	VK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pd, vs, &caps), "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
	n = 16;
	VK(vkGetPhysicalDeviceSurfaceFormatsKHR(pd, vs, &n, fmts), "vkGetPhysicalDeviceSurfaceFormatsKHR");
	for(i = 0; i < n; i++)
		if(fmts[i].format == VK_FORMAT_B8G8R8A8_UNORM)
			break;
	check("wsi: B8G8R8A8_UNORM offered", i < n, "%u formats", n);
	if(i == n)
		return;
	VkSwapchainCreateInfoKHR sci = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = vs,
		.minImageCount = caps.minImageCount < 3 ? 3 : caps.minImageCount,
		.imageFormat = VK_FORMAT_B8G8R8A8_UNORM,
		.imageColorSpace = fmts[i].colorSpace,
		.imageExtent = { W, H },
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_FIFO_KHR,
		.clipped = VK_TRUE,
	};
	if(sci.minImageCount > 8)
		sci.minImageCount = 8;
	res = vkCreateSwapchainKHR(dev, &sci, NULL, &sc);
	check("wsi: vkCreateSwapchainKHR", res == VK_SUCCESS, "VkResult %d", res);
	if(res != VK_SUCCESS)
		return;
	n = 8;
	VK(vkGetSwapchainImagesKHR(dev, sc, &n, imgs), "vkGetSwapchainImagesKHR");
	readbackinit();

	clearframe(sc, imgs, 1, 0, 1, "magenta");
	usleep(200*1000);
	check("wsi: a window of wsys labelled wlvk-wsi", findwin("wlvk-wsi") == 0, NULL);
	want(W/2, H/2, 0xff00ff, "the WSI's first frame (magenta)");
	clearframe(sc, imgs, 1, 1, 0, "yellow");
	want(W/2, H/2, 0xffff00, "the second (yellow)");
	clearframe(sc, imgs, 0, 1, 0, "green");
	want(W-1, H-1, 0x00ff00, "the third (green), its last pixel");

	vkDeviceWaitIdle(dev);
	vkDestroySwapchainKHR(dev, sc, NULL);
	vkDestroySurfaceKHR(inst, vs, NULL);
	xdg_toplevel_destroy(wt);
	xdg_surface_destroy(wx);
	wl_surface_destroy(ws);
	wl_display_roundtrip(d);
}

int
main(void)
{
	struct zwp_linux_buffer_params_v1 *p;
	struct wl_buffer *b0, *b1;
	int fd0, fd1, fdx;

	vksetup();
	fd0 = gpubuffer(Orange);
	fd1 = gpubuffer(Cyan);
	fdx = gpubuffer(0);
	check("three buffers filled on the GPU, their dma-buf fds", fd0 >= 0 && fd1 >= 0 && fdx >= 0, NULL);

	if((d = wl_display_connect(NULL)) == NULL)
		FATAL("wl_display_connect: no wlsys");
	wl_registry_add_listener(wl_display_get_registry(d), &reglistener, NULL);
	wl_display_roundtrip(d);
	wl_display_roundtrip(d);
	check("zwp_linux_dmabuf_v1 (version 3) offered", dmabuf != NULL, NULL);
	if(comp == NULL || wm == NULL || dmabuf == NULL)
		FATAL("globals: compositor %p, xdg_wm_base %p, linux-dmabuf %p", comp, wm, dmabuf);
	check("xrgb8888, linear", linear, NULL);

	/* by create: the created event */
	p = params(fd0, 0);
	zwp_linux_buffer_params_v1_add_listener(p, &paramslistener, NULL);
	zwp_linux_buffer_params_v1_create(p, W, H, Xrgb, 0);
	waitfor(&gotcreated);
	check("params create: created", created != NULL, failed ? "failed" : "nothing");
	b0 = created;
	zwp_linux_buffer_params_v1_destroy(p);
	close(fd0);

	/* by create_immed */
	p = params(fd1, 0);
	b1 = zwp_linux_buffer_params_v1_create_immed(p, W, H, Xrgb, 0);
	zwp_linux_buffer_params_v1_destroy(p);
	close(fd1);
	wl_display_roundtrip(d);
	check("params create_immed: a buffer, no error", b1 != NULL && wl_display_get_error(d) == 0,
		"error %d", wl_display_get_error(d));

	/* refused: an offset (wsys takes an object from its start) */
	created = NULL;
	failed = 0;
	p = params(fdx, 4096);
	zwp_linux_buffer_params_v1_add_listener(p, &paramslistener, NULL);
	zwp_linux_buffer_params_v1_create(p, W, H-1, Xrgb, 0);
	waitfor(&failed);
	check("params with an offset: failed", failed && created == NULL, NULL);
	zwp_linux_buffer_params_v1_destroy(p);
	close(fdx);
	if(b0 == NULL || b1 == NULL)
		FATAL("no buffers");
	wl_buffer_add_listener(b0, &buflistener, (void*)0);
	wl_buffer_add_listener(b1, &buflistener, (void*)1);

	surf = wl_compositor_create_surface(comp);
	xsurf = xdg_wm_base_get_xdg_surface(wm, surf);
	xdg_surface_add_listener(xsurf, &xslistener, NULL);
	top = xdg_surface_get_toplevel(xsurf);
	xdg_toplevel_add_listener(top, &toplistener, NULL);
	xdg_toplevel_set_title(top, "wlvk");
	wl_surface_commit(surf);
	check("configure", waitfor(&configured), NULL);
	xdg_surface_ack_configure(xsurf, confserial);

	wl_surface_attach(surf, b0, 0, 0);
	wl_surface_damage_buffer(surf, 0, 0, W, H);
	wl_surface_commit(surf);
	wl_display_roundtrip(d);
	usleep(200*1000);
	check("a window of wsys labelled wlvk", findwin("wlvk") == 0, NULL);
	want(10, 10, Orange, "the first GPU buffer (orange)");
	want(W-1, H-1, Orange, "its last pixel");

	wl_surface_attach(surf, b1, 0, 0);
	wl_surface_damage_buffer(surf, 0, 0, W, H);
	wl_surface_commit(surf);
	wl_display_roundtrip(d);
	want(W/2, H/2, Cyan, "the second (cyan)");
	check("the first released", waitfor(&released[0]), NULL);

	xdg_toplevel_destroy(top);
	xdg_surface_destroy(xsurf);
	wl_surface_destroy(surf);
	wl_buffer_destroy(b0);
	wl_buffer_destroy(b1);
	wl_display_roundtrip(d);

	wsi();
	wl_display_disconnect(d);
	printf(nfail ? "wlvk: %d failed\n" : "wlvk: all passed\n", nfail);
	return nfail != 0;
}
