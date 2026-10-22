/*
 * SPDX-License-Identifier: MIT
 *
 * vkdraw: a Vulkan swapchain on 9front (VK_PLAN9_surface): every frame
 * clears the image dark blue and copies a red and a green 200x200
 * square into it from a host buffer, then presents it to the window
 * (or the screen, without rio). Runs for the seconds given (default 5);
 * tests/gpu/run.sh checks the screen dump. No shaders: transfers only.
 *
 * usage: vkdraw [seconds]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define VK_USE_PLATFORM_PLAN9_9FRONT
#include <vulkan/vulkan.h>

extern PFN_vkVoidFunction vk_icdGetInstanceProcAddr(VkInstance, const char *);
extern VkResult vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t *);
extern int screenpeek(int, int, unsigned *);	/* screenpeek.c */

#define FAIL(...) do { printf("FAIL "); printf(__VA_ARGS__); printf("\n"); exit(1); } while(0)
#define CHECK(call, what) do { VkResult r_ = (call); if(r_ != VK_SUCCESS && r_ != VK_SUBOPTIMAL_KHR) FAIL("%s: VkResult %d", what, r_); } while(0)
#define IFN(name) PFN_##name name = (PFN_##name)vk_icdGetInstanceProcAddr(inst, #name); \
	if(name == NULL) FAIL("no %s", #name)
#define DFN(name) PFN_##name name = (PFN_##name)GetDeviceProcAddr(dev, #name); \
	if(name == NULL) FAIL("no %s", #name)

enum { Sq = 200 };

static double
now(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec/1e9;
}

int
main(int argc, char **argv)
{
	VkInstance inst;
	VkDevice dev;
	VkSurfaceKHR surf;
	VkSwapchainKHR sc;
	uint32_t ver = 5, n, i, qf, mt, nimg, frames;
	double secs, t0;

	secs = argc > 1 ? atof(argv[1]) : 5;
	if(vk_icdNegotiateLoaderICDInterfaceVersion(&ver) != VK_SUCCESS)
		FAIL("negotiate");
	PFN_vkCreateInstance CreateInstance =
		(PFN_vkCreateInstance)vk_icdGetInstanceProcAddr(NULL, "vkCreateInstance");
	const char *iext[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_PLAN9_SURFACE_EXTENSION_NAME };
	VkApplicationInfo app = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "vkdraw",
		.apiVersion = VK_API_VERSION_1_1,
	};
	VkInstanceCreateInfo ici = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app,
		.enabledExtensionCount = 2,
		.ppEnabledExtensionNames = iext,
	};
	CHECK(CreateInstance(&ici, NULL, &inst), "vkCreateInstance");

	IFN(vkCreatePlan9SurfacePLAN9);
	IFN(vkEnumeratePhysicalDevices);
	IFN(vkGetPhysicalDeviceQueueFamilyProperties);
	IFN(vkGetPhysicalDeviceMemoryProperties);
	IFN(vkGetPhysicalDeviceSurfaceSupportKHR);
	IFN(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
	IFN(vkGetPhysicalDeviceSurfaceFormatsKHR);
	IFN(vkCreateDevice);
	PFN_vkGetDeviceProcAddr GetDeviceProcAddr =
		(PFN_vkGetDeviceProcAddr)vk_icdGetInstanceProcAddr(inst, "vkGetDeviceProcAddr");

	VkPlan9SurfaceCreateInfoPLAN9 sci = {
		.sType = VK_STRUCTURE_TYPE_PLAN9_SURFACE_CREATE_INFO_PLAN9,
		.pWindowName = NULL,
	};
	CHECK(vkCreatePlan9SurfacePLAN9(inst, &sci, NULL, &surf), "vkCreatePlan9SurfacePLAN9");

	VkPhysicalDevice pd;
	n = 1;
	vkEnumeratePhysicalDevices(inst, &n, &pd);
	if(n == 0)
		FAIL("no physical device");
	VkQueueFamilyProperties qfp[16];
	n = 16;
	vkGetPhysicalDeviceQueueFamilyProperties(pd, &n, qfp);
	for(qf = 0; qf < n; qf++){
		VkBool32 ok = 0;
		vkGetPhysicalDeviceSurfaceSupportKHR(pd, qf, surf, &ok);
		if(ok && (qfp[qf].queueFlags & VK_QUEUE_GRAPHICS_BIT))
			break;
	}
	if(qf == n)
		FAIL("no queue family can present");

	VkSurfaceCapabilitiesKHR caps;
	CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pd, surf, &caps), "surface capabilities");
	VkSurfaceFormatKHR fmts[8];
	n = 8;
	CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surf, &n, fmts), "surface formats");
	printf("ok surface %ux%u, %u formats, first %d\n", caps.currentExtent.width,
		caps.currentExtent.height, n, fmts[0].format);

	float prio = 1.0f;
	const char *dext[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
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
		.enabledExtensionCount = 1,
		.ppEnabledExtensionNames = dext,
	};
	CHECK(vkCreateDevice(pd, &dci, NULL, &dev), "vkCreateDevice");

	DFN(vkGetDeviceQueue);
	DFN(vkCreateSwapchainKHR);
	DFN(vkGetSwapchainImagesKHR);
	DFN(vkAcquireNextImageKHR);
	DFN(vkQueuePresentKHR);
	DFN(vkCreateBuffer);
	DFN(vkGetBufferMemoryRequirements);
	DFN(vkAllocateMemory);
	DFN(vkBindBufferMemory);
	DFN(vkMapMemory);
	DFN(vkCreateCommandPool);
	DFN(vkAllocateCommandBuffers);
	DFN(vkResetCommandBuffer);
	DFN(vkBeginCommandBuffer);
	DFN(vkEndCommandBuffer);
	DFN(vkCmdPipelineBarrier);
	DFN(vkCmdClearColorImage);
	DFN(vkCmdCopyBufferToImage);
	DFN(vkCreateSemaphore);
	DFN(vkCreateFence);
	DFN(vkWaitForFences);
	DFN(vkResetFences);
	DFN(vkQueueSubmit);
	DFN(vkDeviceWaitIdle);

	VkQueue q;
	vkGetDeviceQueue(dev, qf, 0, &q);

	VkExtent2D ext = caps.currentExtent;
	VkSwapchainCreateInfoKHR swci = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = surf,
		.minImageCount = caps.minImageCount < 2 ? 2 : caps.minImageCount,
		.imageFormat = VK_FORMAT_B8G8R8A8_UNORM,
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageExtent = ext,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_FIFO_KHR,
		.clipped = VK_TRUE,
	};
	CHECK(vkCreateSwapchainKHR(dev, &swci, NULL, &sc), "vkCreateSwapchainKHR");
	VkImage img[8];
	nimg = 8;
	CHECK(vkGetSwapchainImagesKHR(dev, sc, &nimg, img), "swapchain images");
	printf("ok swapchain of %u images %ux%u\n", nimg, ext.width, ext.height);

	/* the squares: a host buffer, red then green, B,G,R,A bytes */
	VkBuffer buf;
	VkBufferCreateInfo bci = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = 2*Sq*Sq*4,
		.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	};
	CHECK(vkCreateBuffer(dev, &bci, NULL, &buf), "vkCreateBuffer");
	VkMemoryRequirements req;
	vkGetBufferMemoryRequirements(dev, buf, &req);
	VkPhysicalDeviceMemoryProperties mp;
	vkGetPhysicalDeviceMemoryProperties(pd, &mp);
	const VkMemoryPropertyFlags want = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for(mt = 0; mt < mp.memoryTypeCount; mt++)
		if((req.memoryTypeBits & (1u<<mt)) && (mp.memoryTypes[mt].propertyFlags & want) == want)
			break;
	if(mt == mp.memoryTypeCount)
		FAIL("no host visible memory");
	VkDeviceMemory mem;
	VkMemoryAllocateInfo mai = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = req.size,
		.memoryTypeIndex = mt,
	};
	CHECK(vkAllocateMemory(dev, &mai, NULL, &mem), "vkAllocateMemory");
	CHECK(vkBindBufferMemory(dev, buf, mem, 0), "vkBindBufferMemory");
	uint32_t *px;
	CHECK(vkMapMemory(dev, mem, 0, bci.size, 0, (void**)&px), "vkMapMemory");
	for(i = 0; i < Sq*Sq; i++){
		px[i] = 0xffff0000;		/* red: B=0 G=0 R=ff A=ff */
		px[Sq*Sq + i] = 0xff00ff00;	/* green */
	}

	VkCommandPool pool;
	VkCommandPoolCreateInfo pci = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
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
	VkSemaphore acquired, done;
	VkSemaphoreCreateInfo semi = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	CHECK(vkCreateSemaphore(dev, &semi, NULL, &acquired), "semaphore");
	CHECK(vkCreateSemaphore(dev, &semi, NULL, &done), "semaphore");
	VkFence fence;
	VkFenceCreateInfo fci = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	CHECK(vkCreateFence(dev, &fci, NULL, &fence), "fence");

	const VkImageSubresourceRange all = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	t0 = now();
	for(frames = 0; frames == 0 || now() - t0 < secs; frames++){
		uint32_t idx;
		VkResult r = vkAcquireNextImageKHR(dev, sc, UINT64_MAX, acquired, VK_NULL_HANDLE, &idx);
		if(r == VK_ERROR_OUT_OF_DATE_KHR){
			printf("ok window changed size after %u frames\n", frames);
			break;
		}
		CHECK(r, "vkAcquireNextImageKHR");

		vkResetCommandBuffer(cb, 0);
		VkCommandBufferBeginInfo cbi = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		};
		CHECK(vkBeginCommandBuffer(cb, &cbi), "begin");
		VkImageMemoryBarrier b = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = img[idx],
			.subresourceRange = all,
		};
		vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 0, NULL, 0, NULL, 1, &b);
		VkClearColorValue blue = { .float32 = { 0.0f, 0.0f, 0.4f, 1.0f } };
		vkCmdClearColorImage(cb, img[idx], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &blue, 1, &all);
		VkMemoryBarrier wb = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
			.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
		};
		vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 1, &wb, 0, NULL, 0, NULL);
		VkBufferImageCopy cp[2] = {{
			.bufferOffset = 0,
			.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
			.imageOffset = { 100, 350, 0 },
			.imageExtent = { Sq, Sq, 1 },
		}, {
			.bufferOffset = Sq*Sq*4,
			.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
			.imageOffset = { 400, 350, 0 },
			.imageExtent = { Sq, Sq, 1 },
		}};
		vkCmdCopyBufferToImage(cb, buf, img[idx], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 2, cp);
		b.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		b.dstAccessMask = 0;
		b.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		b.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0, 0, NULL, 0, NULL, 1, &b);
		CHECK(vkEndCommandBuffer(cb), "end");

		VkPipelineStageFlags ws = VK_PIPELINE_STAGE_TRANSFER_BIT;
		VkSubmitInfo si = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &acquired,
			.pWaitDstStageMask = &ws,
			.commandBufferCount = 1,
			.pCommandBuffers = &cb,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &done,
		};
		CHECK(vkQueueSubmit(q, 1, &si, fence), "vkQueueSubmit");
		VkPresentInfoKHR pi = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &done,
			.swapchainCount = 1,
			.pSwapchains = &sc,
			.pImageIndices = &idx,
		};
		r = vkQueuePresentKHR(q, &pi);
		if(r == VK_ERROR_OUT_OF_DATE_KHR){
			printf("ok window changed size after %u frames\n", frames+1);
			break;
		}
		CHECK(r, "vkQueuePresentKHR");
		CHECK(vkWaitForFences(dev, 1, &fence, VK_TRUE, UINT64_MAX), "fence");
		vkResetFences(dev, 1, &fence);
	}
	vkDeviceWaitIdle(dev);
	double t1 = now();

	/* the last frame on the screen, before anyone writes to the console
	 * (the kernel draws it on the screen): check first, print after */
	{
		static const struct { int x, y; unsigned rgb; char *what; } want[] = {
			{ 200, 450, 0xff0000, "red square" },
			{ 500, 450, 0x00ff00, "green square" },
			{ 900, 100, 0x000066, "blue background" },
			{ 50, 700, 0x000066, "blue background" },
		};
		unsigned rgb[4];
		int r[4];

		for(i = 0; i < 4; i++)
			r[i] = screenpeek(want[i].x, want[i].y, &rgb[i]);
		printf("ok %u frames in %.1f s\n", frames, t1 - t0);
		for(i = 0; i < 4; i++){
			if(r[i] < 0)
				FAIL("screenpeek %d,%d", want[i].x, want[i].y);
			if(rgb[i] != want[i].rgb)
				FAIL("screen at %d,%d is %06x, want %06x (%s)", want[i].x, want[i].y,
					rgb[i], want[i].rgb, want[i].what);
			printf("ok screen at %d,%d: %06x (%s)\n", want[i].x, want[i].y, rgb[i], want[i].what);
		}
	}

	printf("vkdraw: all passed\n");
	return 0;
}
