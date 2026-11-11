/*
 * SPDX-License-Identifier: MIT
 *
 * vktri: a red triangle on dark blue, drawn with a graphics pipeline
 * (shaders/tri.vert, tri.frag; SPIR-V compiled on the host with glslc)
 * into a VK_PLAN9_surface swapchain. An ordinary Vulkan program: it
 * calls vk* and links -lvulkan, the static loader with the driver in
 * it. Runs for the seconds given (default 3), then checks the pixels.
 *
 * usage: vktri [seconds]
 * VKTRI_AGAIN=1: on VK_ERROR_OUT_OF_DATE_KHR a new swapchain and on (a window
 * that outlives its wsys: tests/gpu/wsysagain.rc), not the end
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define VK_USE_PLATFORM_PLAN9_9FRONT
#include <vulkan/vulkan.h>

extern int screenpeek(int, int, unsigned *);	/* screenpeek.c */

/* the same colour, give or take what a 16-bit screen loses */
static int
near(unsigned a, unsigned b)
{
	int i, d;

	for(i = 0; i < 24; i += 8){
		d = (int)(a>>i & 0xff) - (int)(b>>i & 0xff);
		if(d < -8 || d > 8)
			return 0;
	}
	return 1;
}

#define FAIL(...) do { printf("FAIL "); printf(__VA_ARGS__); printf("\n"); exit(1); } while(0)
#define CHECK(call, what) do { VkResult r_ = (call); if(r_ != VK_SUCCESS && r_ != VK_SUBOPTIMAL_KHR) FAIL("%s: VkResult %d", what, r_); } while(0)

static const uint32_t vertspv[] =
#include "tri.vert.inc"
;
static const uint32_t fragspv[] =
#include "tri.frag.inc"
;

static double
now(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec/1e9;
}

static VkShaderModule
shader(VkDevice dev, const uint32_t *code, size_t size)
{
	VkShaderModule m;
	VkShaderModuleCreateInfo ci = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = size,
		.pCode = code,
	};

	CHECK(vkCreateShaderModule(dev, &ci, NULL, &m), "vkCreateShaderModule");
	return m;
}

/*
 * a new swapchain for the same window (the WSI said VK_SUBOPTIMAL_KHR: in
 * wsys, its images go to the overlay plane now, or not any more), its
 * images' views and framebuffers
 */
static void
rebuild(VkDevice dev, VkSwapchainCreateInfoKHR *swci, VkSwapchainKHR *sc, VkImage *img, uint32_t *nimg,
	VkImageView *view, VkFramebuffer *fb, VkRenderPass rp)
{
	VkSwapchainKHR old = *sc;
	uint32_t i;

	vkDeviceWaitIdle(dev);
	for(i = 0; i < *nimg; i++){
		vkDestroyFramebuffer(dev, fb[i], NULL);
		vkDestroyImageView(dev, view[i], NULL);
	}
	swci->oldSwapchain = old;
	CHECK(vkCreateSwapchainKHR(dev, swci, NULL, sc), "vkCreateSwapchainKHR (again)");
	vkDestroySwapchainKHR(dev, old, NULL);
	*nimg = 8;
	CHECK(vkGetSwapchainImagesKHR(dev, *sc, nimg, img), "swapchain images");
	for(i = 0; i < *nimg; i++){
		VkImageViewCreateInfo vci = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = img[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = swci->imageFormat,
			.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
		};
		CHECK(vkCreateImageView(dev, &vci, NULL, &view[i]), "vkCreateImageView");
		VkFramebufferCreateInfo fci = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = rp,
			.attachmentCount = 1,
			.pAttachments = &view[i],
			.width = swci->imageExtent.width,
			.height = swci->imageExtent.height,
			.layers = 1,
		};
		CHECK(vkCreateFramebuffer(dev, &fci, NULL, &fb[i]), "vkCreateFramebuffer");
	}
}

int
main(int argc, char **argv)
{
	VkInstance inst;
	VkDevice dev;
	VkSurfaceKHR surf;
	VkSwapchainKHR sc;
	uint32_t n, i, qf, nimg, frames, apiver, nrebuilt = 0;
	int again;
	double secs, t0;

	secs = argc > 1 ? atof(argv[1]) : 3;
	again = getenv("VKTRI_AGAIN") != NULL;

	/* the global commands, and one through vkGetInstanceProcAddr */
	PFN_vkEnumerateInstanceVersion EnumerateInstanceVersion =
		(PFN_vkEnumerateInstanceVersion)vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");
	if(EnumerateInstanceVersion == NULL)
		FAIL("vkGetInstanceProcAddr(NULL, vkEnumerateInstanceVersion)");
	CHECK(EnumerateInstanceVersion(&apiver), "vkEnumerateInstanceVersion");
	CHECK(vkEnumerateInstanceExtensionProperties(NULL, &n, NULL), "instance extensions");
	printf("ok loader: instance version %u.%u, %u instance extensions\n",
		VK_API_VERSION_MAJOR(apiver), VK_API_VERSION_MINOR(apiver), n);

	const char *iext[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_PLAN9_SURFACE_EXTENSION_NAME };
	VkApplicationInfo app = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "vktri",
		.apiVersion = VK_API_VERSION_1_1,
	};
	VkInstanceCreateInfo ici = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app,
		.enabledExtensionCount = 2,
		.ppEnabledExtensionNames = iext,
	};
	CHECK(vkCreateInstance(&ici, NULL, &inst), "vkCreateInstance");

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
	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(pd, &props);
	printf("ok device %s\n", props.deviceName);
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
	if(vkGetDeviceProcAddr(dev, "vkCmdDraw") == NULL)
		FAIL("vkGetDeviceProcAddr(vkCmdDraw)");
	VkQueue q;
	vkGetDeviceQueue(dev, qf, 0, &q);

	const VkFormat fmt = VK_FORMAT_B8G8R8A8_UNORM;
	VkExtent2D ext = caps.currentExtent;
	VkSwapchainCreateInfoKHR swci = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = surf,
		.minImageCount = caps.minImageCount < 2 ? 2 : caps.minImageCount,
		.imageFormat = fmt,
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageExtent = ext,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
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

	/* render pass: clear, draw, present */
	VkAttachmentDescription att = {
		.format = fmt,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	};
	VkAttachmentReference ref = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	VkSubpassDescription sub = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &ref,
	};
	VkSubpassDependency dep = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	};
	VkRenderPassCreateInfo rpci = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &att,
		.subpassCount = 1,
		.pSubpasses = &sub,
		.dependencyCount = 1,
		.pDependencies = &dep,
	};
	VkRenderPass rp;
	CHECK(vkCreateRenderPass(dev, &rpci, NULL, &rp), "vkCreateRenderPass");

	VkImageView view[8];
	VkFramebuffer fb[8];
	for(i = 0; i < nimg; i++){
		VkImageViewCreateInfo vci = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = img[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = fmt,
			.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
		};
		CHECK(vkCreateImageView(dev, &vci, NULL, &view[i]), "vkCreateImageView");
		VkFramebufferCreateInfo fci = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = rp,
			.attachmentCount = 1,
			.pAttachments = &view[i],
			.width = ext.width,
			.height = ext.height,
			.layers = 1,
		};
		CHECK(vkCreateFramebuffer(dev, &fci, NULL, &fb[i]), "vkCreateFramebuffer");
	}

	/* the pipeline */
	VkPushConstantRange pcr = { VK_SHADER_STAGE_FRAGMENT_BIT, 0, 4*sizeof(float) };
	VkPipelineLayoutCreateInfo plci = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pushConstantRangeCount = 1,
		.pPushConstantRanges = &pcr,
	};
	VkPipelineLayout pl;
	CHECK(vkCreatePipelineLayout(dev, &plci, NULL, &pl), "vkCreatePipelineLayout");
	VkPipelineShaderStageCreateInfo stages[2] = {{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = shader(dev, vertspv, sizeof vertspv),
		.pName = "main",
	}, {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = shader(dev, fragspv, sizeof fragspv),
		.pName = "main",
	}};
	VkPipelineVertexInputStateCreateInfo vis = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
	};
	VkPipelineInputAssemblyStateCreateInfo ias = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	};
	VkViewport vp = { 0, 0, ext.width, ext.height, 0, 1 };
	VkRect2D scissor = { { 0, 0 }, ext };
	VkPipelineViewportStateCreateInfo vps = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &vp,
		.scissorCount = 1,
		.pScissors = &scissor,
	};
	VkPipelineRasterizationStateCreateInfo rs = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_NONE,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.lineWidth = 1.0f,
	};
	VkPipelineMultisampleStateCreateInfo ms = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
	};
	VkPipelineColorBlendAttachmentState cba = {
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT|VK_COLOR_COMPONENT_G_BIT|
			VK_COLOR_COMPONENT_B_BIT|VK_COLOR_COMPONENT_A_BIT,
	};
	VkPipelineColorBlendStateCreateInfo cbs = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &cba,
	};
	VkGraphicsPipelineCreateInfo gpci = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = 2,
		.pStages = stages,
		.pVertexInputState = &vis,
		.pInputAssemblyState = &ias,
		.pViewportState = &vps,
		.pRasterizationState = &rs,
		.pMultisampleState = &ms,
		.pColorBlendState = &cbs,
		.layout = pl,
		.renderPass = rp,
		.subpass = 0,
	};
	VkPipeline pipe;
	CHECK(vkCreateGraphicsPipelines(dev, VK_NULL_HANDLE, 1, &gpci, NULL, &pipe), "vkCreateGraphicsPipelines");
	printf("ok graphics pipeline\n");

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

	t0 = now();
	for(frames = 0; frames == 0 || now() - t0 < secs; frames++){
		uint32_t idx;
		VkResult r = vkAcquireNextImageKHR(dev, sc, UINT64_MAX, acquired, VK_NULL_HANDLE, &idx);
		if(r == VK_ERROR_OUT_OF_DATE_KHR && again){
			/* VKTRI_AGAIN: a new swapchain, on (its wsys gone: in the next one) */
			rebuild(dev, &swci, &sc, img, &nimg, view, fb, rp);
			printf("ok a new swapchain after out of date (frame %u)\n", frames);
			fflush(stdout);
			continue;
		}
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
		VkClearValue clear = { .color = { .float32 = { 0.0f, 0.0f, 0.4f, 1.0f } } };
		VkRenderPassBeginInfo rbi = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = rp,
			.framebuffer = fb[idx],
			.renderArea = { { 0, 0 }, ext },
			.clearValueCount = 1,
			.pClearValues = &clear,
		};
		vkCmdBeginRenderPass(cb, &rbi, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe);
		static const float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		vkCmdPushConstants(cb, pl, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof red, red);
		vkCmdDraw(cb, 3, 1, 0, 0);
		vkCmdEndRenderPass(cb);
		CHECK(vkEndCommandBuffer(cb), "end");

		VkPipelineStageFlags ws = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
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
		if(r == VK_ERROR_OUT_OF_DATE_KHR && again){
			CHECK(vkWaitForFences(dev, 1, &fence, VK_TRUE, UINT64_MAX), "fence");
			vkResetFences(dev, 1, &fence);
			rebuild(dev, &swci, &sc, img, &nimg, view, fb, rp);
			printf("ok a new swapchain after out of date (frame %u)\n", frames+1);
			fflush(stdout);
			continue;
		}
		if(r == VK_ERROR_OUT_OF_DATE_KHR){
			printf("ok window changed size after %u frames\n", frames+1);
			break;
		}
		CHECK(r, "vkQueuePresentKHR");
		CHECK(vkWaitForFences(dev, 1, &fence, VK_TRUE, UINT64_MAX), "fence");
		vkResetFences(dev, 1, &fence);
		/*
		 * VKTRI_NOREBUILD=1: one new swapchain (in wsys: onto the overlay
		 * plane, images in VRAM), then VK_SUBOPTIMAL_KHR ignored, as many
		 * apps: covered, the WSI's out of date must come
		 */
		if(r == VK_SUBOPTIMAL_KHR && (getenv("VKTRI_NOREBUILD") == NULL || nrebuilt++ == 0)){
			rebuild(dev, &swci, &sc, img, &nimg, view, fb, rp);
			printf("ok a new swapchain after %u frames (suboptimal)\n", frames+1);
		}
	}
	vkDeviceWaitIdle(dev);
	double t1 = now();

	/*
	 * the triangle spans x -0.5..0.5, y -0.5..0.5 (y down): its centroid
	 * is at (0, -1/6); check before printing (the kernel's console
	 * draws on the screen)
	 */
	{
		int w = ext.width, h = ext.height;
		struct { int x, y; unsigned rgb; char *what; } want[] = {
			{ w/2, h*5/12, 0xff0000, "triangle" },
			{ w/2, h*13/16, 0x000066, "below the triangle" },
			{ w/8, h/2, 0x000066, "left of the triangle" },
			{ w*13/16, h/5, 0x000066, "above the triangle" },
		};
		unsigned rgb[4];
		int r[4];

		for(i = 0; i < 4; i++)
			r[i] = screenpeek(want[i].x, want[i].y, &rgb[i]);
		printf("ok %u frames in %.1f s\n", frames, t1 - t0);
		for(i = 0; i < 4; i++){
			if(r[i] < 0)
				FAIL("screenpeek %d,%d", want[i].x, want[i].y);
			if(!near(rgb[i], want[i].rgb))
				FAIL("screen at %d,%d is %06x, want %06x (%s)", want[i].x, want[i].y,
					rgb[i], want[i].rgb, want[i].what);
			printf("ok screen at %d,%d: %06x (%s)\n", want[i].x, want[i].y, rgb[i], want[i].what);
		}
	}

	vkDestroyPipeline(dev, pipe, NULL);
	vkDestroySwapchainKHR(dev, sc, NULL);
	vkDestroyDevice(dev, NULL);
	vkDestroySurfaceKHR(inst, surf, NULL);
	vkDestroyInstance(inst, NULL);
	printf("vktri: all passed\n");
	return 0;
}
