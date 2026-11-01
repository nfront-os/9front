/*
 * SPDX-License-Identifier: MIT
 *
 * radvtest: steps of growing reach for a Vulkan driver on a real GPU
 * (RADV over aux/amdgpu), each one a submission waited for 3 s, so a hang
 * tells where it starts: an empty command buffer; vkCmdFillBuffer of 1 KB
 * (CP DMA) and of 1 MB (a compute shader of the driver's) into host
 * visible memory of each kind (GTT, VRAM through the BAR); a fill of
 * device local memory copied into host memory; vkCmdClearColorImage of
 * an image copied into host memory.  Prints "ok ..." / "FAIL ..." and a
 * summary; after a hang it stops.
 *
 * usage: radvtest
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>

static int nfail;
static VkDevice dev;
static VkQueue queue;
static VkCommandPool pool;
static VkPhysicalDeviceMemoryProperties mp;

#define FAIL(...) do { printf("FAIL "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); nfail++; } while(0)
#define OK(...) do { printf("ok "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)

typedef struct Buf Buf;
struct Buf {
	VkBuffer b;
	VkDeviceMemory m;
	uint32_t *p;
	VkDeviceSize size;
};

/* a memory type with all of want, none of avoid; -1 if none */
static int
memtype(uint32_t bits, VkMemoryPropertyFlags want, VkMemoryPropertyFlags avoid)
{
	uint32_t i;

	for(i = 0; i < mp.memoryTypeCount; i++)
		if((bits & 1u<<i) && (mp.memoryTypes[i].propertyFlags & want) == want &&
		   (mp.memoryTypes[i].propertyFlags & avoid) == 0)
			return i;
	return -1;
}

static int
mkbuf(Buf *b, VkDeviceSize size, VkMemoryPropertyFlags want, VkMemoryPropertyFlags avoid)
{
	VkBufferCreateInfo bi;
	VkMemoryRequirements mr;
	VkMemoryAllocateInfo ai;
	int t;

	memset(b, 0, sizeof *b);
	b->size = size;
	memset(&bi, 0, sizeof bi);
	bi.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bi.size = size;
	bi.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if(vkCreateBuffer(dev, &bi, NULL, &b->b) != VK_SUCCESS)
		return -1;
	vkGetBufferMemoryRequirements(dev, b->b, &mr);
	if((t = memtype(mr.memoryTypeBits, want, avoid)) < 0){
		vkDestroyBuffer(dev, b->b, NULL);
		b->b = VK_NULL_HANDLE;
		return -2;
	}
	memset(&ai, 0, sizeof ai);
	ai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	ai.allocationSize = mr.size;
	ai.memoryTypeIndex = t;
	if(vkAllocateMemory(dev, &ai, NULL, &b->m) != VK_SUCCESS ||
	   vkBindBufferMemory(dev, b->b, b->m, 0) != VK_SUCCESS)
		return -1;
	if(mp.memoryTypes[t].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		if(vkMapMemory(dev, b->m, 0, size, 0, (void**)&b->p) != VK_SUCCESS)
			return -1;
	return 0;
}

static void
freebuf(Buf *b)
{
	if(b->p != NULL)
		vkUnmapMemory(dev, b->m);
	if(b->b != VK_NULL_HANDLE)
		vkDestroyBuffer(dev, b->b, NULL);
	if(b->m != VK_NULL_HANDLE)
		vkFreeMemory(dev, b->m, NULL);
	memset(b, 0, sizeof *b);
}

static VkCommandBuffer
begin(void)
{
	VkCommandBufferAllocateInfo ai;
	VkCommandBufferBeginInfo bi;
	VkCommandBuffer cb;

	memset(&ai, 0, sizeof ai);
	ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	ai.commandPool = pool;
	ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	ai.commandBufferCount = 1;
	if(vkAllocateCommandBuffers(dev, &ai, &cb) != VK_SUCCESS)
		return VK_NULL_HANDLE;
	memset(&bi, 0, sizeof bi);
	bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(cb, &bi);
	return cb;
}

/* end, submit, wait 3 s: 0, -1 on an error, -2 on a timeout (the GPU hangs) */
static int
run(VkCommandBuffer cb, char *what)
{
	VkFenceCreateInfo fi;
	VkSubmitInfo si;
	VkFence f;
	VkResult r;

	vkEndCommandBuffer(cb);
	memset(&fi, 0, sizeof fi);
	fi.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	if(vkCreateFence(dev, &fi, NULL, &f) != VK_SUCCESS){
		FAIL("%s: vkCreateFence", what);
		return -1;
	}
	memset(&si, 0, sizeof si);
	si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	si.commandBufferCount = 1;
	si.pCommandBuffers = &cb;
	if((r = vkQueueSubmit(queue, 1, &si, f)) != VK_SUCCESS){
		FAIL("%s: vkQueueSubmit: %d", what, r);
		vkDestroyFence(dev, f, NULL);
		return -1;
	}
	r = vkWaitForFences(dev, 1, &f, VK_TRUE, 3000000000ull);
	vkDestroyFence(dev, f, NULL);
	if(r == VK_TIMEOUT){
		FAIL("%s: not done in 3 s", what);
		return -2;
	}
	if(r != VK_SUCCESS){
		FAIL("%s: vkWaitForFences: %d", what, r);
		return -1;
	}
	vkFreeCommandBuffers(dev, pool, 1, &cb);
	return 0;
}

static int
check(uint32_t *p, VkDeviceSize n, uint32_t want, char *what)
{
	VkDeviceSize i;

	for(i = 0; i < n/4; i++)
		if(p[i] != want){
			FAIL("%s: word %llu is %#x, not %#x", what, (unsigned long long)i, p[i], want);
			return -1;
		}
	OK("%s", what);
	return 0;
}

/* fill size bytes of host visible memory (want/avoid) on the GPU */
static int
fillhost(VkDeviceSize size, VkMemoryPropertyFlags want, VkMemoryPropertyFlags avoid, char *kind)
{
	char what[128];
	VkCommandBuffer cb;
	Buf b;
	int r;

	snprintf(what, sizeof what, "fill %llu bytes of %s", (unsigned long long)size, kind);
	if((r = mkbuf(&b, size, want, avoid)) < 0){
		if(r == -2)
			OK("%s: no such memory, skipped", what);
		else
			FAIL("%s: buffer", what);
		freebuf(&b);
		return r == -2 ? 0 : -1;
	}
	memset(b.p, 0, size);
	cb = begin();
	vkCmdFillBuffer(cb, b.b, 0, size, 0xc0ffee00 | (uint32_t)(size & 0xff));
	if((r = run(cb, what)) == 0)
		check(b.p, size, 0xc0ffee00 | (uint32_t)(size & 0xff), what);
	freebuf(&b);
	return r;
}

/* fill device local memory, copy it into host memory */
static int
filldev(VkDeviceSize size)
{
	char what[128];
	VkCommandBuffer cb;
	VkBufferCopy bc;
	VkMemoryBarrier mb;
	Buf d, h;
	int r;

	snprintf(what, sizeof what, "fill %llu bytes of device memory, copy to host", (unsigned long long)size);
	if(mkbuf(&d, size, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) < 0 &&
	   (freebuf(&d), mkbuf(&d, size, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0) < 0)){
		FAIL("%s: device buffer", what);
		return -1;
	}
	if(mkbuf(&h, size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 0) < 0){
		FAIL("%s: host buffer", what);
		freebuf(&d);
		return -1;
	}
	memset(h.p, 0, size);
	cb = begin();
	vkCmdFillBuffer(cb, d.b, 0, size, 0x5eed5eed);
	memset(&mb, 0, sizeof mb);
	mb.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	mb.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	mb.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 1, &mb, 0, NULL, 0, NULL);
	bc.srcOffset = bc.dstOffset = 0;
	bc.size = size;
	vkCmdCopyBuffer(cb, d.b, h.b, 1, &bc);
	if((r = run(cb, what)) == 0)
		check(h.p, size, 0x5eed5eed, what);
	freebuf(&d);
	freebuf(&h);
	return r;
}

/* clear an image, copy it into host memory */
static int
clearimage(int w, int h)
{
	char what[128];
	VkImageCreateInfo ii;
	VkMemoryRequirements mr;
	VkMemoryAllocateInfo ai;
	VkImageMemoryBarrier ib;
	VkClearColorValue cc;
	VkImageSubresourceRange sr;
	VkBufferImageCopy bic;
	VkCommandBuffer cb;
	VkImage img;
	VkDeviceMemory m;
	Buf b;
	int r, t;

	snprintf(what, sizeof what, "clear a %dx%d image, copy to host", w, h);
	memset(&ii, 0, sizeof ii);
	ii.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ii.imageType = VK_IMAGE_TYPE_2D;
	ii.format = VK_FORMAT_B8G8R8A8_UNORM;
	ii.extent.width = w;
	ii.extent.height = h;
	ii.extent.depth = 1;
	ii.mipLevels = 1;
	ii.arrayLayers = 1;
	ii.samples = VK_SAMPLE_COUNT_1_BIT;
	ii.tiling = VK_IMAGE_TILING_OPTIMAL;
	ii.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	ii.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	if(vkCreateImage(dev, &ii, NULL, &img) != VK_SUCCESS){
		FAIL("%s: vkCreateImage", what);
		return -1;
	}
	vkGetImageMemoryRequirements(dev, img, &mr);
	if((t = memtype(mr.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0)) < 0){
		FAIL("%s: no memory type", what);
		return -1;
	}
	memset(&ai, 0, sizeof ai);
	ai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	ai.allocationSize = mr.size;
	ai.memoryTypeIndex = t;
	if(vkAllocateMemory(dev, &ai, NULL, &m) != VK_SUCCESS || vkBindImageMemory(dev, img, m, 0) != VK_SUCCESS){
		FAIL("%s: image memory", what);
		return -1;
	}
	if(mkbuf(&b, (VkDeviceSize)w*h*4, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 0) < 0){
		FAIL("%s: host buffer", what);
		return -1;
	}
	memset(b.p, 0, (size_t)w*h*4);
	memset(&sr, 0, sizeof sr);
	sr.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	sr.levelCount = 1;
	sr.layerCount = 1;
	cb = begin();
	memset(&ib, 0, sizeof ib);
	ib.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	ib.srcAccessMask = 0;
	ib.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	ib.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	ib.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	ib.srcQueueFamilyIndex = ib.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ib.image = img;
	ib.subresourceRange = sr;
	vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &ib);
	cc.float32[0] = 0.0f;	/* B8G8R8A8: blue, green, red, alpha in the word's bytes */
	cc.float32[1] = 1.0f;
	cc.float32[2] = 0.0f;
	cc.float32[3] = 1.0f;
	vkCmdClearColorImage(cb, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &cc, 1, &sr);
	ib.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	ib.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	ib.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	ib.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &ib);
	memset(&bic, 0, sizeof bic);
	bic.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	bic.imageSubresource.layerCount = 1;
	bic.imageExtent = ii.extent;
	vkCmdCopyImageToBuffer(cb, img, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, b.b, 1, &bic);
	if((r = run(cb, what)) == 0)
		check(b.p, (VkDeviceSize)w*h*4, 0xff00ff00, what);
	freebuf(&b);
	vkDestroyImage(dev, img, NULL);
	vkFreeMemory(dev, m, NULL);
	return r;
}

int
main(void)
{
	VkInstanceCreateInfo ci;
	VkApplicationInfo app;
	VkDeviceCreateInfo dci;
	VkDeviceQueueCreateInfo qci;
	VkCommandPoolCreateInfo pci;
	VkPhysicalDevice pd[4];
	VkPhysicalDeviceProperties pp;
	VkQueueFamilyProperties qf[8];
	VkInstance inst;
	VkCommandBuffer cb;
	uint32_t n, nq, i, q;
	float prio = 1.0f;
	const VkMemoryPropertyFlags hv = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	const VkMemoryPropertyFlags dl = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	memset(&app, 0, sizeof app);
	app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app.pApplicationName = "radvtest";
	app.apiVersion = VK_API_VERSION_1_1;
	memset(&ci, 0, sizeof ci);
	ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	ci.pApplicationInfo = &app;
	if(vkCreateInstance(&ci, NULL, &inst) != VK_SUCCESS){
		FAIL("vkCreateInstance");
		goto out;
	}
	n = 4;
	if(vkEnumeratePhysicalDevices(inst, &n, pd) < 0 || n == 0){
		FAIL("no physical device");
		goto out;
	}
	for(i = 0; i < n; i++){
		vkGetPhysicalDeviceProperties(pd[i], &pp);
		if(pp.vendorID == 0x1002)
			break;
	}
	if(i == n){
		FAIL("no AMD device");
		goto out;
	}
	OK("device %s", pp.deviceName);
	vkGetPhysicalDeviceMemoryProperties(pd[i], &mp);
	nq = 8;
	vkGetPhysicalDeviceQueueFamilyProperties(pd[i], &nq, qf);
	for(q = 0; q < nq; q++)
		if(qf[q].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			break;
	if(q == nq){
		FAIL("no graphics queue");
		goto out;
	}
	memset(&qci, 0, sizeof qci);
	qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	qci.queueFamilyIndex = q;
	qci.queueCount = 1;
	qci.pQueuePriorities = &prio;
	memset(&dci, 0, sizeof dci);
	dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	dci.queueCreateInfoCount = 1;
	dci.pQueueCreateInfos = &qci;
	if(vkCreateDevice(pd[i], &dci, NULL, &dev) != VK_SUCCESS){
		FAIL("vkCreateDevice");
		goto out;
	}
	OK("device created");
	vkGetDeviceQueue(dev, q, 0, &queue);
	memset(&pci, 0, sizeof pci);
	pci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pci.queueFamilyIndex = q;
	if(vkCreateCommandPool(dev, &pci, NULL, &pool) != VK_SUCCESS){
		FAIL("vkCreateCommandPool");
		goto out;
	}

	cb = begin();
	if(run(cb, "empty command buffer") == -2)
		goto out;
	OK("empty command buffer");
	if(fillhost(1024, hv, dl, "GTT") == -2 ||
	   fillhost(1024, hv|dl, 0, "VRAM (BAR)") == -2 ||
	   fillhost(1<<20, hv, dl, "GTT") == -2 ||
	   fillhost(1<<20, hv|dl, 0, "VRAM (BAR)") == -2 ||
	   filldev(1<<20) == -2 ||
	   clearimage(256, 256) == -2)
		goto out;
out:
	printf(nfail ? "radvtest: %d failed\n" : "radvtest: all passed\n", nfail);
	return nfail ? 1 : 0;
}
