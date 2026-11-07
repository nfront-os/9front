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
 * offset) fail.  Prints "ok ..." / "FAIL ..." lines and a summary.
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <vulkan/vulkan.h>
#include <wayland-client.h>
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
static VkCommandPool cpool;

static void
vksetup(void)
{
	uint32_t n, qf;
	VkQueueFamilyProperties qfp[16];
	float prio = 1.0f;
	const char *dext[] = { VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME, VK_EXT_EXTERNAL_MEMORY_DMA_BUF_EXTENSION_NAME };
	VkApplicationInfo app = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "wlvk",
		.apiVersion = VK_API_VERSION_1_1,
	};
	VkInstanceCreateInfo ici = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app,
	};
	VK(vkCreateInstance(&ici, NULL, &inst), "vkCreateInstance");
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
		.enabledExtensionCount = 2,
		.ppEnabledExtensionNames = dext,
	};
	VK(vkCreateDevice(pd, &dci, NULL, &dev), "vkCreateDevice (external memory fd, dma-buf)");
	vkGetDeviceQueue(dev, qf, 0, &q);
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

/* the window of wsys labelled "wlvk", and where it is */
static int
findwin(void)
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
		if(readfile(path, buf, sizeof buf) >= 0 && strcmp(buf, "wlvk") == 0)
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
	check("a window of wsys labelled wlvk", findwin() == 0, NULL);
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
	wl_display_disconnect(d);
	printf(nfail ? "wlvk: %d failed\n" : "wlvk: all passed\n", nfail);
	return nfail != 0;
}
