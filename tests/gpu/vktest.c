/*
 * SPDX-License-Identifier: MIT
 *
 * vktest: Vulkan on 9front through Mesa's venus (statically linked,
 * no loader: everything from vk_icdGetInstanceProcAddr) and the
 * virtio-gpu driver of gpu(4). Creates an instance and a device,
 * fills a host visible buffer on the GPU (vkCmdFillBuffer) and reads
 * it back. Prints "ok ..." / "FAIL ..." lines and a summary.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>

extern PFN_vkVoidFunction vk_icdGetInstanceProcAddr(VkInstance, const char *);
extern VkResult vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t *);

static int nfail;

#define FAIL(...) do { printf("FAIL "); printf(__VA_ARGS__); printf("\n"); nfail++; } while(0)
#define OK(...) do { printf("ok "); printf(__VA_ARGS__); printf("\n"); } while(0)
#define CHECK(call, what) do { VkResult r_ = (call); if(r_ != VK_SUCCESS){ FAIL("%s: VkResult %d", what, r_); goto out; } } while(0)

#define IFN(name) PFN_##name name = (PFN_##name)vk_icdGetInstanceProcAddr(inst, #name); \
	if(name == NULL){ FAIL("no %s", #name); goto out; }
#define DFN(name) PFN_##name name = (PFN_##name)GetDeviceProcAddr(dev, #name); \
	if(name == NULL){ FAIL("no %s", #name); goto out; }

int
main(void)
{
	VkInstance inst = VK_NULL_HANDLE;
	VkDevice dev = VK_NULL_HANDLE;
	uint32_t ver = 5, n, i, qf, mt;
	const VkDeviceSize size = 65536;

	if(vk_icdNegotiateLoaderICDInterfaceVersion(&ver) != VK_SUCCESS){
		FAIL("negotiate the ICD interface");
		goto out;
	}
	PFN_vkCreateInstance CreateInstance =
		(PFN_vkCreateInstance)vk_icdGetInstanceProcAddr(NULL, "vkCreateInstance");
	VkApplicationInfo app = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "vktest",
		.apiVersion = VK_API_VERSION_1_1,
	};
	VkInstanceCreateInfo ici = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app,
	};
	CHECK(CreateInstance(&ici, NULL, &inst), "vkCreateInstance");
	OK("instance (ICD interface version %u)", ver);

	IFN(vkEnumeratePhysicalDevices);
	IFN(vkGetPhysicalDeviceProperties);
	IFN(vkGetPhysicalDeviceQueueFamilyProperties);
	IFN(vkGetPhysicalDeviceMemoryProperties);
	IFN(vkCreateDevice);
	IFN(vkDestroyInstance);
	PFN_vkGetDeviceProcAddr GetDeviceProcAddr =
		(PFN_vkGetDeviceProcAddr)vk_icdGetInstanceProcAddr(inst, "vkGetDeviceProcAddr");
	if(GetDeviceProcAddr == NULL){
		FAIL("no vkGetDeviceProcAddr");
		goto out;
	}

	VkPhysicalDevice pd[8];
	n = 8;
	CHECK(vkEnumeratePhysicalDevices(inst, &n, pd), "vkEnumeratePhysicalDevices");
	if(n == 0){
		FAIL("no physical device");
		goto out;
	}
	VkPhysicalDeviceProperties props;
	for(i = 0; i < n; i++){
		vkGetPhysicalDeviceProperties(pd[i], &props);
		OK("device %u: %s, Vulkan %u.%u.%u, vendor %#x", i, props.deviceName,
			VK_API_VERSION_MAJOR(props.apiVersion), VK_API_VERSION_MINOR(props.apiVersion),
			VK_API_VERSION_PATCH(props.apiVersion), props.vendorID);
	}

	VkQueueFamilyProperties qfp[16];
	n = 16;
	vkGetPhysicalDeviceQueueFamilyProperties(pd[0], &n, qfp);
	for(qf = 0; qf < n; qf++)
		if(qfp[qf].queueFlags & (VK_QUEUE_GRAPHICS_BIT|VK_QUEUE_COMPUTE_BIT|VK_QUEUE_TRANSFER_BIT))
			break;
	if(qf == n){
		FAIL("no queue family");
		goto out;
	}
	float prio = 1.0f;
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
	};
	CHECK(vkCreateDevice(pd[0], &dci, NULL, &dev), "vkCreateDevice");
	OK("device created, queue family %u", qf);

	DFN(vkGetDeviceQueue);
	DFN(vkCreateBuffer);
	DFN(vkGetBufferMemoryRequirements);
	DFN(vkAllocateMemory);
	DFN(vkBindBufferMemory);
	DFN(vkMapMemory);
	DFN(vkCreateCommandPool);
	DFN(vkAllocateCommandBuffers);
	DFN(vkBeginCommandBuffer);
	DFN(vkCmdFillBuffer);
	DFN(vkCmdCopyBuffer);
	DFN(vkResetCommandBuffer);
	DFN(vkResetFences);
	DFN(vkCreateImage);
	DFN(vkGetImageMemoryRequirements);
	DFN(vkBindImageMemory);
	DFN(vkCmdPipelineBarrier);
	DFN(vkCmdCopyBufferToImage);
	DFN(vkCmdCopyImageToBuffer);
	DFN(vkEndCommandBuffer);
	DFN(vkCreateFence);
	DFN(vkQueueSubmit);
	DFN(vkWaitForFences);
	DFN(vkDestroyDevice);
	DFN(vkDeviceWaitIdle);

	VkQueue q;
	vkGetDeviceQueue(dev, qf, 0, &q);

	VkBuffer buf;
	VkBufferCreateInfo bci = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
	};
	CHECK(vkCreateBuffer(dev, &bci, NULL, &buf), "vkCreateBuffer");
	VkMemoryRequirements req;
	vkGetBufferMemoryRequirements(dev, buf, &req);
	VkPhysicalDeviceMemoryProperties mp;
	vkGetPhysicalDeviceMemoryProperties(pd[0], &mp);
	for(i = 0; i < mp.memoryTypeCount; i++)
		OK("memory type %u: heap %u flags %#x%s%s%s%s", i, mp.memoryTypes[i].heapIndex,
			mp.memoryTypes[i].propertyFlags,
			mp.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ? " device-local" : "",
			mp.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ? " host-visible" : "",
			mp.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ? " coherent" : "",
			mp.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT ? " cached" : "");
	const VkMemoryPropertyFlags want = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for(mt = 0; mt < mp.memoryTypeCount; mt++)
		if((req.memoryTypeBits & (1u<<mt)) && (mp.memoryTypes[mt].propertyFlags & want) == want)
			break;
	if(mt == mp.memoryTypeCount){
		FAIL("no host visible coherent memory");
		goto out;
	}
	VkDeviceMemory mem;
	VkMemoryAllocateInfo mai = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = req.size,
		.memoryTypeIndex = mt,
	};
	CHECK(vkAllocateMemory(dev, &mai, NULL, &mem), "vkAllocateMemory");
	CHECK(vkBindBufferMemory(dev, buf, mem, 0), "vkBindBufferMemory");
	uint32_t *p;
	CHECK(vkMapMemory(dev, mem, 0, size, 0, (void**)&p), "vkMapMemory");
	memset(p, 0, size);
	OK("buffer of %llu bytes in memory type %u, mapped", (unsigned long long)size, mt);

	VkCommandPool pool;
	VkCommandPoolCreateInfo pci = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = qf,
	};
	CHECK(vkCreateCommandPool(dev, &pci, NULL, &pool), "vkCreateCommandPool");
	VkCommandBuffer cb;
	VkCommandBufferAllocateInfo cai = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};
	CHECK(vkAllocateCommandBuffers(dev, &cai, &cb), "vkAllocateCommandBuffers");
	VkCommandBufferBeginInfo cbi = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};
	CHECK(vkBeginCommandBuffer(cb, &cbi), "vkBeginCommandBuffer");
	vkCmdFillBuffer(cb, buf, 0, size, 0x9f5a9f5a);
	CHECK(vkEndCommandBuffer(cb), "vkEndCommandBuffer");

	VkFence fence;
	VkFenceCreateInfo fci = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	CHECK(vkCreateFence(dev, &fci, NULL, &fence), "vkCreateFence");
	VkSubmitInfo si = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &cb,
	};
	CHECK(vkQueueSubmit(q, 1, &si, fence), "vkQueueSubmit");
	CHECK(vkWaitForFences(dev, 1, &fence, VK_TRUE, 10ull*1000*1000*1000), "vkWaitForFences");
	for(i = 0; i < size/4; i++)
		if(p[i] != 0x9f5a9f5a)
			break;
	if(i != size/4)
		FAIL("vkCmdFillBuffer: word %u is %#x", i, p[i]);
	else
		OK("vkCmdFillBuffer on the GPU: %llu bytes read back", (unsigned long long)size);

	/* the other way: the CPU writes buffer a, the GPU copies it to b */
	{
		VkBuffer b2;
		VkDeviceMemory mem2;
		uint32_t *p2;

		CHECK(vkCreateBuffer(dev, &bci, NULL, &b2), "vkCreateBuffer b");
		CHECK(vkAllocateMemory(dev, &mai, NULL, &mem2), "vkAllocateMemory b");
		CHECK(vkBindBufferMemory(dev, b2, mem2, 0), "vkBindBufferMemory b");
		CHECK(vkMapMemory(dev, mem2, 0, size, 0, (void**)&p2), "vkMapMemory b");
		memset(p2, 0, size);
		for(i = 0; i < size/4; i++)
			p[i] = i*2654435761u;
		vkResetFences(dev, 1, &fence);
		vkResetCommandBuffer(cb, 0);
		CHECK(vkBeginCommandBuffer(cb, &cbi), "vkBeginCommandBuffer");
		VkBufferCopy bc = { 0, 0, size };
		vkCmdCopyBuffer(cb, buf, b2, 1, &bc);
		CHECK(vkEndCommandBuffer(cb), "vkEndCommandBuffer");
		CHECK(vkQueueSubmit(q, 1, &si, fence), "vkQueueSubmit");
		CHECK(vkWaitForFences(dev, 1, &fence, VK_TRUE, 10ull*1000*1000*1000), "vkWaitForFences");
		for(i = 0; i < size/4; i++)
			if(p2[i] != i*2654435761u)
				break;
		if(i != size/4)
			FAIL("CPU-written buffer copied by the GPU: word %u is %#x, want %#x", i, p2[i], i*2654435761u);
		else
			OK("vkCmdCopyBuffer of a CPU-written buffer: %llu bytes", (unsigned long long)size);

		/* buffer a -> 128x128 image -> buffer b */
		VkImage im;
		VkImageCreateInfo ii = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = VK_FORMAT_B8G8R8A8_UNORM,
			.extent = { 128, 128, 1 },
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		};
		CHECK(vkCreateImage(dev, &ii, NULL, &im), "vkCreateImage");
		VkMemoryRequirements ir;
		vkGetImageMemoryRequirements(dev, im, &ir);
		uint32_t it;
		for(it = 0; it < mp.memoryTypeCount; it++)
			if(ir.memoryTypeBits & (1u<<it))
				break;
		VkMemoryAllocateInfo imai = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = ir.size,
			.memoryTypeIndex = it,
		};
		VkDeviceMemory imem;
		CHECK(vkAllocateMemory(dev, &imai, NULL, &imem), "vkAllocateMemory image");
		CHECK(vkBindImageMemory(dev, im, imem, 0), "vkBindImageMemory");
		memset(p2, 0, size);
		vkResetFences(dev, 1, &fence);
		vkResetCommandBuffer(cb, 0);
		CHECK(vkBeginCommandBuffer(cb, &cbi), "vkBeginCommandBuffer");
		VkImageMemoryBarrier ib = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = im,
			.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
		};
		vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 0, NULL, 0, NULL, 1, &ib);
		VkBufferImageCopy bic = {
			.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
			.imageExtent = { 128, 128, 1 },
		};
		vkCmdCopyBufferToImage(cb, buf, im, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bic);
		ib.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		ib.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		ib.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		ib.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 0, NULL, 0, NULL, 1, &ib);
		vkCmdCopyImageToBuffer(cb, im, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, b2, 1, &bic);
		CHECK(vkEndCommandBuffer(cb), "vkEndCommandBuffer");
		CHECK(vkQueueSubmit(q, 1, &si, fence), "vkQueueSubmit");
		CHECK(vkWaitForFences(dev, 1, &fence, VK_TRUE, 10ull*1000*1000*1000), "vkWaitForFences");
		for(i = 0; i < 128*128; i++)
			if(p2[i] != i*2654435761u)
				break;
		if(i != 128*128)
			FAIL("buffer -> image -> buffer: word %u is %#x, want %#x", i, p2[i], i*2654435761u);
		else
			OK("vkCmdCopyBufferToImage and back: 128x128");
	}
	vkDeviceWaitIdle(dev);
	vkDestroyDevice(dev, NULL);
	dev = VK_NULL_HANDLE;
	vkDestroyInstance(inst, NULL);
	inst = VK_NULL_HANDLE;
	OK("destroyed");
out:
	printf("vktest: %s\n", nfail ? "FAIL" : "all passed");
	return nfail ? 1 : 0;
}
